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

#include <bej_tree.h>
#include <logging/log.h>
#include <smc/rde/common.h>
#include <smc/utils.h>
#include <smc/version.h>

LOG_MODULE_REGISTER(rde_oem, LOG_LEVEL_WRN);

struct software_inventory_oem_json
{
    struct RedfishPropertyParent oem_owner_set;
    struct RedfishPropertyLeafString build_info;
};

#ifdef CONFIG_BOARD_NATIVE_POSIX_64BIT
#define RDE_OEM_JSON_MAX_SIZE 1048
#else
#define RDE_OEM_JSON_MAX_SIZE 512
#endif
_Static_assert(
    RDE_OEM_JSON_MAX_SIZE >= sizeof(struct software_inventory_oem_json),
    "RDE_OEM_JSON_MAX_SIZE is too small for software_inventory_oem_json");

/**
 * @brief Memory for representing OEM data in rde bejTree api.
 *
 * Since this will be casted to bejTree structs, we need to make sure that this
 * array is 4 byte aligned.
 */
static uint8_t __attribute__((aligned(
    4))) oem_json_buffer[CONFIG_SMC_RDE_CONCURRENCY][RDE_OEM_JSON_MAX_SIZE];

// Only override the redfish_get_software_inventory_oem if a OEM dictionary is
// avaiable.
#ifndef CONFIG_SMC_RDE_INCLUDE_SOFTWARE_INVENTORY_DICT
int redfish_get_software_inventory_oem(uint8_t operation_index,
                                       struct RedfishPropertyParent* oem_root)
{
    IS_PARAM_NULL(oem_root, "oem_root is  NULL in software_inventory oem");

    struct software_inventory_oem_json* resource =
        (struct software_inventory_oem_json*)&oem_json_buffer[operation_index]
                                                             [0];
    struct RedfishPropertyParent* parent = &resource->oem_owner_set;

    bejTreeInitSet(parent, "Smc");
    bejTreeLinkChildToParent(oem_root, parent);

    return redfish_add_string_to_json(parent, &resource->build_info,
                                      "BuildInfo", smc_get_build_info());
}
#endif /* CONFIG_SMC_RDE_INCLUDE_SOFTWARE_INVENTORY_DICT */
