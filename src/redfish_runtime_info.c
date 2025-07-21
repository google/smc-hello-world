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

#include "platform.h"
#include "platform_cfg.h"

#include <logging/log.h>
#include <smc/fan_sensor.h>
#include <smc/flash_request_encoder.h>
#include <smc/fru.h>
#include <smc/gpio_pulse_monitor.h>
#include <smc/mctp_uart.h>
#include <smc/peripheral_stats.h>
#include <smc/pid.h>
#include <smc/rde/runtime_info.h>
#include <smc/sensor.h>
#include <smc/system.h>
#include <smc/utils.h>
#include <smc/wdt.h>
#include <stdio.h>

LOG_MODULE_REGISTER(redfish_runtime_info, LOG_LEVEL_WRN);

int redfish_get_chassis_runtime_info(const struct redfish_chassis* chassis,
                                     uint8_t operation_index,
                                     struct RedfishPropertyParent* oem_root,
                                     struct redfish_chassis_runtime_info* info)
{
    // No OEM properties are used for now.
    ARG_UNUSED(oem_root);
    ARG_UNUSED(operation_index);

    IS_PARAM_NULL(chassis, "chassis cannot be NULL");
    IS_PARAM_NULL(info, "info cannot be NULL");

    if (!chassis->initialized)
    {
        LOG_ERR("Chassis should be initialized before getting info");
        return -1;
    }

    const char* serial = "ABCD1234";
    smc_copy_str(info->serial_number, REDFISH_CHASSIS_SERIAL_NUM_LEN, serial);

    const char* part_number = "1234FFAABB";
    smc_copy_str(info->part_number, REDFISH_CHASSIS_PART_NUMBER_LEN, part_number);

    const char* name = "SomeChassis";
    smc_copy_str(info->name, REDFISH_CHASSIS_NAME_LEN, name);

    return 0;
}

int redfish_get_drive_runtime_info(
    uint16_t hdd_index, uint8_t operation_index,
    struct RedfishPropertyParent* oem_root,
    struct redfish_drive_runtime_info* runtime_info)
{
    // No OEM properties are used for now.
    ARG_UNUSED(oem_root);
    ARG_UNUSED(operation_index);
    ARG_UNUSED(hdd_index);

    IS_PARAM_NULL(oem_root, "oem_root NULL in drive_runtime_info");
    IS_PARAM_NULL(runtime_info, "runtime_info NULL in drive_runtime_info");

    const char* serial = "EFGH1234";
    smc_copy_str(runtime_info->serial_number, REDFISH_DRIVE_SERIAL_NUM_LEN,
                serial);

    const char* part_number = "1234FFAABB";
    smc_copy_str(runtime_info->part_number, REDFISH_DRIVE_PART_NUMBER_LEN,
                part_number);

    const char* model = "SomeHDDModel";
    smc_copy_str(runtime_info->model, REDFISH_DRIVE_MODEL_LEN, model);

    return 0;
}

int redfish_get_control_runtime_info(uint16_t pid_control_id,
                                     struct redfish_control_runtime_info* info)
{
    IS_PARAM_NULL(info, "info NULL in control_runtime_info");

    info->mode = REDFISH_CONTROL_CONTROL_MODE_AUTOMATIC;

    int mode_ret = thermalControlConfig();
    if ((mode_ret == KTHERMAL_CONTROLS_NOT_CONFIGURED) ||
        (mode_ret == KTHERMAL_CONTROLS_MANUAL))
    {
        info->mode = REDFISH_CONTROL_CONTROL_MODE_MANUAL;
    }

    switch (pid_control_id)
    {
        case SMC_PID_CONTROL_FAN:
        {
            float fan0;
            RETURN_IF_IERROR(
                get_sensor_calibrated_reading(SMC_SENSOR_DUTY_FAN, &fan0));
            info->setpoint = fan0;
            break;
        }
        case SMC_PID_CONTROL_VR:
        case SMC_PID_CONTROL_HDD:
        {
            float value;
            info->setpoint = REDFISH_DOUBLE_INVALID_READING;
            info->p_coeff = REDFISH_DOUBLE_INVALID_READING;
            info->i_coeff = REDFISH_DOUBLE_INVALID_READING;
            info->d_coeff = REDFISH_DOUBLE_INVALID_READING;

            if (getPIDparam(pid_control_id, kPID_setpoint, &value) == 0)
            {
                info->setpoint = value;
            }

            if (getPIDparam(pid_control_id, kPID_kP, &value) == 0)
            {
                info->p_coeff = value;
            }

            if (getPIDparam(pid_control_id, kPID_kI, &value) == 0)
            {
                info->i_coeff = value;
            }

            if (getPIDparam(pid_control_id, kPID_kD, &value) == 0)
            {
                info->d_coeff = value;
            }
            break;
        }
        default:
            LOG_ERR("Invalid PID control id: %d", pid_control_id);
            return -1;
    }
    return 0;
}

int redfish_set_control_runtime_cfg(
    uint16_t pid_control_id, struct redfish_control_decoder_params* params)
{
    ARG_UNUSED(pid_control_id);
    ARG_UNUSED(params);
    return 0;
}

int redfish_get_storage_controller_runtime_info(
    const struct redfish_storage_controller* controller,
    uint8_t operation_index, struct RedfishPropertyParent* oem_root,
    struct redfish_storage_controller_runtime_info* info)
{
    ARG_UNUSED(controller);
    ARG_UNUSED(operation_index);
    ARG_UNUSED(oem_root);

    IS_PARAM_NULL(info, "info NULL in storage_controller_runtime_info");

    info->state = REDFISH_STATUS_STATE_ENABLED;

    const char* version = "1.2.4.0";
    smc_copy_str(info->firmware_version, REDFISH_FIRMWARE_VERS_BUFFER_SIZE, version);
    return 0;
}

int redfish_get_i2c_diagnostic_data(uint16_t manager_id, uint8_t i2c_id,
                                    struct redfish_i2c_bus_data* i2c_data)
{
    ARG_UNUSED(manager_id);
    IS_PARAM_NULL(i2c_data, "NULL i2c_data in get i2c diagnostics");

    i2c_data->total_transactions = 1000;
    i2c_data->bus_errors = 1;
    return 0;
}

int redfish_get_manager_diagnostic_runtime_info(
    uint8_t operation_index, struct RedfishPropertyParent* oem_root,
    struct redfish_manager_diagnostic_runtime_info* info)
{
    ARG_UNUSED(operation_index);
    ARG_UNUSED(oem_root);

    IS_PARAM_NULL(info, "info is  NULL in manager_runtime_info");
    info->reboot_count = 11;
    info->crash_count = 2;

    return 0;
}

int redfish_get_sensor_reading(uint16_t sensor_id, float* val)
{
    return get_sensor_calibrated_reading(sensor_id, val);
}

int redfish_set_sensor_reading(uint16_t sensor_id, float val)
{
    return set_write_allowed_sensor_reading(sensor_id, val);
}

int redfish_set_drive_power(uint16_t hdd_index, bool power)
{
    return platform_set_hdd_power_state(hdd_index, power);
}
