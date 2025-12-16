/*******************************************************************************
* File Name: Test_Clock.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_Test_Clock_H)
#define CY_CLOCK_Test_Clock_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
*        Function Prototypes
***************************************/
#if defined CYREG_PERI_DIV_CMD

void Test_Clock_StartEx(uint32 alignClkDiv);
#define Test_Clock_Start() \
    Test_Clock_StartEx(Test_Clock__PA_DIV_ID)

#else

void Test_Clock_Start(void);

#endif/* CYREG_PERI_DIV_CMD */

void Test_Clock_Stop(void);

void Test_Clock_SetFractionalDividerRegister(uint16 clkDivider, uint8 clkFractional);

uint16 Test_Clock_GetDividerRegister(void);
uint8  Test_Clock_GetFractionalDividerRegister(void);

#define Test_Clock_Enable()                         Test_Clock_Start()
#define Test_Clock_Disable()                        Test_Clock_Stop()
#define Test_Clock_SetDividerRegister(clkDivider, reset)  \
    Test_Clock_SetFractionalDividerRegister((clkDivider), 0u)
#define Test_Clock_SetDivider(clkDivider)           Test_Clock_SetDividerRegister((clkDivider), 1u)
#define Test_Clock_SetDividerValue(clkDivider)      Test_Clock_SetDividerRegister((clkDivider) - 1u, 1u)


/***************************************
*             Registers
***************************************/
#if defined CYREG_PERI_DIV_CMD

#define Test_Clock_DIV_ID     Test_Clock__DIV_ID

#define Test_Clock_CMD_REG    (*(reg32 *)CYREG_PERI_DIV_CMD)
#define Test_Clock_CTRL_REG   (*(reg32 *)Test_Clock__CTRL_REGISTER)
#define Test_Clock_DIV_REG    (*(reg32 *)Test_Clock__DIV_REGISTER)

#define Test_Clock_CMD_DIV_SHIFT          (0u)
#define Test_Clock_CMD_PA_DIV_SHIFT       (8u)
#define Test_Clock_CMD_DISABLE_SHIFT      (30u)
#define Test_Clock_CMD_ENABLE_SHIFT       (31u)

#define Test_Clock_CMD_DISABLE_MASK       ((uint32)((uint32)1u << Test_Clock_CMD_DISABLE_SHIFT))
#define Test_Clock_CMD_ENABLE_MASK        ((uint32)((uint32)1u << Test_Clock_CMD_ENABLE_SHIFT))

#define Test_Clock_DIV_FRAC_MASK  (0x000000F8u)
#define Test_Clock_DIV_FRAC_SHIFT (3u)
#define Test_Clock_DIV_INT_MASK   (0xFFFFFF00u)
#define Test_Clock_DIV_INT_SHIFT  (8u)

#else 

#define Test_Clock_DIV_REG        (*(reg32 *)Test_Clock__REGISTER)
#define Test_Clock_ENABLE_REG     Test_Clock_DIV_REG
#define Test_Clock_DIV_FRAC_MASK  Test_Clock__FRAC_MASK
#define Test_Clock_DIV_FRAC_SHIFT (16u)
#define Test_Clock_DIV_INT_MASK   Test_Clock__DIVIDER_MASK
#define Test_Clock_DIV_INT_SHIFT  (0u)

#endif/* CYREG_PERI_DIV_CMD */

#endif /* !defined(CY_CLOCK_Test_Clock_H) */

/* [] END OF FILE */
