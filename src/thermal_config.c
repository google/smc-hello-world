/*
 * Copyright 2025 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "platform_cfg.h"

#include <init.h>
#include <logging/log.h>
#include <shell/shell.h>
#include <smc/fan_sensor.h>
#include <smc/pid.h>
#include <smc/pid_sensor.h>
#include <smc/pid_strings.h>
#include <smc/thermal_error.h>
#include <stdio.h>
#include <stdlib.h>

extern double atof(const char* string);

// default settings
const int kPid_Def_Enable = 1;

#define FAN_OUTPUT_MIN 20.0
#define FAN_OUTPUT_MAX 90.0

const float kOutput_Min = FAN_OUTPUT_MIN;            // Min PID loop
const float kOutput_Min_Post = (kOutput_Min - 0.01); // Min post proc

const float kOutput_Max = FAN_OUTPUT_MAX; // Max PID loop

const float kOutput_Start = 60.0; // pwm during start phase

const float kIntegral_Min = kOutput_Min;
const float kIntegral_Max = kOutput_Max;

// 1 minute start phase
const int kStartPhaseInSec = (1 * 60);

LOG_MODULE_REGISTER(smc_thermal_config);

static void setOutputTable(uint32_t ctx, float value);
static void smcPostProc(void);
static float getHddAvgTemp(uint32_t);

//===========================================

static float outputTable[SMC_CLOSED_LOOP_PID_CNT]; // pid output storage (one
                                                   // per pid loop)

//---------------
// SMC thermal control descriptors
//

#define getter(procedure, context)                                             \
    {                                                                          \
        .hdlr.get = procedure,                                                 \
        .ctx = context,                                                        \
    }

#define setter(procedure, context)                                             \
    {                                                                          \
        .hdlr.set = procedure,                                                 \
        .ctx = context,                                                        \
    }

pid_desc_t smcPidDesc[] = {
    [SMC_PID_CONTROL_VR] =
        {
            .namePtr = "VRs",
            .localSetpoint = 66.0,
            .setpt = getter(localSetptHdlr, SMC_PID_CONTROL_VR),
            .input = getter(readSensor, SMC_SENSOR_TEMP),
            .output = setter(setOutputTable, SMC_PID_CONTROL_VR),

            .info =
                {
                    .enabled = kPid_Def_Enable,
                    .ts = 1,
                    .kP = -0.1,
                    .kI = -0.05,
                    .kD = 0.0,
                    .i_lim.max = kIntegral_Max,
                    .i_lim.min = kIntegral_Min,
                    .out_lim.max = kOutput_Max,
                    .out_lim.min = kOutput_Min,
                    .slew_pos = 0.0,
                    .slew_neg = 0.0,
                    .deadband = 0.0,
                },
        },

    [SMC_PID_CONTROL_HDD] =
        {
            .namePtr = "HDD",
            .localSetpoint = 48.0,
            .setpt = getter(localSetptHdlr, SMC_PID_CONTROL_HDD),
            .input = getter(getHddAvgTemp, 0),
            .output = setter(setOutputTable, SMC_PID_CONTROL_HDD),
            .info =
                {
                    .enabled = kPid_Def_Enable,
                    .ts = 60,
                    .kP = -4.0,
                    .kI = -0.01,
                    .kD = 0.0,
                    .i_lim.max = kIntegral_Max,
                    .i_lim.min = kIntegral_Min,
                    .out_lim.max = kOutput_Max,
                    .out_lim.min = kOutput_Min,
                    .slew_pos = 0.0,
                    .slew_neg = 0.0,
                    .deadband = 0.0,
                },
        },

    // Terminator
    {
        .info.ts = 0,
    },
};

//-----------------------
// object export
const thermal_ctl_t smc_thermal_ctl = {
    .descPtr = &smcPidDesc[0],
    .postProc = smcPostProc,
};

//------------------------
// getHddMaxTemp - get the average HDD temperature
static float getHddAvgTemp(uint32_t)
{
    float hdd0 = readSensor(SMC_SENSOR_HDD0_TEMP);
    float hdd1 = readSensor(SMC_SENSOR_HDD1_TEMP);

    return (hdd0 + hdd1) / 2.0;
}

//------------------------
// pidHdl - Get max value from PIDs
static void pidHdl(float* maxPtr)
{
    *maxPtr = 0;
    // find max value of requests
    for (int i = 0; i < SMC_CLOSED_LOOP_PID_CNT; i++)
    {
        if (smcPidDesc[i].info.enabled)
        {
            if (*maxPtr < outputTable[i])
                *maxPtr = outputTable[i];
        }
    }
}

//------------------------
// smcPostProc - Post thermal control calcuation callback (once a second)
//   - This routine is called after the PID FSM completes process of the
//     thermal descriptor.
//   - Any post process is done here.
//     - Implement start phase
//     - Get PID/bmc max request
//     - Update FAN pwm sensors for FAN driver
//
static void smcPostProc(void)
{
    float max = kOutput_Min_Post;
    static int start_phase_timer_sec = kStartPhaseInSec;

    // handle start phase
    if (start_phase_timer_sec > 0)
        start_phase_timer_sec -= 1;
    if (start_phase_timer_sec > 0)
    {
        writeSensor(SMC_SENSOR_DUTY_FAN, kOutput_Start);
    }
    else
    {
        pidHdl(&max);
        writeSensor(SMC_SENSOR_DUTY_FAN, max);
    }

    // drive fans to lastest settings. But no fans are connected or configured.
    // int ret = fan_set_duty_by_id(/*fan_index=*/0, max);
    // if (ret != 0)
    //         printk("  Set Fan PWM%d:%d %f\n", fan_index, ret, percent);
}

//------------------------
// setOutputTable -
//   input:
//     index: outputTable index
//     value: value to store in outputTable
//
static void setOutputTable(uint32_t index, float value)
{
    outputTable[index] = value;
}

//-------------------------
int install_smc_thermal_ctl()
{
    pidControlInit(&smc_thermal_ctl);
    return 0;
}
