/*
 * Copyright (c) 2018-2020 Arm Limited. All rights reserved.
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

#include CMSIS_device_header
#include "stm32l5xx_hal.h"
#include "target_cfg.h"
#include "region_defs.h"
#include "tfm_plat_defs.h"

/* Macros to pick linker symbols */
#define REGION(a, b, c) a##b##c
#define REGION_NAME(a, b, c) REGION(a, b, c)
#define REGION_DECLARE(a, b, c) extern uint32_t REGION_NAME(a, b, c)

/* The section names come from the scatter file */
REGION_DECLARE(Load$$LR$$, LR_NS_PARTITION, $$Base);
REGION_DECLARE(Load$$LR$$, LR_VENEER, $$Base);
REGION_DECLARE(Load$$LR$$, LR_VENEER, $$Limit);
#ifdef BL2
REGION_DECLARE(Load$$LR$$, LR_SECONDARY_PARTITION, $$Base);
#endif /* BL2 */

const struct memory_region_limits memory_regions = {
    .non_secure_code_start =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_NS_PARTITION, $$Base) +
        BL2_HEADER_SIZE,

    .non_secure_partition_base =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_NS_PARTITION, $$Base),

    .non_secure_partition_limit =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_NS_PARTITION, $$Base) +
        NS_PARTITION_SIZE - 1,

    .veneer_base =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_VENEER, $$Base),

    .veneer_limit =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_VENEER, $$Limit),

#ifdef BL2
    .secondary_partition_base =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_SECONDARY_PARTITION, $$Base),

    .secondary_partition_limit =
        (uint32_t)&REGION_NAME(Load$$LR$$, LR_SECONDARY_PARTITION, $$Base) +
        SECONDARY_PARTITION_SIZE - 1,
#endif /* BL2 */
};

/* Define Peripherals NS address range for the platform */
#define PERIPHERALS_BASE_NS_START (0x40000000)
#define PERIPHERALS_BASE_NS_END   (0x4FFFFFFF)

/* To write into AIRCR register, 0x5FA value must be write to the VECTKEY field,
 * otherwise the processor ignores the write.
 */
#define SCB_AIRCR_WRITE_MASK ((0x5FAUL << SCB_AIRCR_VECTKEY_Pos))

struct tfm_spm_partition_platform_data_t tfm_peripheral_std_uart = {
  /* USART1 (NS) */
  0x40013800U,
  0x40013BFFU,
  PERIPHERAL_NONE /*GTZC_PERIPH_USART1*/
};

struct tfm_spm_partition_platform_data_t tfm_peripheral_timer0 = {
  /* TIM1 (NS) */
  0x40012C00U,
  0x40012FFFU,
  PERIPHERAL_NONE /*GTZC_PERIPH_TIM1*/
};

enum tfm_plat_err_t enable_fault_handlers(void)
{
    /* Explicitly set secure fault priority to the highest */
    NVIC_SetPriority(SecureFault_IRQn, 0);

    /* Enables BUS, MEM, USG and Secure faults */
    SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk
                  | SCB_SHCSR_BUSFAULTENA_Msk
                  | SCB_SHCSR_MEMFAULTENA_Msk
                  | SCB_SHCSR_SECUREFAULTENA_Msk;
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t system_reset_cfg(void)
{
    uint32_t reg_value = SCB->AIRCR;

    /* Clear SCB_AIRCR_VECTKEY value */
    reg_value &= ~(uint32_t)(SCB_AIRCR_VECTKEY_Msk);

    /* Enable system reset request only to the secure world */
    reg_value |= (uint32_t)(SCB_AIRCR_WRITE_MASK | SCB_AIRCR_SYSRESETREQS_Msk);

    SCB->AIRCR = reg_value;

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t init_debug(void)
{
//#if defined(DAUTH_NONE)
//#else

//#if !defined(DAUTH_CHIP_DEFAULT)
//#error "No debug authentication setting is provided."
//#endif

//#endif
    return TFM_PLAT_ERR_SUCCESS;
}

/*----------------- NVIC interrupt target state to NS configuration ----------*/
enum tfm_plat_err_t nvic_interrupt_target_state_cfg(void)
{
    /* Target every interrupt to NS; unimplemented interrupts will be WI */
//  Configured via CMSIS-Zone generated code
//  for (uint8_t i=0; i<sizeof(NVIC->ITNS)/sizeof(NVIC->ITNS[0]); i++) {
//      NVIC->ITNS[i] = 0xFFFFFFFF;
//  }

    /* Make sure that GTZC is targeted to S state */
    NVIC_ClearTargetState(GTZC_IRQn);
    return TFM_PLAT_ERR_SUCCESS;
}

/*----------------- NVIC interrupt enabling for S peripherals ----------------*/
enum tfm_plat_err_t nvic_interrupt_enable(void)
{
    NVIC_EnableIRQ(GTZC_IRQn);
    return TFM_PLAT_ERR_SUCCESS;
}

/*------------------- SAU/IDAU configuration functions -----------------------*/

void sau_and_idau_cfg(void)
{
}

/*------------------- Memory configuration functions -------------------------*/

void mpc_init_cfg(void)
{
}

/*---------------------- PPC configuration functions -------------------------*/

void ppc_init_cfg(void)
{
}

void ppc_configure_to_non_secure(uint32_t periph_id)
{
    /* Clear Secure flag for peripheral to enable NS access */
    uint32_t PeriphAttributes;

    HAL_GTZC_TZSC_GetConfigPeriphAttributes(periph_id, &PeriphAttributes);
    PeriphAttributes &= ~GTZC_TZSC_PERIPH_SEC;
    HAL_GTZC_TZSC_ConfigPeriphAttributes(periph_id, PeriphAttributes);
}

void ppc_configure_to_secure(uint32_t periph_id)
{
    /* Set Secure flag for peripheral to prevent NS access */
    uint32_t PeriphAttributes;

    HAL_GTZC_TZSC_GetConfigPeriphAttributes(periph_id, &PeriphAttributes);
    PeriphAttributes |= GTZC_TZSC_PERIPH_SEC;
    HAL_GTZC_TZSC_ConfigPeriphAttributes(periph_id, PeriphAttributes);
}

void ppc_en_secure_unpriv(uint32_t periph_id)
{
    /* Clear Privilege access flag for peripheral */
    uint32_t PeriphAttributes;

    HAL_GTZC_TZSC_GetConfigPeriphAttributes(periph_id, &PeriphAttributes);
    PeriphAttributes &= ~GTZC_TZSC_PERIPH_NPRIV;
    HAL_GTZC_TZSC_ConfigPeriphAttributes(periph_id, PeriphAttributes);
}

void ppc_clr_secure_unpriv(uint32_t periph_id)
{
    /* Set Privilege access flag for peripheral */
    uint32_t PeriphAttributes;

    HAL_GTZC_TZSC_GetConfigPeriphAttributes(periph_id, &PeriphAttributes);
    PeriphAttributes |= GTZC_TZSC_PERIPH_NPRIV;
    HAL_GTZC_TZSC_ConfigPeriphAttributes(periph_id, PeriphAttributes);
}
