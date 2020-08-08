#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <psp2/types.h>
#include <psp2/kernel/modulemgr.h>
#include <psp2/shacccg.h>
#include <taihen.h>

tai_hook_ref_t hookRef;
SceShaccCgSourceFile source;
SceUID hook;
SceUID pigID, shID;
int shaccCgDisabled = 0;  // 0 Enabled, 1 Enabled Without Diagnostics, 2 Disabled

const SceShaccCgCompileOutput *output = NULL;

static void logShaccCg(const SceShaccCgCompileOutput *output)
{
    for (int i = 0; i < output->diagnosticCount; ++i) {
		const SceShaccCgDiagnosticMessage *log = &output->diagnostics[i];
        char diagnosticLevel[8];
        switch (log->level)
        {
            case SCE_SHACCCG_DIAGNOSTIC_LEVEL_INFO:
                strcpy(diagnosticLevel, "INFO");
                break;
            case SCE_SHACCCG_DIAGNOSTIC_LEVEL_WARNING:
                strcpy(diagnosticLevel, "WARNING");
                break;
            case SCE_SHACCCG_DIAGNOSTIC_LEVEL_ERROR:
                strcpy(diagnosticLevel, "ERROR");
                break;
        }
		printf("Line: %d, %s: %s\n", log->location->lineNumber, diagnosticLevel, log->message);
	}
}

static SceShaccCgSourceFile *openFile_callback(const char *filename, const SceShaccCgSourceLocation *includedFrom, const SceShaccCgCompileOptions *compileOptions, ScePVoid userData, const char **errorString)
{
    return &source;
}

int pglPlatformShaderCompiler_CustomPatch(int a1, void *shader)
{
    SceShaccCgCallbackList callback = {0};
    SceShaccCgCompileOptions options;

    sceShaccCgInitializeCallbackList(&callback, 1);
    callback.openFile = openFile_callback;
    sceShaccCgInitializeCompileOptions(&options);

    printf("All Options\n");

    int shaderType = *(int *)(shader + 0x1c);
    switch (shaderType)
    {
        case 1:
            options.targetProfile = SCE_SHACCCG_PROFILE_VP;
            break;
        case 2:
            options.targetProfile = SCE_SHACCCG_PROFILE_FP;
            break;
    }

    source.fileName = "";  // Crashes Otherwise. Name doesn't matter
    source.text = *(char **)(shader + 0x20);  // Shader Data Pointer
    source.size = *(int *)(shader + 0x24);  // Shader Data Size Pointer

    printf("%s : %d\n", source.text, source.size);

    options.mainSourceFile = source.fileName;
	options.entryFunctionName = "main";
	options.macroDefinitions = NULL;
    options.locale = 0;
	options.useFx = 0;
	options.warningLevel = 3;
	options.optimizationLevel = 3;  // Lol. Fuck you Rinne
	options.useFastmath = 1;
	options.useFastint = 1;
    options.warningsAsErrors = 0;
    options.useFastprecision = 0;
    options.pedantic = 0;
    options.performanceWarnings = 0;

    output = sceShaccCgCompileProgram(&options, &callback, 0);

    if (!shaccCgDisabled)
        logShaccCg(output);
    if (!output->programData)
    {
        sceShaccCgDestroyCompileOutput(output);
        return TAI_CONTINUE(int, hookRef, a1, shader);
    }
    printf("Before Malloc\n");
    SceUInt8 *shaderData = malloc(output->programSize);
    printf("After Malloc\n");
    memcpy(shaderData, output->programData, output->programSize);
    printf("After memcpy\n");
    *(SceUInt8**)(shader + 0x34) = shaderData;   // Compiled Shader Data Pointer
    printf("After shaderdata\n");
    *(SceInt32*)(shader + 0x38) = output->programSize;  // Compiled Shader Data Size Pointer
    printf("After programsize\n");
    *(int*)(&shader + 0x30) = 1;    // Flags Indicating Successful Compile
    *(int*)(&shader + 0x1d) = 2;

    printf("%s : %d : %d : %d\n", *(SceUInt8**)(shader + 0x34), *(SceInt32*)(shader + 0x38), *(int*)(&shader + 0x30), *(int*)(&shader + 0x1d));

    sceShaccCgDestroyCompileOutput(output);
    printf("Destroy\n");
    return 1;
}

static int loadModules(void)
{
    if (pigID = sceKernelLoadStartModule("ur0:data/external/libScePiglet.suprx", 0, SCE_NULL, 0, SCE_NULL, 0), pigID < 0)
        return -2;
    if (shaccCgDisabled != 2) {
        int status = 0;
        if (shID = sceKernelLoadStartModule("ur0:data/external/libshacccg.suprx", 0, SCE_NULL, 0, SCE_NULL, 0), shID < 0)
            return -1;
        printf("Status %d\n", status);
        sceShaccCgSetMemAllocator(malloc, free);
    }
    return 0;
}

static int unloadModules(void)
{
    sceKernelStopUnloadModule(pigID, 0, NULL, 0, NULL, 0);
    sceKernelStopUnloadModule(shID, 0, NULL, 0, NULL, 0);
    return 0;
}

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args)
{
    if (argc == 1) {
        switch (*(int*)args)
        {
            case 1:
                shaccCgDisabled = 1;
            case 2:
                shaccCgDisabled = 2;
        }
    }

    int ret;
    if (ret = loadModules(), ret)
        return ret;

    if (shaccCgDisabled != 2) {
        tai_module_info_t info;

        info.size = sizeof(info);
        taiGetModuleInfo("libScePiglet", &info);
        hook = taiHookFunctionOffset(&hookRef, info.modid, 0, 0x32BB4, 1, pglPlatformShaderCompiler_CustomPatch);
        printf("Hook 0x%08x\n", hook);
    }

    return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args)
{
    unloadModules();
    taiHookRelease(hook, hookRef);
	return SCE_KERNEL_STOP_SUCCESS;	
}