/*******************************************************************************
* File Name: Test_Clock.c
* Version 2.20
*
*  Description:
*   Provides system API for the clocking, interrupts and watchdog timer.
*
*  Note:
*   Documentation of the API's in this file is located in the
*   System Reference Guide provided with PSoC Creator.
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <cydevice_trm.h>
#include "Test_Clock.h"

#if defined CYREG_PERI_DIV_CMD

/*******************************************************************************
* Function Name: Test_Clock_StartEx
********************************************************************************
*
* Summary:
*  Starts the clock, aligned to the specified running clock.
*
* Parameters:
*  alignClkDiv:  The divider to which phase alignment is performed when the
*    clock is started.
*
* Returns:
*  None
*
*******************************************************************************/
void Test_Clock_StartEx(uint32 alignClkDiv)
{
    /* Make sure any previous start command has finished. */
    while((Test_Clock_CMD_REG & Test_Clock_CMD_ENABLE_MASK) != 0u)
    {
    }
    
    /* Specify the target divider and it's alignment divider, and enable. */
    Test_Clock_CMD_REG =
        ((uint32)Test_Clock__DIV_ID << Test_Clock_CMD_DIV_SHIFT)|
        (alignClkDiv << Test_Clock_CMD_PA_DIV_SHIFT) |
        (uint32)Test_Clock_CMD_ENABLE_MASK;
}

#else

/*******************************************************************************
* Function Name: Test_Clock_Start
********************************************************************************
*
* Summary:
*  Starts the clock.
*
* Parameters:
*  None
*
* Returns:
*  None
*
*******************************************************************************/

void Test_Clock_Start(void)
{
    /* Set the bit to enable the clock. */
    Test_Clock_ENABLE_REG |= Test_Clock__ENABLE_MASK;
}

#endif /* CYREG_PERI_DIV_CMD */


/*******************************************************************************
* Function Name: Test_Clock_Stop
********************************************************************************
*
* Summary:
*  Stops the clock and returns immediately. This API does not require the
*  source clock to be running but may return before the hardware is actually
*  disabled.
*
* Parameters:
*  None
*
* Returns:
*  None
*
*******************************************************************************/
void Test_Clock_Stop(void)
{
#if defined CYREG_PERI_DIV_CMD

    /* Make sure any previous start command has finished. */
    while((Test_Clock_CMD_REG & Test_Clock_CMD_ENABLE_MASK) != 0u)
    {
    }
    
    /* Specify the target divider and it's alignment divider, and disable. */
    Test_Clock_CMD_REG =
        ((uint32)Test_Clock__DIV_ID << Test_Clock_CMD_DIV_SHIFT)|
        ((uint32)Test_Clock_CMD_DISABLE_MASK);

#else

    /* Clear the bit to disable the clock. */
    Test_Clock_ENABLE_REG &= (uint32)(~Test_Clock__ENABLE_MASK);
    
#endif /* CYREG_PERI_DIV_CMD */
}


/*******************************************************************************
* Function Name: Test_Clock_SetFractionalDividerRegister
********************************************************************************
*
* Summary:
*  Modifies the clock divider and the fractional divider.
*
* Parameters:
*  clkDivider:  Divider register value (0-65535). This value is NOT the
*    divider; the clock hardware divides by clkDivider plus one. For example,
*    to divide the clock by 2, this parameter should be set to 1.
*  fracDivider:  Fractional Divider register value (0-31).
* Returns:
*  None
*
*******************************************************************************/
void Test_Clock_SetFractionalDividerRegister(uint16 clkDivider, uint8 clkFractional)
{
    uint32 maskVal;
    uint32 regVal;
    
#if defined (Test_Clock__FRAC_MASK) || defined (CYREG_PERI_DIV_CMD)
    
	/* get all but divider bits */
    maskVal = Test_Clock_DIV_REG & 
                    (uint32)(~(uint32)(Test_Clock_DIV_INT_MASK | Test_Clock_DIV_FRAC_MASK)); 
	/* combine mask and new divider vals into 32-bit value */
    regVal = maskVal |
        ((uint32)((uint32)clkDivider <<  Test_Clock_DIV_INT_SHIFT) & Test_Clock_DIV_INT_MASK) |
        ((uint32)((uint32)clkFractional << Test_Clock_DIV_FRAC_SHIFT) & Test_Clock_DIV_FRAC_MASK);
    
#else
    /* get all but integer divider bits */
    maskVal = Test_Clock_DIV_REG & (uint32)(~(uint32)Test_Clock__DIVIDER_MASK);
    /* combine mask and new divider val into 32-bit value */
    regVal = clkDivider | maskVal;
    
#endif /* Test_Clock__FRAC_MASK || CYREG_PERI_DIV_CMD */

    Test_Clock_DIV_REG = regVal;
}


/*******************************************************************************
* Function Name: Test_Clock_GetDividerRegister
********************************************************************************
*
* Summary:
*  Gets the clock divider register value.
*
* Parameters:
*  None
*
* Returns:
*  Divide value of the clock minus 1. For example, if the clock is set to
*  divide by 2, the return value will be 1.
*
*******************************************************************************/
uint16 Test_Clock_GetDividerRegister(void)
{
    return (uint16)((Test_Clock_DIV_REG & Test_Clock_DIV_INT_MASK)
        >> Test_Clock_DIV_INT_SHIFT);
}


/*******************************************************************************
* Function Name: Test_Clock_GetFractionalDividerRegister
********************************************************************************
*
* Summary:
*  Gets the clock fractional divider register value.
*
* Parameters:
*  None
*
* Returns:
*  Fractional Divide value of the clock
*  0 if the fractional divider is not in use.
*
*******************************************************************************/
uint8 Test_Clock_GetFractionalDividerRegister(void)
{
#if defined (Test_Clock__FRAC_MASK)
    /* return fractional divider bits */
    return (uint8)((Test_Clock_DIV_REG & Test_Clock_DIV_FRAC_MASK)
        >> Test_Clock_DIV_FRAC_SHIFT);
#else
    return 0u;
#endif /* Test_Clock__FRAC_MASK */
}


/* [] END OF FILE */
