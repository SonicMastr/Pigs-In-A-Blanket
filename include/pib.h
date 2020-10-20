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

#ifndef PIB_H_
#define PIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <psp2common/types.h>

/**
 * @brief Enums for eglCreateWindowSurface's EGLNativeWindowType parameter
 * 
 */
typedef enum vitaEGLNativeWindowType {
    VITA_INVALID_WINDOW = 0,
    VITA_WINDOW_960X544,
    VITA_WINDOW_720X408,
    VITA_WINDOW_640X368,
    VITA_WINDOW_480X272,
    VITA_WINDOW_1280X720,
    VITA_WINDOW_1920X1080
} VitaEGLNativeWindowType;

/**
 * @brief Initialization options for PIB.
 * 
 */
typedef enum _PibOptions {
    PIB_NONE = 0, // Defaults
    PIB_SHACCCG = 1, // Enable Runtime CG Shader Compiler
    PIB_NOSTDLIB = 2 // Enable support for -nostdlib usage
} PibOptions;

/**
 * @brief Initializes Piglet and optionally SceShaccCg, the Native Runtime Shader Compiler. 
 *  Searches the "ur0:data/external" directory
 * 
 * @param[in] pibOptions 
 *  Intialization options for PIB
 * 
 * @return 0 on success, -1 if libshacccg.suprx is not present (ShaccCgEnabled option only), -2 if libScePiglet.suprx is not present,
 *  -3 if libc could not load (noStdLib option only), -4 if libfios2 could not load (noStdLib option only)
 * 
 */
int pibInit(PibOptions pibOptions);

/**
 * @brief Terminates and unloads Piglet and optionally SceShaccCg if specified by pibInit
 * 
 * @return int 
 * 
 */
int pibTerm(void);

#ifdef __cplusplus
}
#endif

#endif /* PIB_H_ */