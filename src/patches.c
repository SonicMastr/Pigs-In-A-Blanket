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
#include <psp2/display.h>
#include "../include/patches.h"
#include "../include/hooks.h"
#include "../include/debug.h"

#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_STENCIL_BUFFER_BIT 0x00000400
#define GL_COLOR_BUFFER_BIT 0x00004000

static int swap_interval = 1;
static int clear = 0, clearDepth = 0, clearStencil = 0;

void glGetBooleanv_shaderCompilerPatch(unsigned int pname, unsigned char *data)
{
    TAI_CONTINUE(void, hookRef[1], pname, data);
    if (pname == 0x8DFA)
        *data = 1;
}

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

    TAI_CONTINUE(void, hookRef[2], textureUploadParams);
}

int eglCreateWindowSurface_resolutionPatch(int dpy, int config, int win, int *attrib_list)
{
    if (customResolutionMode)
        win = customResolutionMode;
    switch (win)
    {
        case 5:
            *(int16_t *)(dpy + 0x26) = 1280;
            *(int16_t *)(dpy + 0x28) = 725;
            *(int16_t *)(dpy + 0x2A) = 1280;
            break;
        case 6:
            *(int16_t *)(dpy + 0x26) = 1920;
            *(int16_t *)(dpy + 0x28) = 1088;
            *(int16_t *)(dpy + 0x2A) = 1920;
            break;
        case 7:
            *(int16_t *)(dpy + 0x26) = 960;
            *(int16_t *)(dpy + 0x28) = 544;
            *(int16_t *)(dpy + 0x2A) = 960;
            break;
    }

    return TAI_CONTINUE(int, hookRef[3], dpy, config, win, attrib_list);
}

void *eglGetProcAddress_functionNamePatch(const char *procname)
{

    void *ret = TAI_CONTINUE(void*, hookRef[4], procname);

    if(ret)
        return ret; // Got an Extension function address. No need to do anything else.

    char digest[21];
    SHA1(digest, procname, strlen(procname)); // This may be slow. Look into different solutions
    
    void *function;
    
    if(taiGetModuleExportFunc("libScePiglet", 0xB4FE1ABB, *(uint32_t*)(&digest), (uintptr_t *)&function) < 0)
        return NULL;

    return function;
}

unsigned int eglGetConfigAttrib_intervalPatch(void *display, void *config, int attrib, int *value)
{
    unsigned int ret = TAI_CONTINUE(unsigned int, hookRef[5], display, config, attrib, value);

    if (ret)
    {
        if (attrib == 0x303B) // EGL_MIN_SWAP_INTERVAL
            *value = 0;

        if (attrib == 0x303C) // EGL_MAX_SWAP_INTERVAL
            *value = 4;
    }

    return ret;
}

unsigned int pglDisplaySetSwapInterval_intervalPatch(void *display, int interval) 
{
    unsigned int ret = TAI_CONTINUE(int, hookRef[6], display, interval);

    if (ret)
    {
        if (interval < 0)
            interval = 0;

        if (interval > 4)
            interval = 4;

        swap_interval = interval;
    }
        
    return ret;
}

int sceDisplayWaitVblankStart_intervalPatch(void)
{
    return sceDisplayWaitVblankStartMulti(swap_interval);
}

SceGxmErrorCode sceGxmColorSurfaceInit_msaaPatch(SceGxmColorSurface *surface,
                                                    SceGxmColorFormat colorFormat,
                                                    SceGxmColorSurfaceType surfaceType,
                                                    SceGxmColorSurfaceScaleMode scaleMode,
                                                    SceGxmOutputRegisterSize outputRegisterSize,
                                                    uint32_t width,
                                                    uint32_t height,
                                                    uint32_t strideInPixels,
                                                    void *data)
{
    if (isCreatingSurface)
        scaleMode = SCE_GXM_COLOR_SURFACE_SCALE_MSAA_DOWNSCALE;
    return TAI_CONTINUE(SceGxmErrorCode, hookRef[16], surface, colorFormat, surfaceType, scaleMode, outputRegisterSize, width, height, strideInPixels, data);
}

SceGxmErrorCode sceGxmCreateRenderTarget_msaaPatch(const SceGxmRenderTargetParams *params, SceGxmRenderTarget **renderTarget)
{
    SceGxmRenderTargetParams renderTargetParams;
	memset(&renderTargetParams, 0, 0x14);
	renderTargetParams.flags = 0;
	renderTargetParams.width = params->width;
	renderTargetParams.height = params->height;
    renderTargetParams.multisampleMode = isCreatingSurface ? SCE_GXM_MULTISAMPLE_4X : SCE_GXM_MULTISAMPLE_NONE;
	renderTargetParams.scenesPerFrame = 1;
	renderTargetParams.multisampleLocations = 0;
	renderTargetParams.driverMemBlock = -1;
    return TAI_CONTINUE(SceGxmErrorCode, hookRef[17], &renderTargetParams, renderTarget);
}

