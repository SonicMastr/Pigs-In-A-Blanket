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
#include "../include/patches.h"
#include "../include/hooks.h"

extern tai_hook_ref_t hookRef[NUM_HOOKS];
extern SceUID hook[NUM_HOOKS];

void _pglPlatformTextureUploadParams_patch(int textureUploadParams)
{
    if (*(int *)(textureUploadParams + 0xd0) != 0)
    {
        SceGxmTextureAddrMode addrMode = SCE_GXM_TEXTURE_ADDR_MIRROR_CLAMP;
        int ret;
        if (*(char *)(textureUploadParams + 0x38) != '\0') {
            if ((*(char *)(textureUploadParams + 0x20) != '\x01') && (*(int *)(textureUploadParams + 0xbc) != 0x1902)) {
                ret = *(int *)(textureUploadParams + 0x34);
                if (ret == 0x8370) {
                    ret = sceGxmTextureSetVAddrMode((SceGxmTexture *)(textureUploadParams + 0x490),addrMode);
                } else {
                    ret = 1;
                }
                if (ret == 0) {
                    *(char *)(textureUploadParams + 0x38) = 0;
                }
            }
        }
        if (*(char *)(textureUploadParams + 0x40) != '\0') {
            if ((*(char *)(textureUploadParams + 0x20) != '\x01') && (*(int *)(textureUploadParams + 0xbc) != 0x1902)) {
                ret = *(int *)(textureUploadParams + 0x3c);
                if (ret == 0x8370) {
                    ret = sceGxmTextureSetUAddrMode((SceGxmTexture *)(textureUploadParams + 0x490),addrMode);
                } else {
                    ret = 1;
                }
                if (ret == 0) {
                    *(char *)(textureUploadParams + 0x40) = 0;
                }
            }
        }
    }

    TAI_CONTINUE(void, hookRef[1], textureUploadParams);
}