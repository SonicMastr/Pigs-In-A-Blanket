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
#include "../include/shacccgPatch.h"
#include "../include/hooks.h"
#include "../include/patches.h"

tai_hook_ref_t hookRef[NUM_HOOKS];
SceUID hook[NUM_HOOKS];

void loadHooks(PibOptions *options)
{
    tai_module_info_t info;

    info.size = sizeof(info);
    taiGetModuleInfo("libScePiglet", &info);
    if (options->shaccCgEnabled) {
        hook[0] = taiHookFunctionOffset(&hookRef[0], info.modid, 0, 0x32BB4, 1, pglPlatformShaderCompiler_CustomPatch);
#ifdef DEBUG_MODE
        printf("Hook 0: 0x%08x\n", hook[0]);
#endif
    }
    hook[1] = taiHookFunctionOffset(&hookRef[1], info.modid, 0, 0x39770, 1, _pglPlatformTextureUploadParams_patch);
    hook[2] = taiHookFunctionExport(&hookRef[2], info.name, 0xB4FE1ABB, 0x4B86317A, eglCreateWindowSurface_resolutionPatch);
#ifdef DEBUG_MODE
    printf("Hook 1: 0x%08x\n", hook[1]);
    printf("Hook 2: 0x%08x\n", hook[2]);
#endif
}

void releaseHooks(void)
{
    for (int i = 0; i < NUM_HOOKS; i++)
    {
        taiHookRelease(hook[i], hookRef[i]);
    }
}