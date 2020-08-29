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
#include <psp2/kernel/modulemgr.h>
#include <psp2/shacccg.h>
#include <psp2/kernel/clib.h>
#include "../include/hooks.h"
#include <taihen.h>

static SceUID modID[4];

static int loadModules(PibOptions *options)
{
    if (!options->noStdLib) {
        if (modID[3] = sceKernelLoadStartModule("vs0:sys/external/libfios2.suprx", 0, SCE_NULL, 0, SCE_NULL, 0), modID[3] < 0 && modID[3] != 0x8002D014)
            return -4;
        if (modID[2] = sceKernelLoadStartModule("vs0:sys/external/libc.suprx", 0, SCE_NULL, 0, SCE_NULL, 0), modID[2] < 0 && modID[2] != 0x8002D014)
            return -3;
    }
    if (modID[1]= sceKernelLoadStartModule("ur0:data/external/libScePiglet.suprx", 0, SCE_NULL, 0, SCE_NULL, 0), modID[1] < 0)
        return -2;
    if (options->shaccCgEnabled) {
        printf("Shacc %d\n", options->shaccCgEnabled);
        if (modID[0] = sceKernelLoadStartModule("ur0:data/external/libshacccg.suprx", 0, SCE_NULL, 0, SCE_NULL, 0), modID[0] < 0)
            return -1;
        sceShaccCgSetMemAllocator(malloc, free);
    }
    return 0;
}

static void unloadModules(void)
{
    for (int i; i < 4; i++) {
        if (modID[i])
            sceKernelStopUnloadModule(modID[i], 0, NULL, 0, NULL, 0);
    }
}

#ifdef PLUGIN
void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize args, PibOptions *options)
#else
int pibInit(PibOptions *options)
#endif
{
    int ret;
    if (ret = loadModules(options), ret)
        return ret;
   
    loadHooks(options);
    return 0;
}

#ifdef PLUGIN
int module_stop(SceSize argc, const void *args)
#else
int pibTerm(void)
#endif
{
    unloadModules();
    releaseHooks();
    return 0;	
}