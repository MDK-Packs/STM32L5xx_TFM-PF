<#compress>
<#include "helper.ftlinc"/>

<#assign aDateTime = .now>

<#-- Prepare SAU entries -->
<#assign sau_entries = 0>
<#assign sau_table = []/>

<#if system.sau?has_content>
  <#list system.sau?sort_by("start") as s>
    <#assign sau_table += [{"start_address":s.start,      "end_address":s.end,        "nsc":s.nsc, "init":1}]/>
    <#assign sau_entries += 1>
  </#list>
</#if>
  <#list sau_entries..7 as i>
    <#assign sau_table += [{"start_address":"0x00000000", "end_address":"0x00000000", "nsc":0,     "init":0}]/>
  </#list>

<#-- Prepare Non-Secure Interrupt Entries -->
<#assign itns_entries = 0>
<#assign itns_table = []/>
<#assign maxirq = 0/>
<#assign it = {}/>
<#assign itns = {}/>

<#if system.interrupt?has_content>
  <#list system.interrupt?sort_by("irqn") as irq>
    <#assign it += { irq.irqn?number : irq } />  
    <#if (irq.irqn?number > maxirq)>
	  <#assign maxirq = irq.irqn?number>
    </#if>
    <#if irq.security.n == "1">
      <#assign itns += { irq.irqn?number : irq } />
    </#if>
  </#list>
</#if>

<#list 0..(maxirq/32)?floor as i>
  <#assign itns_val  = 0>
  <#assign itns_init = 0>
  <#list 0..31 as j>
    <#if itns?keys?seq_contains((i * 32 + j)?c)>
      <#assign itns_init = 1>
      <#assign itns_val += pow2(j)>
    </#if>
  </#list>
  <#assign itns_table += [{"val" : num2hex(itns_val, "0x", 8), "init" : itns_init}]/>
</#list>
</#compress>
/**************************************************************************//**
 * @file     partition_gen.h
 * @brief    CMSIS-CORE Initial Setup for Secure / Non-Secure Zones for STM32L5xx Device
 * @version  V1.0.0      (FreeMarker generated)
 * @date     ${aDateTime?date} ${aDateTime?time}
 ******************************************************************************/
/*
 * Copyright (c) 2009-2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PARTITION_GEN_H
#define PARTITION_GEN_H

/*
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
*/

/*
// <h>Initialize Security Attribution Unit (SAU) Address Regions
// <i>SAU configuration specifies regions to be one of:
// <i> - Secure and Non-Secure Callable
// <i> - Non-Secure
// <i>Note: All memory regions not configured by SAU are Secure
*/
#define SAU_REGIONS_MAX   ${sau_table?size}                 /* Max. number of SAU regions */

<#list sau_table as sau_entry>
/*
//   <e>Initialize SAU Region ${sau_entry?index}
//   <i> Setup SAU Region ${sau_entry?index} memory attributes
*/
#define SAU_INIT_REGION${sau_entry?index}    ${sau_entry.init}

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START${sau_entry?index}     ${sau_entry.start_address}      /* start address of SAU region ${sau_entry?index} */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END${sau_entry?index}       ${sau_entry.end_address}      /* end address of SAU region ${sau_entry?index} */

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC${sau_entry?index}       ${sau_entry.nsc}
/*
//   </e>
*/

</#list>
/*
// </h>
*/


/*
// <h>Setup Interrupt Target
*/

<#list itns_table as itns_entry>
/*
//   <e>Initialize ITNS ${itns_entry?index} (Interrupts ${itns_entry?index*32}..${itns_entry?index*32+31})
*/
#define NVIC_INIT_ITNS${itns_entry?index}    ${itns_entry.init}

/*
// Interrupts ${itns_entry?index*32}..${itns_entry?index*32+31}
<#list 0..31 as i>
//   <o.${i}> Interrupt ${(itns_entry?index*32+i)?left_pad(3)}<#if it?keys?seq_contains((itns_entry?index*32+i)?c)>: ${it[(itns_entry?index*32+i)?c].name}</#if> <0=> Secure state <1=> Non-Secure state
</#list>
*/
#define NVIC_INIT_ITNS${itns_entry?index}_VAL      ${itns_entry.val}

/*
//   </e>
*/

</#list>

/*
// </h>
*/

#endif  /* PARTITION_GEN_H */
