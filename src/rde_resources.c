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

#include "rde_resources.h"

#include "platform_cfg.h"

#include <logging/log.h>

LOG_MODULE_REGISTER(rde_init, LOG_LEVEL_ERR);

static const char* const smc_manager_odata_id = "/redfish/v1/Managers/smc";

static struct redfish_manager manager_params[] = {
    {
        .manager_index = RDE_MANAGER_SMC,
        .manager_type = REDFISH_MANAGER_TYPE_BMC,
        .odata_id = smc_manager_odata_id,
        .id = "smc",
        .name = "smc",
        .model = "smc",
        .manager_diagnostic_data =
            "/redfish/v1/Managers/smc/ManagerDiagnosticData",
        .location =
            {
                .location_type = REDFISH_LOCATION_TYPE_EMBEDDED,
                .service_label = NULL,
            },
    },
};

static struct redfish_update_service update_service_params = {
    .odata_id = "/redfish/v1/UpdateService",
    .firmware_inventory =
        {
            .odata_id = "/redfish/v1/UpdateService/FirmwareInventory",
        },
    .id = "UpdateService",
    .max_image_size_byte = 0,
    .name = "Update Service",
    .service_enabled = true,
    .initialized = true,
};

static struct redfish_software_inventory software_inventory_params[] = {
    {
        .software_inventory_index = RDE_SOFTWARE_INVENTORY_SMC,
        .odata_id = "/redfish/v1/UpdateService/FirmwareInventory/smc",
        .description = "SMC firmware",
        .id = "smc",
        .name = "Software Inventory",
        .related_items = {smc_manager_odata_id},
        .updateable = false,
    },
};

static struct redfish_chassis hdd_chassis_params[] = {
    {
        .chassis_id = RDE_CHASSIS_SATA_0,
        .odata_id = "/redfish/v1/Chassis/SATA_0",
        .chassis_type = REDFISH_CHASSIS_TYPE_STORAGEENCLOSURE,
        .id = "SATA_0",
        .sensors_collection = "/redfish/v1/Chassis/SATA_0/Sensors",
        .drives = {"/redfish/v1/Chassis/SATA_0/Drives/SATA_0"},
        .hdd_index = SMC_DRIVE_ID_0,
        .drives_collection = "/redfish/v1/Chassis/SATA_0/Drives",
        .contained_by_chassis_id = RDE_CHASSIS_TRAY,
        .location =
            {
                .location_type = REDFISH_LOCATION_TYPE_SLOT,
                .service_label = "sas@0",
            },
    },
    {
        .chassis_id = RDE_CHASSIS_SATA_1,
        .odata_id = "/redfish/v1/Chassis/SATA_1",
        .chassis_type = REDFISH_CHASSIS_TYPE_STORAGEENCLOSURE,
        .id = "SATA_1",
        .sensors_collection = "/redfish/v1/Chassis/SATA_1/Sensors",
        .drives = {"/redfish/v1/Chassis/SATA_1/Drives/SATA_1"},
        .hdd_index = SMC_DRIVE_ID_1,
        .drives_collection = "/redfish/v1/Chassis/SATA_1/Drives",
        .contained_by_chassis_id = RDE_CHASSIS_TRAY,
        .location =
            {
                .location_type = REDFISH_LOCATION_TYPE_SLOT,
                .service_label = "sas@1",
            },
    },
};

static struct redfish_sensor tray_sensor_params[] = {
    {
        .chassis_id = RDE_CHASSIS_TRAY,
        .sensor_id = SMC_SENSOR_VOLTAGE,
        .odata_id = "/redfish/v1/Chassis/Tray/Sensors/Sen_voltage",
        .id = "sen_voltage",
        .name = "sen_voltage",
        .reading_type = REDFISH_SENSOR_READING_TYPE_VOLTAGE,
        .related_item_odata_id = smc_manager_odata_id,
    },
    {
        .chassis_id = RDE_CHASSIS_TRAY,
        .sensor_id = SMC_SENSOR_CURRENT,
        .odata_id = "/redfish/v1/Chassis/Tray/Sensors/Sen_current",
        .id = "sen_current",
        .name = "sen_current",
        .reading_type = REDFISH_SENSOR_READING_TYPE_CURRENT,
        .related_item_odata_id = smc_manager_odata_id,
    },
    {
        .chassis_id = RDE_CHASSIS_TRAY,
        .sensor_id = SMC_SENSOR_TEMP,
        .odata_id = "/redfish/v1/Chassis/Tray/Sensors/Sen_temperature",
        .id = "sen_temperature",
        .name = "sen_temperature",
        .reading_type = REDFISH_SENSOR_READING_TYPE_TEMPERATURE,
        .related_item_odata_id = smc_manager_odata_id,
    },
    {
        .chassis_id = RDE_CHASSIS_TRAY,
        .sensor_id = SMC_SENSOR_POW,
        .odata_id = "/redfish/v1/Chassis/Tray/Sensors/Sen_power",
        .id = "sen_power",
        .name = "sen_power",
        .reading_type = REDFISH_SENSOR_READING_TYPE_POWER,
        .related_item_odata_id = smc_manager_odata_id,
    },
    {
        .chassis_id = RDE_CHASSIS_TRAY,
        .sensor_id = SMC_SENSOR_TACH_FAN,
        .odata_id = "/redfish/v1/Chassis/Tray/Sensors/Fan_tach",
        .id = "fan_tach",
        .name = "fan_tach",
        .reading_type = REDFISH_SENSOR_READING_TYPE_ROTATIONAL,
        .related_item_odata_id = smc_manager_odata_id,
    },
    {
        .chassis_id = RDE_CHASSIS_TRAY,
        .sensor_id = SMC_SENSOR_DUTY_FAN,
        .odata_id = "/redfish/v1/Chassis/Tray/Sensors/Fan_duty",
        .id = "fan_duty",
        .name = "fan_duty",
        .reading_type = REDFISH_SENSOR_READING_TYPE_PERCENT,
        .related_item_odata_id = smc_manager_odata_id,
    },
};

