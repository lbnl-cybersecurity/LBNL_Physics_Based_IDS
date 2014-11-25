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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <modbus/modbus.h>
#include "waterHeaterModel.h"
#include "modbusRegisters.h"

/* 
 * This code models the behavior of a simple hot water
 * heater.  The water is heated by a simple heating element
 * that is controlled by a simple control element with a
 * programmable amount of hysteresis (i.e. over/under shoot).
 * Heat is lost through simple conduction, through an 
 * insulated wall, to the ambient environment.  Mathematical 
 * model is based on Newtonial exponential cooling model.  
 * Constants have been computed externally - details provied
 * as needed.
 * 
 * This model executes as a thread that is periodically inovoked
 * to compute, via heat loss and gain, the current temperature of the
 * contained water.  Calculations are based on elapsed seconds and
 * time can be accelerated by scaling the number of seconds calculated
 * by the model as opposed to the actual thread sleep interval.  Note
 * the use of mutex to protect reads and writes to the modbus register
 * structure.  This mutex is shared with the main thread that is 
 * responsible for communications processing.
 * 
 *
 * The following registers are implemented:
 * ---------------------------------------------
 * coil #0 is used to tnable heater operation
 * input bit #0 reflects the current powered state of the heater
 * input register #0 is the current water temperature in deg. F x 10
 * holding register #0 is current target temperature in deg. F x 10
 * 
 * The heater model is enabled and set to an initial target temperature
 * within the main routine at startup.
 */


extern pthread_mutex_t modbusRegisterAccessLock;

void * startPhysicalModel(void * data) {
    
    int sts;
    struct timespec tm;
    modbus_mapping_t *mb_mapping = (modbus_mapping_t *)data;
    
    float tAmbient = AMBIENT_TEMP_DEG_F;
    float tHeater = HEATER_TEMP_DEG_F;
    bool heaterEnabled = false;
    bool heaterOn = HEATER_ON_STATE;
    float tHeaterTarget = HEATER_TARGET_TEMP_DEG_F;
    float tContHysteresis = HEATER_CONTROL_HYSTERESIS_DEG_F;
    float modelDelayTime_hrs = (float)(PHYSICAL_MODEL_DELAY_INTERVAL_MSEC)/
        (1000. * 60. * 60.);
    float tIncrease = 0.0;
    
    heaterEnabled = false;
    heaterOn = false;
    char * heaterEnableState;
    char * heaterElementState;
    char * displayFlag;
    bool displayCnt = true;
    
    if(data == NULL) {
        perror("Illegal modbus mapping struct.");
                exit(1);
    }
    
    while (1) {
        /* get current EPOCH time */
        //clock_gettime(CLOCK_REALTIME, &tm);
        
        usleep(PHYSICAL_MODEL_DELAY_INTERVAL_MSEC * 1000); 

        /* get any modbus registers that may have changed */
        pthread_mutex_lock(&modbusRegisterAccessLock);
        heaterEnabled = getRegisterBit(mb_mapping, HEATER_COIL_ENABLE);
        /* turn off heater if we are disabled */
        if(!heaterEnabled) {
            heaterOn = false;
        }
        tHeaterTarget = (float)(getRegisterValue(mb_mapping, 
                HEATER_WATER_TARGET_TEMP_REG)) / 10.0;
        pthread_mutex_unlock(&modbusRegisterAccessLock);
        
        /* update the model state and, if necessary, update
         modbus registers */
        tIncrease = 0.0;
        if(heaterOn) {
            /* calculate temp increase if heater is on */
            tIncrease = (HEATER_KW * HEATING_CONST) * modelDelayTime_hrs 
                * TIME_SCALING_FACTOR;
        }
        float tCoolDown = tAmbient +            
            (tHeater - tAmbient) * exp(COOLING_CONST * modelDelayTime_hrs
            * TIME_SCALING_FACTOR);
        /* water temp is sum of + and - temperature deltas*/
        tHeater = tCoolDown + tIncrease;
        
        /* update the controller state */
        if(heaterEnabled) {
            if(heaterOn) {
                if(tHeater > (tHeaterTarget + tContHysteresis)) {
                    heaterOn = false;
                }
            }
            else {
                if(tHeater < (tHeaterTarget - tContHysteresis)) {
                    heaterOn = true;
                }
            }
        }
        
        /* lock context and transfer values from the model.  Then unlock.*/
        pthread_mutex_lock(&modbusRegisterAccessLock);
        setInputRegisterValue(mb_mapping, HEATER_WATER_TEMP_REG,
            (short)(tHeater * 10.0));
        setInputRegisterBit(mb_mapping, HEATER_COIL_ENERGIZED, heaterOn);
        pthread_mutex_unlock(&modbusRegisterAccessLock);
        
        if(heaterEnabled) {
            heaterEnableState = "Heater Enabled, ";
        }
        else {
            heaterEnableState = "Heater Disabled, ";
        }
        
        if(heaterOn) {
            heaterElementState = "Heater Element On, ";
        }
        else {
            heaterElementState = "Heater Element Off, ";
        }
        
        if (displayCnt) {
            displayCnt = false;
            displayFlag = "+";
        }
        else {
            displayCnt = true;
            displayFlag = "-";
        }
        
        printf("Model Status(%s): %s%sSet Point: %5.2f, Temp: %5.2f\n",
                displayFlag,
                heaterEnableState, heaterElementState, 
                tHeaterTarget, tHeater);
    }
}
