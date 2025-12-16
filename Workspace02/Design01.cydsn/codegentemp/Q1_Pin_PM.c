/*******************************************************************************
* File Name: Q1_Pin.c  
* Version 2.20
*
* Description:
*  This file contains APIs to set up the Pins component for low power modes.
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "Q1_Pin.h"

static Q1_Pin_BACKUP_STRUCT  Q1_Pin_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: Q1_Pin_Sleep
****************************************************************************//**
*
* \brief Stores the pin configuration and prepares the pin for entering chip 
*  deep-sleep/hibernate modes. This function applies only to SIO and USBIO pins.
*  It should not be called for GPIO or GPIO_OVT pins.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None 
*  
* \sideeffect
*  For SIO pins, this function configures the pin input threshold to CMOS and
*  drive level to Vddio. This is needed for SIO pins when in device 
*  deep-sleep/hibernate modes.
*
* \funcusage
*  \snippet Q1_Pin_SUT.c usage_Q1_Pin_Sleep_Wakeup
*******************************************************************************/
void Q1_Pin_Sleep(void)
{
    #if defined(Q1_Pin__PC)
        Q1_Pin_backup.pcState = Q1_Pin_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            Q1_Pin_backup.usbState = Q1_Pin_CR1_REG;
            Q1_Pin_USB_POWER_REG |= Q1_Pin_USBIO_ENTER_SLEEP;
            Q1_Pin_CR1_REG &= Q1_Pin_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(Q1_Pin__SIO)
        Q1_Pin_backup.sioState = Q1_Pin_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        Q1_Pin_SIO_REG &= (uint32)(~Q1_Pin_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: Q1_Pin_Wakeup
****************************************************************************//**
*
* \brief Restores the pin configuration that was saved during Pin_Sleep(). This 
* function applies only to SIO and USBIO pins. It should not be called for
* GPIO or GPIO_OVT pins.
*
* For USBIO pins, the wakeup is only triggered for falling edge interrupts.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None
*  
* \funcusage
*  Refer to Q1_Pin_Sleep() for an example usage.
*******************************************************************************/
void Q1_Pin_Wakeup(void)
{
    #if defined(Q1_Pin__PC)
        Q1_Pin_PC = Q1_Pin_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            Q1_Pin_USB_POWER_REG &= Q1_Pin_USBIO_EXIT_SLEEP_PH1;
            Q1_Pin_CR1_REG = Q1_Pin_backup.usbState;
            Q1_Pin_USB_POWER_REG &= Q1_Pin_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(Q1_Pin__SIO)
        Q1_Pin_SIO_REG = Q1_Pin_backup.sioState;
    #endif
}


/* [] END OF FILE */
