/*****************************************************************************
 * 
 *  Copyright (c) 2020 by SonicMastr <sonicmastr@gmail.com>
 * 
 *  This file is part of Pigs In A Blanket
 * 
 *  Pigs in a Blanket is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 ****************************************************************************/

#ifndef PATCHES_H_
#define PATCHES_H_

#ifdef USE_VITASDK
#include <psp2/gxm.h>
#include <psp2/kernel/threadmgr.h>
#else
#include <kernel.h>
#include <gxm.h>
#endif

void SHA1(char *hash_out, const char *str, int len);

void glGetBooleanv_shaderCompilerPatch(unsigned int pname, unsigned char *data);
void _pglPlatformTextureUploadParams_patch(int textureUploadParams);
int eglCreateWindowSurface_resolutionPatch(int dpy, int config, int win, int *attrib_list);
void *eglGetProcAddress_functionNamePatch(const char *procname);
unsigned int eglGetConfigAttrib_intervalPatch(void *display, void *config, int attrib, int *value);
unsigned int pglDisplaySetSwapInterval_intervalPatch(void *display, int swap_interval);
int sceDisplayWaitVblankStart_intervalPatch(void);
SceGxmErrorCode sceGxmColorSurfaceInit_msaaPatch(SceGxmColorSurface *surface,
                                                    SceGxmColorFormat colorFormat,
                                                    SceGxmColorSurfaceType surfaceType,
                                                    SceGxmColorSurfaceScaleMode scaleMode,
                                                    SceGxmOutputRegisterSize outputRegisterSize,
                                                    uint32_t width,
                                                    uint32_t height,
                                                    uint32_t strideInPixels,
                                                    void *data);
SceGxmErrorCode sceGxmCreateRenderTarget_msaaPatch(const SceGxmRenderTargetParams *params, SceGxmRenderTarget **renderTarget);
SceGxmErrorCode sceGxmDepthStencilSurfaceInit_msaaPatch(SceGxmDepthStencilSurface *surface,
                                                            SceGxmDepthStencilFormat depthStencilFormat,
                                                            SceGxmDepthStencilSurfaceType surfaceType,
                                                            uint32_t strideInSamples,
                                                            void *depthData,
                                                            void *stencilData);
SceGxmErrorCode sceGxmShaderPatcherCreateFragmentProgram_msaaPatch(SceGxmShaderPatcher *shaderPatcher,
                                                                    SceGxmShaderPatcherId programId,
                                                                    SceGxmOutputRegisterFormat outputFormat,
                                                                    SceGxmMultisampleMode multisampleMode,
                                                                    const SceGxmBlendInfo *blendInfo,
                                                                    const SceGxmProgram *vertexProgram,
                                                                    SceGxmFragmentProgram **fragmentProgram);
void glClear_loadPatch(unsigned int mask);
SceGxmErrorCode sceGxmBeginScene_loadPatch(SceGxmContext *context, unsigned int flags, const SceGxmRenderTarget *renderTarget, const SceGxmValidRegion *validRegion, SceGxmSyncObject *vertexSyncObject, SceGxmSyncObject *fragmentSyncObject, const SceGxmColorSurface *colorSurface, const SceGxmDepthStencilSurface *depthStencil);

int pglPlatformCriticalSectionEnter_patch(SceUID mutexId, SceInt32 lockCount, SceUInt32 *pTimeout);
int pglPlatformCriticalSectionLeave_patch(SceUID mutexId, SceInt32 unlockCount);
int pglPlatformCriticalSectionCreate_patch(const char *pName, SceUInt32 attr, SceInt32 initCount, const SceKernelMutexOptParam *pOptParam);
int pglPlatformCriticalSectionDestroy_patch(SceUID mutexId);
#endif /* PATCHES_H_ */