static struct redfish_sensor hdd_sensor_params[] = {
    {
        .chassis_id = RDE_CHASSIS_SATA_0,
        .sensor_id = SMC_SENSOR_HDD0_TEMP,
        .odata_id = "/redfish/v1/Chassis/SATA_0/Sensors/Temp",
        .id = "Temp",
        .name = "Temp",
        .reading_type = REDFISH_SENSOR_READING_TYPE_TEMPERATURE,
        .related_item_odata_id = smc_manager_odata_id,
    },
    {
        .chassis_id = RDE_CHASSIS_SATA_1,
        .sensor_id = SMC_SENSOR_HDD1_TEMP,
        .odata_id = "/redfish/v1/Chassis/SATA_1/Sensors/Temp",
        .id = "Temp",
        .name = "Temp",
        .reading_type = REDFISH_SENSOR_READING_TYPE_TEMPERATURE,
        .related_item_odata_id = smc_manager_odata_id,
    },
};

static struct redfish_control control_params[] = {
    {
        .chassis_id = RDE_CHASSIS_TRAY,
        .control_index = SMC_PID_CONTROL_VR,
        .odata_id = "/redfish/v1/Chassis/Tray/Controls/vr_pid",
        .description = "VR PID",
        .id = "vr_pid",
        .name = "vr_pid",
        .control_type = REDFISH_CONTROL_CONTROL_TYPE_TEMPERATURE,
        .setpoint_type = REDFISH_CONTROL_SETPOINT_TYPE_SINGLE,
        .implementation_type = REDFISH_CONTROL_IMPLEMENTATION_TYPE_PROGRAMMABLE,
    },
    {
        .chassis_id = RDE_CHASSIS_TRAY,
        .control_index = SMC_PID_CONTROL_HDD,
        .odata_id = "/redfish/v1/Chassis/Tray/Controls/hdd_pid",
        .description = "PID loop with HDD temp",
        .id = "hdd_pid",
        .name = "hdd_pid",
        .control_type = REDFISH_CONTROL_CONTROL_TYPE_TEMPERATURE,
        .setpoint_type = REDFISH_CONTROL_SETPOINT_TYPE_SINGLE,
        .implementation_type = REDFISH_CONTROL_IMPLEMENTATION_TYPE_PROGRAMMABLE,
    },
    {
        .chassis_id = RDE_CHASSIS_TRAY,
        .control_index = SMC_PID_CONTROL_FAN,
        .odata_id = "/redfish/v1/Chassis/Tray/Controls/fans",
        .description = "Control fans",
        .id = "fans",
        .name = "fans",
        .is_open_loop = true,
        // TODO: PWM control type is not supported by the standard yet.
        .control_type = REDFISH_CONTROL_CONTROL_TYPE_INVALID,
    },
};

