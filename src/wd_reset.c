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

#include <kernel.h>
#include <smc/wdt.h>
#include <soc.h>

#define SYS_WDT2_FULL_RESET BIT(21)
#define SYS_WDT2_SOC_RESET BIT(20)

/**
 * Watchdog reset module for the AST1030/AST1035 chip
 */
static int smc_wdt_init(const struct device* dev)
{
    ARG_UNUSED(dev);

    bool wdt_reset = false;
    uint32_t reset_logs = sys_read32(SYS_RESET_LOG_REG1);
    if ((reset_logs & SYS_WDT2_SOC_RESET) || (reset_logs & SYS_WDT2_FULL_RESET))
    {
        wdt_reset = true;
    }

    wdt_set_system_reset_count(wdt_reset);

    // This will print the last reset information and also clear the reset logs.
    aspeed_print_sysrst_info();

    return wdt_init(CONFIG_WDT_RESET_CPU_TIMEOUT_MS, WDT_FLAG_RESET_CPU_CORE,
                    NULL);
}
SYS_INIT(smc_wdt_init, POST_KERNEL, CONFIG_APPLICATION_INIT_PRIORITY);
