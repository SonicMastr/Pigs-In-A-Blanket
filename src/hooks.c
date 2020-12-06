/*****************************************************************************
 * 
 *  Copyright (c) 2020 by SonicMastr <sonicmastr@gmail.com>
 * 
 *  This file is part of Pigs In A Blanket
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 ****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <psp2/types.h>
#include "../include/shacccgpatch.h"
#include "../include/sysmodepatch.h"
#include "../include/hooks.h"
#include "../include/patches.h"
#include "../include/debug.h"

tai_hook_ref_t hookRef[NUM_HOOKS];
SceUID hook[NUM_HOOKS];
int customResolutionMode;
tai_module_info_t modInfo;

void loadHooks(PibOptions options)
{
    modInfo.size = sizeof(modInfo);
    taiGetModuleInfo("libScePiglet", &modInfo);
    if (options & PIB_SHACCCG) {
        hook[0] = taiHookFunctionOffset(&hookRef[0], modInfo.modid, 0, 0x32BB4, 1, pglPlatformShaderCompiler_CustomPatch);
        hook[1] = taiHookFunctionExport(&hookRef[1], modInfo.name, 0xB4FE1ABB, 0x919FBCB7, glGetBooleanv_shaderCompilerPatch);
        LOG("ShaccCg Patch: 0x%08x\nEnabled Shader Compiler Response: 0x%08x\n", hook[0], hook[1]);
    }
    hook[2] = taiHookFunctionOffset(&hookRef[2], modInfo.modid, 0, 0x39770, 1, _pglPlatformTextureUploadParams_patch);
    hook[3] = taiHookFunctionExport(&hookRef[3], modInfo.name, 0xB4FE1ABB, 0x4B86317A, eglCreateWindowSurface_resolutionPatch);
    LOG("Texture Upload Params Patch: 0x%08x\n", hook[2]);
    LOG("Resolution Patch: 0x%08x\n", hook[3]);
    if (options & PIB_GET_PROC_ADDR_CORE) {
        hook[4] = taiHookFunctionExport(&hookRef[4], modInfo.name, 0xB4FE1ABB, 0x249A431A, eglGetProcAddress_functionNamePatch);
        LOG("eglGetProcAddress Function Name Patch: 0x%08x\n", hook[4]);
    }
    hook[5] = taiHookFunctionExport(&hookRef[5], modInfo.name, 0xB4FE1ABB, 0x33A55EAB, eglGetConfigAttrib_intervalPatch);
    hook[6] = taiHookFunctionOffset(&hookRef[6], modInfo.modid, 0, 0x158F8, 1, pglDisplaySetSwapInterval_intervalPatch);
    hook[7] = taiHookFunctionImport(&hookRef[7], modInfo.name, 0x5ED8F994, 0x5795E898, sceDisplayWaitVblankStart_intervalPatch);
    LOG("Swap interval Patch: 0x%08x\nWaitVblankStart Patch: 0x%08X\n", hook[5], hook[6]);
    if (options & PIB_SYSTEM_MODE) {
        uint8_t cbnz_opcode = 0xB9;
        uint8_t mem_mode_two = 0x2;
        taiInjectData(modInfo.modid, 0, 0x33219, &cbnz_opcode, sizeof(cbnz_opcode));
        taiInjectData(modInfo.modid, 0, 0x2D2C0, &mem_mode_two, sizeof(mem_mode_two)); // Patch pglVitaMemoryAlloc to always use MAIN memblock
        taiInjectData(modInfo.modid, 0, 0x2D1DC, &mem_mode_two, sizeof(mem_mode_two)); //
        hook[8] = taiHookFunctionImport(&hookRef[8], modInfo.name, 0xF76B66BD, 0xB0F1E4EC, sceGxmInitialize_patch);
        hook[9] = taiHookFunctionOffset(&hookRef[9], modInfo.modid, 0, 0x17d24, 1, pglMemoryAllocAlign_patch);
        hook[10] = taiHookFunctionOffset(&hookRef[10], modInfo.modid, 0, 0x33074, 1, pglPlatformSurfaceCreateWindow_detect);
        hook[11] = taiHookFunctionImport(&hookRef[11], modInfo.name, 0xF76B66BD, 0x6A6013E1, sceGxmSyncObjectCreate_patch);
        hook[12] = taiHookFunctionOffset(&hookRef[12], modInfo.modid, 0, 0x2A85A, 1, pglPlatformContextBeginFrame_patch);
        hook[13] = taiHookFunctionOffset(&hookRef[13], modInfo.modid, 0, 0x33902, 1, pglPlatformSurfaceSwap_patch);
        hook[14] = taiHookFunctionOffset(&hookRef[14], modInfo.modid, 0, 0x337A6, 1, pglPlatformSurfaceDestroy_detect);
        hook[15] = taiHookFunctionImport(&hookRef[15], modInfo.name, 0xF76B66BD, 0x889AE88C, sceGxmSyncObjectDestroy_patch);
    }
}

void releaseHooks(void)
{
    for (int i = 0; i < NUM_HOOKS; i++)
    {
        taiHookRelease(hook[i], hookRef[i]);
    }
}