static struct redfish_drive drive_params[] = {
    {
        .chassis_id = RDE_CHASSIS_SATA_0,
        .drive_id = SMC_DRIVE_ID_0,
        .storage_id = RDE_STORAGE_SUBSYSTEM0,
        .odata_id = "/redfish/v1/Chassis/SATA_0/Drives/SATA_0",
        .media_type = REDFISH_DRIVE_MEDIA_TYPE_HDD,
        .protocol = REDFISH_PROTOCOL_SATA,
        .id = "SATA_0",
        .name = "SATA_0",
        .chassis_link = "/redfish/v1/Chassis/SATA_0",
        .reset_action =
            {
                .action_info = NULL,
                .target =
                    "/redfish/v1/Chassis/SATA_0/Drives/SATA_0/Actions/Drive.Reset",
            },
    },
    {
        .chassis_id = RDE_CHASSIS_SATA_1,
        .drive_id = SMC_DRIVE_ID_1,
        .storage_id = RDE_STORAGE_SUBSYSTEM0,
        .odata_id = "/redfish/v1/Chassis/SATA_1/Drives/SATA_1",
        .media_type = REDFISH_DRIVE_MEDIA_TYPE_HDD,
        .protocol = REDFISH_PROTOCOL_SATA,
        .id = "SATA_1",
        .name = "SATA_1",
        .chassis_link = "/redfish/v1/Chassis/SATA_1",
        .reset_action =
            {
                .action_info = NULL,
                .target =
                    "/redfish/v1/Chassis/SATA_1/Drives/SATA_1/Actions/Drive.Reset",
            },
    },
};

static struct redfish_storage storage_params = {
    .storage_id = RDE_STORAGE_SUBSYSTEM0,
    .odata_id = "/redfish/v1/Storage/SATA",
    .id = "SATA",
    .name = "SATA_storage",
    .controllers = "/redfish/v1/Storage/SATA/Controllers",
};

static struct redfish_storage_controller storage_controller_params = {

    .controller_id = RDE_STORAGE_CONTROLLER0,
    .storage_id = RDE_STORAGE_SUBSYSTEM0,
    .odata_id = "/redfish/v1/Storage/SATA/Controllers/sata_controller",
    .id = "sata_controller",
    .name = "sata_controller",
    .location =
        {
            .location_type = REDFISH_LOCATION_TYPE_EMBEDDED,
            .service_label = "abcd",
        },
};

static int rde_create_tray_chassis(struct redfish_server* server)
{
    // We only have 1 tray. So its ok to declare it here as static.
    static struct redfish_chassis params = {0};
    params.chassis_id = RDE_CHASSIS_TRAY;
    params.contained_by_chassis_id = REDFISH_CHASSIS_ID_DO_NOT_EXIST;
    params.odata_id = "/redfish/v1/Chassis/Tray";
    params.chassis_type = REDFISH_CHASSIS_TYPE_RACKMOUNT;
    params.managed_by = smc_manager_odata_id;
    params.sensors_collection = "/redfish/v1/Chassis/Tray/Sensors";
    params.controls_collection = "/redfish/v1/Chassis/Tray/Controls";
    params.storage[0] = "/redfish/v1/Storage/SATA";
    params.location.location_type = REDFISH_LOCATION_TYPE_SLOT;
    params.id = "StorageTray";
    params.name = "StorageTray";
    params.model = "StorageTray_1";
    params.location.service_label = "PCIE0";

    RETURN_IF_IERROR(
        redfish_helper_register_chassis(server, &params, /*chassis_count=*/1));
    // Manager should be contained by the tray Chassis.
    RETURN_IF_IERROR(redfish_server_set_chassis_additional_contains(
        server, RDE_CHASSIS_TRAY, smc_manager_odata_id));

    return 0;
}

int rde_server_init(struct redfish_server* server)
{
    redfish_server_init(server);

    // Register Manager resource
    RETURN_IF_IERROR(redfish_helper_register_managers(
        server, manager_params, ARRAY_SIZE(manager_params)));

    // Register UpdateService resource
    RETURN_IF_IERROR(
        redfish_server_register_update_service(server, &update_service_params));

    // Register Software Inventories
    RETURN_IF_IERROR(redfish_helper_register_software_inventories(
        server, software_inventory_params,
        ARRAY_SIZE(software_inventory_params)));

    // Register tray chassis and sensors
    RETURN_IF_IERROR(rde_create_tray_chassis(server));
    RETURN_IF_IERROR(redfish_helper_register_sensors(
        server, tray_sensor_params, ARRAY_SIZE(tray_sensor_params)));

    // Register Controls for Tray Chassis
    RETURN_IF_IERROR(redfish_helper_register_controls(
        server, control_params, ARRAY_SIZE(control_params)));

    // Register HDD chassis, sensors and drives.
    RETURN_IF_IERROR(redfish_helper_register_chassis(
        server, hdd_chassis_params, ARRAY_SIZE(hdd_chassis_params)));
    RETURN_IF_IERROR(redfish_helper_register_sensors(
        server, hdd_sensor_params, ARRAY_SIZE(hdd_sensor_params)));
    RETURN_IF_IERROR(redfish_helper_register_drives(server, drive_params,
                                                    ARRAY_SIZE(drive_params)));

    // Register the storage and storage controller.
    RETURN_IF_IERROR(redfish_server_register_storage(server, &storage_params));
    RETURN_IF_IERROR(redfish_server_register_storage_controller(
        server, &storage_controller_params));

    return 0;
}
