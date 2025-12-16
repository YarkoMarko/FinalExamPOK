/*******************************************************************************
* File Name: Timer_SCHP_PM.c
* Version 2.80
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "Timer_SCHP.h"

static Timer_SCHP_backupStruct Timer_SCHP_backup;


/*******************************************************************************
* Function Name: Timer_SCHP_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_SCHP_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_SCHP_SaveConfig(void) 
{
    #if (!Timer_SCHP_UsingFixedFunction)
        Timer_SCHP_backup.TimerUdb = Timer_SCHP_ReadCounter();
        Timer_SCHP_backup.InterruptMaskValue = Timer_SCHP_STATUS_MASK;
        #if (Timer_SCHP_UsingHWCaptureCounter)
            Timer_SCHP_backup.TimerCaptureCounter = Timer_SCHP_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_SCHP_UDB_CONTROL_REG_REMOVED)
            Timer_SCHP_backup.TimerControlRegister = Timer_SCHP_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_SCHP_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_SCHP_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_SCHP_RestoreConfig(void) 
{   
    #if (!Timer_SCHP_UsingFixedFunction)

        Timer_SCHP_WriteCounter(Timer_SCHP_backup.TimerUdb);
        Timer_SCHP_STATUS_MASK =Timer_SCHP_backup.InterruptMaskValue;
        #if (Timer_SCHP_UsingHWCaptureCounter)
            Timer_SCHP_SetCaptureCount(Timer_SCHP_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_SCHP_UDB_CONTROL_REG_REMOVED)
            Timer_SCHP_WriteControlRegister(Timer_SCHP_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_SCHP_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_SCHP_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_SCHP_Sleep(void) 
{
    #if(!Timer_SCHP_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_SCHP_CTRL_ENABLE == (Timer_SCHP_CONTROL & Timer_SCHP_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_SCHP_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_SCHP_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_SCHP_Stop();
    Timer_SCHP_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_SCHP_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_SCHP_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_SCHP_Wakeup(void) 
{
    Timer_SCHP_RestoreConfig();
    #if(!Timer_SCHP_UDB_CONTROL_REG_REMOVED)
        if(Timer_SCHP_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_SCHP_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