SceGxmErrorCode sceGxmDepthStencilSurfaceInit_msaaPatch(SceGxmDepthStencilSurface *surface,
                                                            SceGxmDepthStencilFormat depthStencilFormat,
                                                            SceGxmDepthStencilSurfaceType surfaceType,
                                                            uint32_t strideInSamples,
                                                            void *depthData,
                                                            void *stencilData)
{
    if (isCreatingSurface)
        strideInSamples *= 2;
    return TAI_CONTINUE(SceGxmErrorCode, hookRef[18], surface, depthStencilFormat, surfaceType, strideInSamples, depthData, stencilData);
}

SceGxmErrorCode sceGxmShaderPatcherCreateFragmentProgram_msaaPatch(SceGxmShaderPatcher *shaderPatcher,
                                                                    SceGxmShaderPatcherId programId,
                                                                    SceGxmOutputRegisterFormat outputFormat,
                                                                    SceGxmMultisampleMode multisampleMode,
                                                                    const SceGxmBlendInfo *blendInfo,
                                                                    const SceGxmProgram *vertexProgram,
                                                                    SceGxmFragmentProgram **fragmentProgram)
{
    multisampleMode = SCE_GXM_MULTISAMPLE_4X;
    return TAI_CONTINUE(SceGxmErrorCode, hookRef[19], shaderPatcher, programId, outputFormat, multisampleMode, blendInfo, vertexProgram, fragmentProgram);
}

void glClear_loadPatch(unsigned int mask)
{
    if (mask & GL_DEPTH_BUFFER_BIT)
        clearDepth = 1;

    if (mask & GL_STENCIL_BUFFER_BIT)
        clearStencil = 1;

    clear = 1;

    TAI_CONTINUE(void, hookRef[20], mask);

    clear = 0;
}

SceGxmErrorCode sceGxmBeginScene_loadPatch(SceGxmContext *context, unsigned int flags, const SceGxmRenderTarget *renderTarget, const SceGxmValidRegion *validRegion, SceGxmSyncObject *vertexSyncObject, SceGxmSyncObject *fragmentSyncObject, const SceGxmColorSurface *colorSurface, const SceGxmDepthStencilSurface *depthStencil)
{
    if (!clear || !depthStencil || !sceGxmDepthStencilSurfaceIsEnabled(depthStencil))
        return TAI_CONTINUE(SceGxmErrorCode, hookRef[21], context, flags, renderTarget, validRegion, vertexSyncObject, fragmentSyncObject, colorSurface, depthStencil);

    size_t depthStencilFormat = sceGxmDepthStencilSurfaceGetFormat(depthStencil);

    switch(depthStencilFormat)
    {
    case SCE_GXM_DEPTH_STENCIL_FORMAT_DF32M:
    case SCE_GXM_DEPTH_STENCIL_FORMAT_D16:
        if (clearDepth)
            sceGxmDepthStencilSurfaceSetForceLoadMode(depthStencil, SCE_GXM_DEPTH_STENCIL_FORCE_LOAD_DISABLED);
        break;
    case SCE_GXM_DEPTH_STENCIL_FORMAT_DF32M_S8:
    case SCE_GXM_DEPTH_STENCIL_FORMAT_S8D24:
        if (clearDepth && clearStencil)
            sceGxmDepthStencilSurfaceSetForceLoadMode(depthStencil, SCE_GXM_DEPTH_STENCIL_FORCE_LOAD_DISABLED);
        break;
    case SCE_GXM_DEPTH_STENCIL_FORMAT_S8:
        if (clearStencil)
            sceGxmDepthStencilSurfaceSetForceLoadMode(depthStencil, SCE_GXM_DEPTH_STENCIL_FORCE_LOAD_DISABLED);
        break;
    }

    int ret = TAI_CONTINUE(SceGxmErrorCode, hookRef[21], context, flags, renderTarget, validRegion, vertexSyncObject, fragmentSyncObject, colorSurface, depthStencil);

    sceGxmDepthStencilSurfaceSetForceLoadMode(depthStencil, SCE_GXM_DEPTH_STENCIL_FORCE_LOAD_ENABLED);

    return ret;
}
