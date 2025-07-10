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

#ifndef PLATFORM_CFG_H_
#define PLATFORM_CFG_H_

/**
 * @brief Manager resource enums.
 */
enum rde_manager_id
{
    RDE_MANAGER_SMC = 0,

    // Number of possible managers on the platform.
    RDE_MANAGER_N,
};
_Static_assert(
    CONFIG_SMC_RDE_MANAGER_COUNT == RDE_MANAGER_N,
    "Allocated manager count not equal to the defined manager count");

/**
 * @brief SoftwareInventory resource enums.
 */
enum rde_software_inventory_id
{
    RDE_SOFTWARE_INVENTORY_SMC = 0,

    // Number of possible software inventories on the platform.
    RDE_SOFTWARE_INVENTORY_N,
};
_Static_assert(
    CONFIG_SMC_RDE_SOFTWARE_INVENTORY_COUNT == RDE_SOFTWARE_INVENTORY_N,
    "Allocated software inventories count not equal to the defined software inventories count");

enum rde_chassis_id
{
    RDE_CHASSIS_TRAY = 0,
    RDE_CHASSIS_SATA_0,
    RDE_CHASSIS_SATA_1,

    // Number of possible chassis for the platform.
    RDE_CHASSIS_N,
};
_Static_assert(
    CONFIG_SMC_RDE_CHASSIS_COUNT == RDE_CHASSIS_N,
    "Allocated chassis count not equal to the defined chassis count");

/**
 * @brief Sensor enums.
 *
 * Only the sensors supported by redfish should be added to this list. Adding a
 * member to this list will result in creating storage for the redfish
 * representation of the senor.
 */
enum smc_sensor_id
{
    SMC_SENSOR_VOLTAGE = 0,
    SMC_SENSOR_CURRENT,
    SMC_SENSOR_TEMP,
    SMC_SENSOR_POW,
    SMC_SENSOR_TACH_FAN,
    SMC_SENSOR_DUTY_FAN,
    SMC_SENSOR_HDD0_TEMP,
    SMC_SENSOR_HDD1_TEMP,

    // Number of possible sensors on the platform.
    SMC_SENSOR_N,
};
_Static_assert(CONFIG_SMC_SENSOR_N == SMC_SENSOR_N,
               "Allocated sensor count not equal to the defined sensors count");

/**
 * @brief Thermal and fan control loops
 *
 * Only the controls supported by redfish should be added to this list. Adding a
 * member to this list will result in creating storage for the redfish
 * representation of the control schema.
 *
 * All the closed loop thermal control loop enums MUST start from 0 and MUST be
 * continously defined before adding other control resource IDs here.
 *
 * CHANGING the order here will AFFECT GSMC_CLOSED_LOOP_PID_CNT
 */
enum smc_pid_control_id
{
    SMC_PID_CONTROL_VR,
    SMC_PID_CONTROL_HDD,
    SMC_PID_CONTROL_FAN,

    // Number of PID control loops on the platform.
    SMC_PID_CONTROL_N,
};
_Static_assert(CONFIG_SMC_RDE_TEMP_PID_CONTROL_COUNT == SMC_PID_CONTROL_N,
               "Allocated temp PID count not equal to the defined PID count");

/**
 * @brief Number of closed loop PIDs used for thermal control.
 *
 * This is based on the enums listed in smc_pid_control_id which is 0 index
 * based.
 */
#define SMC_CLOSED_LOOP_PID_CNT (SMC_PID_CONTROL_HDD + 1)

enum smc_drive_id
{
    SMC_DRIVE_ID_0 = 0,
    SMC_DRIVE_ID_1,

    SMC_DRIVE_N
};
_Static_assert(CONFIG_SMC_RDE_DRIVE_COUNT == SMC_DRIVE_N,
               "Allocated drive count not equal to the defined drive count");

/**
 * @brief Storage subsystem index
 */
#define RDE_STORAGE_SUBSYSTEM0 0

/**
 * @brief Storage controller index
 */
#define RDE_STORAGE_CONTROLLER0 0

#endif /* PLATFORM_CFG_H_ */
