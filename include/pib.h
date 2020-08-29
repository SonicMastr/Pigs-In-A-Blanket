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

#include <psp2common/types.h>

/**
 * @brief Initialization options for PIB. If shaccCgEnabled is not specified,
 *  PIB will start without Runtime Shader Compiler support. noStdLib should only be used if
 *  you do not use newlib and opt for SceLibc.
 * 
 */
typedef struct _PibOptions {
    SceBool shaccCgEnabled;
    SceBool noStdLib;
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
int pibInit(PibOptions *pibOptions);

/**
 * @brief Terminates and unloads Piglet and optionally SceShaccCg if specified by pibInit
 * 
 * @return int 
 * 
 */
int pibTerm(void);

#endif