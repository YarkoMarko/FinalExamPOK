/*******************************************************************************
* File Name: ShiftClock_PWM_PM.c
* Version 3.30
*
* Description:
*  This file provides the power management source code to API for the
*  PWM.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "ShiftClock_PWM.h"

static ShiftClock_PWM_backupStruct ShiftClock_PWM_backup;


/*******************************************************************************
* Function Name: ShiftClock_PWM_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  ShiftClock_PWM_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void ShiftClock_PWM_SaveConfig(void) 
{

    #if(!ShiftClock_PWM_UsingFixedFunction)
        #if(!ShiftClock_PWM_PWMModeIsCenterAligned)
            ShiftClock_PWM_backup.PWMPeriod = ShiftClock_PWM_ReadPeriod();
        #endif /* (!ShiftClock_PWM_PWMModeIsCenterAligned) */
        ShiftClock_PWM_backup.PWMUdb = ShiftClock_PWM_ReadCounter();
        #if (ShiftClock_PWM_UseStatus)
            ShiftClock_PWM_backup.InterruptMaskValue = ShiftClock_PWM_STATUS_MASK;
        #endif /* (ShiftClock_PWM_UseStatus) */

        #if(ShiftClock_PWM_DeadBandMode == ShiftClock_PWM__B_PWM__DBM_256_CLOCKS || \
            ShiftClock_PWM_DeadBandMode == ShiftClock_PWM__B_PWM__DBM_2_4_CLOCKS)
            ShiftClock_PWM_backup.PWMdeadBandValue = ShiftClock_PWM_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(ShiftClock_PWM_KillModeMinTime)
             ShiftClock_PWM_backup.PWMKillCounterPeriod = ShiftClock_PWM_ReadKillTime();
        #endif /* (ShiftClock_PWM_KillModeMinTime) */

        #if(ShiftClock_PWM_UseControl)
            ShiftClock_PWM_backup.PWMControlRegister = ShiftClock_PWM_ReadControlRegister();
        #endif /* (ShiftClock_PWM_UseControl) */
    #endif  /* (!ShiftClock_PWM_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: ShiftClock_PWM_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  ShiftClock_PWM_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void ShiftClock_PWM_RestoreConfig(void) 
{
        #if(!ShiftClock_PWM_UsingFixedFunction)
            #if(!ShiftClock_PWM_PWMModeIsCenterAligned)
                ShiftClock_PWM_WritePeriod(ShiftClock_PWM_backup.PWMPeriod);
            #endif /* (!ShiftClock_PWM_PWMModeIsCenterAligned) */

            ShiftClock_PWM_WriteCounter(ShiftClock_PWM_backup.PWMUdb);

            #if (ShiftClock_PWM_UseStatus)
                ShiftClock_PWM_STATUS_MASK = ShiftClock_PWM_backup.InterruptMaskValue;
            #endif /* (ShiftClock_PWM_UseStatus) */

            #if(ShiftClock_PWM_DeadBandMode == ShiftClock_PWM__B_PWM__DBM_256_CLOCKS || \
                ShiftClock_PWM_DeadBandMode == ShiftClock_PWM__B_PWM__DBM_2_4_CLOCKS)
                ShiftClock_PWM_WriteDeadTime(ShiftClock_PWM_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(ShiftClock_PWM_KillModeMinTime)
                ShiftClock_PWM_WriteKillTime(ShiftClock_PWM_backup.PWMKillCounterPeriod);
            #endif /* (ShiftClock_PWM_KillModeMinTime) */

            #if(ShiftClock_PWM_UseControl)
                ShiftClock_PWM_WriteControlRegister(ShiftClock_PWM_backup.PWMControlRegister);
            #endif /* (ShiftClock_PWM_UseControl) */
        #endif  /* (!ShiftClock_PWM_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: ShiftClock_PWM_Sleep
********************************************************************************
*
* Summary:
*  Disables block's operation and saves the user configuration. Should be called
*  just prior to entering sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  ShiftClock_PWM_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void ShiftClock_PWM_Sleep(void) 
{
    #if(ShiftClock_PWM_UseControl)
        if(ShiftClock_PWM_CTRL_ENABLE == (ShiftClock_PWM_CONTROL & ShiftClock_PWM_CTRL_ENABLE))
        {
            /*Component is enabled */
            ShiftClock_PWM_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            ShiftClock_PWM_backup.PWMEnableState = 0u;
        }
    #endif /* (ShiftClock_PWM_UseControl) */

    /* Stop component */
    ShiftClock_PWM_Stop();

    /* Save registers configuration */
    ShiftClock_PWM_SaveConfig();
}


/*******************************************************************************
* Function Name: ShiftClock_PWM_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration. Should be called just after
*  awaking from sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  ShiftClock_PWM_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void ShiftClock_PWM_Wakeup(void) 
{
     /* Restore registers values */
    ShiftClock_PWM_RestoreConfig();

    if(ShiftClock_PWM_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        ShiftClock_PWM_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
