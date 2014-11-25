/*
*** Copyright Notice ***
Water Heater monitoring system, Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved.

If you have questions about your rights to use or distribute this software, please contact Berkeley Lab's Technology Transfer Department at  TTD@lbl.gov.

NOTICE.  This software is owned by the U.S. Department of Energy.  As such, the U.S. Government has been granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, and perform publicly and display publicly.  Beginning five (5) years after the date permission to assert copyright is obtained from the U.S. Department of Energy, and subject to any subsequent five (5) year renewals, the U.S. Government is granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.
****************************


*** License Agreement ***
"Water Heater monitoring system, Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved."

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

(1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

(2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

(3) Neither the name of the University of California, Lawrence Berkeley National Laboratory, U.S. Dept. of Energy nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

You are under no obligation whatsoever to provide any bug fixes, patches, or upgrades to the features, functionality or performance of the source code ("Enhancements") to anyone; however, if you choose to make your Enhancements available either publicly, or directly to Lawrence Berkeley National Laboratory, without imposing a separate written license agreement for such Enhancements, then you hereby grant the following license: a  non-exclusive, royalty-free perpetual license to install, use, modify, prepare derivative works, incorporate into other computer software, distribute, and sublicense such enhancements or derivative works thereof, in binary and source code form.
***************************   
*/

/* 
 * File:   waterHeaterModel.h
 * Author: mcp
 *
 * Created on March 12, 2013, 1:50 AM
 */

#ifndef WATERHEATERMODEL_H
#define	WATERHEATERMODEL_H

#ifdef	__cplusplus
extern "C" {
#endif

/* Default ip parameters.  These can be overridden on the commend line. */
#define DEFAULT_CLIENT_IP "127.0.0.1"
#define DEFAULT_PORT 502
    
#define PHYSICAL_MODEL_DELAY_INTERVAL_MSEC 5000
 
/* address and length of input bits */
#define HEATER_COIL_ENERGIZED 0
#define NUM_INPUT_BIT_REG 1
    
/* address and length of coil bits */
#define HEATER_COIL_ENABLE 0
#define NUM_BIT_REG 1
    
/* address and length of value registers */
#define HEATER_WATER_TEMP_REG 0
#define NUM_INPUT_VALUE_REG 1
    
/* address and length of holding value registers */
#define HEATER_WATER_TARGET_TEMP_REG 0
#define NUM_VALUE_REG 1
    
#define AMBIENT_TEMP_DEG_F 65.0
#define HEATER_TEMP_DEG_F 100.0
#define HEATER_ON_STATE false
#define HEATER_TARGET_TEMP_DEG_F 101.5
#define HEATER_CONTROL_HYSTERESIS_DEG_F .5
#define COOLING_CONST -0.00351
#define HEATING_CONST 0.2
#define HEATER_KW 1.5
#define TIME_SCALING_FACTOR 100.
    


#ifdef	__cplusplus
}
#endif

#endif	/* WATERHEATERMODEL_H */

