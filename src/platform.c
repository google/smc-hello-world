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
#include "rde_resources.h"

#include <smc/adc_sensor.h>
#include <smc/fan_sensor.h>

extern int install_smc_thermal_ctl();

static bool hdd_power_state[SMC_DRIVE_N];

/**
 * @brief Initialize platform specific functionality. This overrides the
 * function in smc-common/src/main
 */
int platform_init()
{
    // Inittialize the HDD dummy power states to ON.
    for (int i = 0; i < SMC_DRIVE_N; ++i)
    {
        hdd_power_state[i] = true;
    }

    return 0;
}

/**
 * @brief Initialize the redfish resources. This overrides the function in
 * smc-common/src/main
 */
int platform_rde_server_init(struct redfish_server* server)
{
    return rde_server_init(server);
}

/**
 * @brief Initializing an ADC sensor
 */
static struct adc_sensor_ctx adc_sensor_list[] = {
    {
        .dev_label = "ADC0",
        .id = SMC_SENSOR_VOLTAGE,
        .name = "sen_voltage",
        .channel_num = 0,
        .max = 10.0,
        .min = 0.0,
        .gain = 6.6,
        .offset = 0.0,
        .poll_rate_ms = 1000,
        .acq_time_us = ADC_ACQ_TIME_DEFAULT,
        .oversampling = 0,
        .unit = volt,
    },
};
static int smc_sensors_init_adc(const struct device* dev)
{
    ARG_UNUSED(dev);
    return adc_sensor_monitor(adc_sensor_list, ARRAY_SIZE(adc_sensor_list));
}
SYS_INIT(smc_sensors_init_adc, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

/**
 * @brief Initializing a fan
 *
 * If a tach sensor is available it can be added here as well. Also it should be
 * initialized in the board file.
 */
static struct smc_fan_sensor_ctx fan_sensor_list[] = {
    {
        .tach =
            {
                .dev_label = "",
            },
        .duty =
            {
                .dev_label = "PWM",
                .id = SMC_SENSOR_DUTY_FAN,
                .name = "fan_duty",
                .channel_num = SENSOR_CHAN_DUTY_CYCLE,
                .max = MAX_FAN_DUTY,
                .min = MIN_FAN_DUTY,
                .default_duty = 65,
                .poll_rate_ms = 0,
                .unit = percent,
                .pwm_index = 0,
            },
    },
};
static int smc_init_fan(const struct device* dev)
{
    ARG_UNUSED(dev);
    return fan_sensor_init(fan_sensor_list, ARRAY_SIZE(fan_sensor_list));
}
SYS_INIT(smc_init_fan, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

/**
 * @brief Initialize several dummy sensors
 */
static int smc_init_dummy_sensors(const struct device* dev)
{
    ARG_UNUSED(dev);

    sensor_register_by_id(SMC_SENSOR_CURRENT, /*device=*/NULL, "current_sensor",
                          /*max=*/10, /*min=*/0,
                          /*poll_rate_ms=*/1000,
                          /*write_protect=*/true, amp,
                          /*gain=*/1, /*offset=*/0);
    set_sensor_reading_float(SMC_SENSOR_CURRENT, 2.55);

    sensor_register_by_id(SMC_SENSOR_TEMP, /*device=*/NULL, "tray_temp",
                          /*max=*/80, /*min=*/5,
                          /*poll_rate_ms=*/1000,
                          /*write_protect=*/true, CELSIUS,
                          /*gain=*/1, /*offset=*/0);
    set_sensor_reading_float(SMC_SENSOR_TEMP, 33.0);

    sensor_register_by_id(SMC_SENSOR_POW, /*device=*/NULL, "tray_power",
                          /*max=*/500, /*min=*/0,
                          /*poll_rate_ms=*/1000,
                          /*write_protect=*/true, watt,
                          /*gain=*/1, /*offset=*/0);
    set_sensor_reading_float(SMC_SENSOR_POW, 250.0);

    sensor_register_by_id(SMC_SENSOR_TACH_FAN, /*device=*/NULL, "fan_tach",
                          /*max=*/12000, /*min=*/100,
                          /*poll_rate_ms=*/1000,
                          /*write_protect=*/true, rpm,
                          /*gain=*/1, /*offset=*/0);
    set_sensor_reading_float(SMC_SENSOR_TACH_FAN, 8000);

    sensor_register_by_id(SMC_SENSOR_HDD0_TEMP, /*device=*/NULL, "hdd0_temp",
                          /*max=*/70, /*min=*/10,
                          /*poll_rate_ms=*/1000,
                          /*write_protect=*/true, CELSIUS,
                          /*gain=*/1, /*offset=*/0);
    set_sensor_reading_float(SMC_SENSOR_HDD0_TEMP, 39.0);

    sensor_register_by_id(SMC_SENSOR_HDD1_TEMP, /*device=*/NULL, "hdd1_temp",
                          /*max=*/70, /*min=*/10,
                          /*poll_rate_ms=*/1000,
                          /*write_protect=*/true, CELSIUS,
                          /*gain=*/1, /*offset=*/0);
    set_sensor_reading_float(SMC_SENSOR_HDD1_TEMP, 40.0);

    return 0;
}
SYS_INIT(smc_init_dummy_sensors, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

/**
 * @brief Initialize PID control
 */
static int smc_sensors_init_thermal(const struct device* dev)
{
    ARG_UNUSED(dev);
    return install_smc_thermal_ctl();
}
SYS_INIT(smc_sensors_init_thermal, APPLICATION,
         CONFIG_APPLICATION_INIT_PRIORITY);

int platform_set_hdd_power_state(uint16_t hdd_index, bool power)
{
    if (hdd_index >= SMC_DRIVE_N)
    {
        return -1;
    }
    hdd_power_state[hdd_index] = power;
    return 0;
}
