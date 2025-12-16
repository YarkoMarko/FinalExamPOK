/*******************************************************************************
* File Name: PWM_Shift_PM.c
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

#include "PWM_Shift.h"

static PWM_Shift_backupStruct PWM_Shift_backup;


/*******************************************************************************
* Function Name: PWM_Shift_SaveConfig
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
*  PWM_Shift_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void PWM_Shift_SaveConfig(void) 
{

    #if(!PWM_Shift_UsingFixedFunction)
        #if(!PWM_Shift_PWMModeIsCenterAligned)
            PWM_Shift_backup.PWMPeriod = PWM_Shift_ReadPeriod();
        #endif /* (!PWM_Shift_PWMModeIsCenterAligned) */
        PWM_Shift_backup.PWMUdb = PWM_Shift_ReadCounter();
        #if (PWM_Shift_UseStatus)
            PWM_Shift_backup.InterruptMaskValue = PWM_Shift_STATUS_MASK;
        #endif /* (PWM_Shift_UseStatus) */

        #if(PWM_Shift_DeadBandMode == PWM_Shift__B_PWM__DBM_256_CLOCKS || \
            PWM_Shift_DeadBandMode == PWM_Shift__B_PWM__DBM_2_4_CLOCKS)
            PWM_Shift_backup.PWMdeadBandValue = PWM_Shift_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(PWM_Shift_KillModeMinTime)
             PWM_Shift_backup.PWMKillCounterPeriod = PWM_Shift_ReadKillTime();
        #endif /* (PWM_Shift_KillModeMinTime) */

        #if(PWM_Shift_UseControl)
            PWM_Shift_backup.PWMControlRegister = PWM_Shift_ReadControlRegister();
        #endif /* (PWM_Shift_UseControl) */
    #endif  /* (!PWM_Shift_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: PWM_Shift_RestoreConfig
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
*  PWM_Shift_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void PWM_Shift_RestoreConfig(void) 
{
        #if(!PWM_Shift_UsingFixedFunction)
            #if(!PWM_Shift_PWMModeIsCenterAligned)
                PWM_Shift_WritePeriod(PWM_Shift_backup.PWMPeriod);
            #endif /* (!PWM_Shift_PWMModeIsCenterAligned) */

            PWM_Shift_WriteCounter(PWM_Shift_backup.PWMUdb);

            #if (PWM_Shift_UseStatus)
                PWM_Shift_STATUS_MASK = PWM_Shift_backup.InterruptMaskValue;
            #endif /* (PWM_Shift_UseStatus) */

            #if(PWM_Shift_DeadBandMode == PWM_Shift__B_PWM__DBM_256_CLOCKS || \
                PWM_Shift_DeadBandMode == PWM_Shift__B_PWM__DBM_2_4_CLOCKS)
                PWM_Shift_WriteDeadTime(PWM_Shift_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(PWM_Shift_KillModeMinTime)
                PWM_Shift_WriteKillTime(PWM_Shift_backup.PWMKillCounterPeriod);
            #endif /* (PWM_Shift_KillModeMinTime) */

            #if(PWM_Shift_UseControl)
                PWM_Shift_WriteControlRegister(PWM_Shift_backup.PWMControlRegister);
            #endif /* (PWM_Shift_UseControl) */
        #endif  /* (!PWM_Shift_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: PWM_Shift_Sleep
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
*  PWM_Shift_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void PWM_Shift_Sleep(void) 
{
    #if(PWM_Shift_UseControl)
        if(PWM_Shift_CTRL_ENABLE == (PWM_Shift_CONTROL & PWM_Shift_CTRL_ENABLE))
        {
            /*Component is enabled */
            PWM_Shift_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            PWM_Shift_backup.PWMEnableState = 0u;
        }
    #endif /* (PWM_Shift_UseControl) */

    /* Stop component */
    PWM_Shift_Stop();

    /* Save registers configuration */
    PWM_Shift_SaveConfig();
}


/*******************************************************************************
* Function Name: PWM_Shift_Wakeup
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
*  PWM_Shift_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void PWM_Shift_Wakeup(void) 
{
     /* Restore registers values */
    PWM_Shift_RestoreConfig();

    if(PWM_Shift_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        PWM_Shift_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
