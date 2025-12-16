/*******************************************************************************
* File Name: ShiftClock_PWM.c
* Version 3.30
*
* Description:
*  The PWM User Module consist of an 8 or 16-bit counter with two 8 or 16-bit
*  comparitors. Each instance of this user module is capable of generating
*  two PWM outputs with the same period. The pulse width is selectable between
*  1 and 255/65535. The period is selectable between 2 and 255/65536 clocks.
*  The compare value output may be configured to be active when the present
*  counter is less than or less than/equal to the compare value.
*  A terminal count output is also provided. It generates a pulse one clock
*  width wide when the counter is equal to zero.
*
* Note:
*
*******************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "ShiftClock_PWM.h"

/* Error message for removed <resource> through optimization */
#ifdef ShiftClock_PWM_PWMUDB_genblk1_ctrlreg__REMOVED
    #error PWM_v3_30 detected with a constant 0 for the enable or \
         constant 1 for reset. This will prevent the component from operating.
#endif /* ShiftClock_PWM_PWMUDB_genblk1_ctrlreg__REMOVED */

uint8 ShiftClock_PWM_initVar = 0u;


/*******************************************************************************
* Function Name: ShiftClock_PWM_Start
********************************************************************************
*
* Summary:
*  The start function initializes the pwm with the default values, the
*  enables the counter to begin counting.  It does not enable interrupts,
*  the EnableInt command should be called if interrupt generation is required.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  ShiftClock_PWM_initVar: Is modified when this function is called for the
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void ShiftClock_PWM_Start(void) 
{
    /* If not Initialized then initialize all required hardware and software */
    if(ShiftClock_PWM_initVar == 0u)
    {
        ShiftClock_PWM_Init();
        ShiftClock_PWM_initVar = 1u;
    }
    ShiftClock_PWM_Enable();

}


/*******************************************************************************
* Function Name: ShiftClock_PWM_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the
*  customizer of the component placed onto schematic. Usually called in
*  ShiftClock_PWM_Start().
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void ShiftClock_PWM_Init(void) 
{
    #if (ShiftClock_PWM_UsingFixedFunction || ShiftClock_PWM_UseControl)
        uint8 ctrl;
    #endif /* (ShiftClock_PWM_UsingFixedFunction || ShiftClock_PWM_UseControl) */

    #if(!ShiftClock_PWM_UsingFixedFunction)
        #if(ShiftClock_PWM_UseStatus)
            /* Interrupt State Backup for Critical Region*/
            uint8 ShiftClock_PWM_interruptState;
        #endif /* (ShiftClock_PWM_UseStatus) */
    #endif /* (!ShiftClock_PWM_UsingFixedFunction) */

    #if (ShiftClock_PWM_UsingFixedFunction)
        /* You are allowed to write the compare value (FF only) */
        ShiftClock_PWM_CONTROL |= ShiftClock_PWM_CFG0_MODE;
        #if (ShiftClock_PWM_DeadBand2_4)
            ShiftClock_PWM_CONTROL |= ShiftClock_PWM_CFG0_DB;
        #endif /* (ShiftClock_PWM_DeadBand2_4) */

        ctrl = ShiftClock_PWM_CONTROL3 & ((uint8 )(~ShiftClock_PWM_CTRL_CMPMODE1_MASK));
        ShiftClock_PWM_CONTROL3 = ctrl | ShiftClock_PWM_DEFAULT_COMPARE1_MODE;

         /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
        ShiftClock_PWM_RT1 &= ((uint8)(~ShiftClock_PWM_RT1_MASK));
        ShiftClock_PWM_RT1 |= ShiftClock_PWM_SYNC;

        /*Enable DSI Sync all all inputs of the PWM*/
        ShiftClock_PWM_RT1 &= ((uint8)(~ShiftClock_PWM_SYNCDSI_MASK));
        ShiftClock_PWM_RT1 |= ShiftClock_PWM_SYNCDSI_EN;

    #elif (ShiftClock_PWM_UseControl)
        /* Set the default compare mode defined in the parameter */
        ctrl = ShiftClock_PWM_CONTROL & ((uint8)(~ShiftClock_PWM_CTRL_CMPMODE2_MASK)) &
                ((uint8)(~ShiftClock_PWM_CTRL_CMPMODE1_MASK));
        ShiftClock_PWM_CONTROL = ctrl | ShiftClock_PWM_DEFAULT_COMPARE2_MODE |
                                   ShiftClock_PWM_DEFAULT_COMPARE1_MODE;
    #endif /* (ShiftClock_PWM_UsingFixedFunction) */

    #if (!ShiftClock_PWM_UsingFixedFunction)
        #if (ShiftClock_PWM_Resolution == 8)
            /* Set FIFO 0 to 1 byte register for period*/
            ShiftClock_PWM_AUX_CONTROLDP0 |= (ShiftClock_PWM_AUX_CTRL_FIFO0_CLR);
        #else /* (ShiftClock_PWM_Resolution == 16)*/
            /* Set FIFO 0 to 1 byte register for period */
            ShiftClock_PWM_AUX_CONTROLDP0 |= (ShiftClock_PWM_AUX_CTRL_FIFO0_CLR);
            ShiftClock_PWM_AUX_CONTROLDP1 |= (ShiftClock_PWM_AUX_CTRL_FIFO0_CLR);
        #endif /* (ShiftClock_PWM_Resolution == 8) */

        ShiftClock_PWM_WriteCounter(ShiftClock_PWM_INIT_PERIOD_VALUE);
    #endif /* (!ShiftClock_PWM_UsingFixedFunction) */

    ShiftClock_PWM_WritePeriod(ShiftClock_PWM_INIT_PERIOD_VALUE);

        #if (ShiftClock_PWM_UseOneCompareMode)
            ShiftClock_PWM_WriteCompare(ShiftClock_PWM_INIT_COMPARE_VALUE1);
        #else
            ShiftClock_PWM_WriteCompare1(ShiftClock_PWM_INIT_COMPARE_VALUE1);
            ShiftClock_PWM_WriteCompare2(ShiftClock_PWM_INIT_COMPARE_VALUE2);
        #endif /* (ShiftClock_PWM_UseOneCompareMode) */

        #if (ShiftClock_PWM_KillModeMinTime)
            ShiftClock_PWM_WriteKillTime(ShiftClock_PWM_MinimumKillTime);
        #endif /* (ShiftClock_PWM_KillModeMinTime) */

        #if (ShiftClock_PWM_DeadBandUsed)
            ShiftClock_PWM_WriteDeadTime(ShiftClock_PWM_INIT_DEAD_TIME);
        #endif /* (ShiftClock_PWM_DeadBandUsed) */

    #if (ShiftClock_PWM_UseStatus || ShiftClock_PWM_UsingFixedFunction)
        ShiftClock_PWM_SetInterruptMode(ShiftClock_PWM_INIT_INTERRUPTS_MODE);
    #endif /* (ShiftClock_PWM_UseStatus || ShiftClock_PWM_UsingFixedFunction) */

    #if (ShiftClock_PWM_UsingFixedFunction)
        /* Globally Enable the Fixed Function Block chosen */
        ShiftClock_PWM_GLOBAL_ENABLE |= ShiftClock_PWM_BLOCK_EN_MASK;
        /* Set the Interrupt source to come from the status register */
        ShiftClock_PWM_CONTROL2 |= ShiftClock_PWM_CTRL2_IRQ_SEL;
    #else
        #if(ShiftClock_PWM_UseStatus)

            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            ShiftClock_PWM_interruptState = CyEnterCriticalSection();
            /* Use the interrupt output of the status register for IRQ output */
            ShiftClock_PWM_STATUS_AUX_CTRL |= ShiftClock_PWM_STATUS_ACTL_INT_EN_MASK;

             /* Exit Critical Region*/
            CyExitCriticalSection(ShiftClock_PWM_interruptState);

            /* Clear the FIFO to enable the ShiftClock_PWM_STATUS_FIFOFULL
                   bit to be set on FIFO full. */
            ShiftClock_PWM_ClearFIFO();
        #endif /* (ShiftClock_PWM_UseStatus) */
    #endif /* (ShiftClock_PWM_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: ShiftClock_PWM_Enable
********************************************************************************
*
* Summary:
*  Enables the PWM block operation
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  This works only if software enable mode is chosen
*
*******************************************************************************/
void ShiftClock_PWM_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (ShiftClock_PWM_UsingFixedFunction)
        ShiftClock_PWM_GLOBAL_ENABLE |= ShiftClock_PWM_BLOCK_EN_MASK;
        ShiftClock_PWM_GLOBAL_STBY_ENABLE |= ShiftClock_PWM_BLOCK_STBY_EN_MASK;
    #endif /* (ShiftClock_PWM_UsingFixedFunction) */

    /* Enable the PWM from the control register  */
    #if (ShiftClock_PWM_UseControl || ShiftClock_PWM_UsingFixedFunction)
        ShiftClock_PWM_CONTROL |= ShiftClock_PWM_CTRL_ENABLE;
    #endif /* (ShiftClock_PWM_UseControl || ShiftClock_PWM_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: ShiftClock_PWM_Stop
********************************************************************************
*
* Summary:
*  The stop function halts the PWM, but does not change any modes or disable
*  interrupts.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  If the Enable mode is set to Hardware only then this function
*  has no effect on the operation of the PWM
*
*******************************************************************************/
void ShiftClock_PWM_Stop(void) 
{
    #if (ShiftClock_PWM_UseControl || ShiftClock_PWM_UsingFixedFunction)
        ShiftClock_PWM_CONTROL &= ((uint8)(~ShiftClock_PWM_CTRL_ENABLE));
    #endif /* (ShiftClock_PWM_UseControl || ShiftClock_PWM_UsingFixedFunction) */

    /* Globally disable the Fixed Function Block chosen */
    #if (ShiftClock_PWM_UsingFixedFunction)
        ShiftClock_PWM_GLOBAL_ENABLE &= ((uint8)(~ShiftClock_PWM_BLOCK_EN_MASK));
        ShiftClock_PWM_GLOBAL_STBY_ENABLE &= ((uint8)(~ShiftClock_PWM_BLOCK_STBY_EN_MASK));
    #endif /* (ShiftClock_PWM_UsingFixedFunction) */
}

#if (ShiftClock_PWM_UseOneCompareMode)
    #if (ShiftClock_PWM_CompareMode1SW)


        /*******************************************************************************
        * Function Name: ShiftClock_PWM_SetCompareMode
        ********************************************************************************
        *
        * Summary:
        *  This function writes the Compare Mode for the pwm output when in Dither mode,
        *  Center Align Mode or One Output Mode.
        *
        * Parameters:
        *  comparemode:  The new compare mode for the PWM output. Use the compare types
        *                defined in the H file as input arguments.
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void ShiftClock_PWM_SetCompareMode(uint8 comparemode) 
        {
            #if(ShiftClock_PWM_UsingFixedFunction)

                #if(0 != ShiftClock_PWM_CTRL_CMPMODE1_SHIFT)
                    uint8 comparemodemasked = ((uint8)((uint8)comparemode << ShiftClock_PWM_CTRL_CMPMODE1_SHIFT));
                #else
                    uint8 comparemodemasked = comparemode;
                #endif /* (0 != ShiftClock_PWM_CTRL_CMPMODE1_SHIFT) */

                ShiftClock_PWM_CONTROL3 &= ((uint8)(~ShiftClock_PWM_CTRL_CMPMODE1_MASK)); /*Clear Existing Data */
                ShiftClock_PWM_CONTROL3 |= comparemodemasked;

            #elif (ShiftClock_PWM_UseControl)

                #if(0 != ShiftClock_PWM_CTRL_CMPMODE1_SHIFT)
                    uint8 comparemode1masked = ((uint8)((uint8)comparemode << ShiftClock_PWM_CTRL_CMPMODE1_SHIFT)) &
                                                ShiftClock_PWM_CTRL_CMPMODE1_MASK;
                #else
                    uint8 comparemode1masked = comparemode & ShiftClock_PWM_CTRL_CMPMODE1_MASK;
                #endif /* (0 != ShiftClock_PWM_CTRL_CMPMODE1_SHIFT) */

                #if(0 != ShiftClock_PWM_CTRL_CMPMODE2_SHIFT)
                    uint8 comparemode2masked = ((uint8)((uint8)comparemode << ShiftClock_PWM_CTRL_CMPMODE2_SHIFT)) &
                                               ShiftClock_PWM_CTRL_CMPMODE2_MASK;
                #else
                    uint8 comparemode2masked = comparemode & ShiftClock_PWM_CTRL_CMPMODE2_MASK;
                #endif /* (0 != ShiftClock_PWM_CTRL_CMPMODE2_SHIFT) */

                /*Clear existing mode */
                ShiftClock_PWM_CONTROL &= ((uint8)(~(ShiftClock_PWM_CTRL_CMPMODE1_MASK |
                                            ShiftClock_PWM_CTRL_CMPMODE2_MASK)));
                ShiftClock_PWM_CONTROL |= (comparemode1masked | comparemode2masked);

            #else
                uint8 temp = comparemode;
            #endif /* (ShiftClock_PWM_UsingFixedFunction) */
        }
    #endif /* ShiftClock_PWM_CompareMode1SW */

#else /* UseOneCompareMode */

    #if (ShiftClock_PWM_CompareMode1SW)


        /*******************************************************************************
        * Function Name: ShiftClock_PWM_SetCompareMode1
        ********************************************************************************
        *
        * Summary:
        *  This function writes the Compare Mode for the pwm or pwm1 output
        *
        * Parameters:
        *  comparemode:  The new compare mode for the PWM output. Use the compare types
        *                defined in the H file as input arguments.
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void ShiftClock_PWM_SetCompareMode1(uint8 comparemode) 
        {
            #if(0 != ShiftClock_PWM_CTRL_CMPMODE1_SHIFT)
                uint8 comparemodemasked = ((uint8)((uint8)comparemode << ShiftClock_PWM_CTRL_CMPMODE1_SHIFT)) &
                                           ShiftClock_PWM_CTRL_CMPMODE1_MASK;
            #else
                uint8 comparemodemasked = comparemode & ShiftClock_PWM_CTRL_CMPMODE1_MASK;
            #endif /* (0 != ShiftClock_PWM_CTRL_CMPMODE1_SHIFT) */

            #if (ShiftClock_PWM_UseControl)
                ShiftClock_PWM_CONTROL &= ((uint8)(~ShiftClock_PWM_CTRL_CMPMODE1_MASK)); /*Clear existing mode */
                ShiftClock_PWM_CONTROL |= comparemodemasked;
            #endif /* (ShiftClock_PWM_UseControl) */
        }
    #endif /* ShiftClock_PWM_CompareMode1SW */

#if (ShiftClock_PWM_CompareMode2SW)


    /*******************************************************************************
    * Function Name: ShiftClock_PWM_SetCompareMode2
    ********************************************************************************
    *
    * Summary:
    *  This function writes the Compare Mode for the pwm or pwm2 output
    *
    * Parameters:
    *  comparemode:  The new compare mode for the PWM output. Use the compare types
    *                defined in the H file as input arguments.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void ShiftClock_PWM_SetCompareMode2(uint8 comparemode) 
    {

        #if(0 != ShiftClock_PWM_CTRL_CMPMODE2_SHIFT)
            uint8 comparemodemasked = ((uint8)((uint8)comparemode << ShiftClock_PWM_CTRL_CMPMODE2_SHIFT)) &
                                                 ShiftClock_PWM_CTRL_CMPMODE2_MASK;
        #else
            uint8 comparemodemasked = comparemode & ShiftClock_PWM_CTRL_CMPMODE2_MASK;
        #endif /* (0 != ShiftClock_PWM_CTRL_CMPMODE2_SHIFT) */

        #if (ShiftClock_PWM_UseControl)
            ShiftClock_PWM_CONTROL &= ((uint8)(~ShiftClock_PWM_CTRL_CMPMODE2_MASK)); /*Clear existing mode */
            ShiftClock_PWM_CONTROL |= comparemodemasked;
        #endif /* (ShiftClock_PWM_UseControl) */
    }
    #endif /*ShiftClock_PWM_CompareMode2SW */

#endif /* UseOneCompareMode */


#if (!ShiftClock_PWM_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: ShiftClock_PWM_WriteCounter
    ********************************************************************************
    *
    * Summary:
    *  Writes a new counter value directly to the counter register. This will be
    *  implemented for that currently running period and only that period. This API
    *  is valid only for UDB implementation and not available for fixed function
    *  PWM implementation.
    *
    * Parameters:
    *  counter:  The period new period counter value.
    *
    * Return:
    *  None
    *
    * Side Effects:
    *  The PWM Period will be reloaded when a counter value will be a zero
    *
    *******************************************************************************/
    void ShiftClock_PWM_WriteCounter(uint8 counter) \
                                       
    {
        CY_SET_REG8(ShiftClock_PWM_COUNTER_LSB_PTR, counter);
    }


    /*******************************************************************************
    * Function Name: ShiftClock_PWM_ReadCounter
    ********************************************************************************
    *
    * Summary:
    *  This function returns the current value of the counter.  It doesn't matter
    *  if the counter is enabled or running.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  The current value of the counter.
    *
    *******************************************************************************/
    uint8 ShiftClock_PWM_ReadCounter(void) 
    {
        /* Force capture by reading Accumulator */
        /* Must first do a software capture to be able to read the counter */
        /* It is up to the user code to make sure there isn't already captured data in the FIFO */
          (void)CY_GET_REG8(ShiftClock_PWM_COUNTERCAP_LSB_PTR_8BIT);

        /* Read the data from the FIFO */
        return (CY_GET_REG8(ShiftClock_PWM_CAPTURE_LSB_PTR));
    }

    #if (ShiftClock_PWM_UseStatus)


        /*******************************************************************************
        * Function Name: ShiftClock_PWM_ClearFIFO
        ********************************************************************************
        *
        * Summary:
        *  This function clears all capture data from the capture FIFO
        *
        * Parameters:
        *  None
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void ShiftClock_PWM_ClearFIFO(void) 
        {
            while(0u != (ShiftClock_PWM_ReadStatusRegister() & ShiftClock_PWM_STATUS_FIFONEMPTY))
            {
                (void)ShiftClock_PWM_ReadCapture();
            }
        }

    #endif /* ShiftClock_PWM_UseStatus */

#endif /* !ShiftClock_PWM_UsingFixedFunction */


/*******************************************************************************
* Function Name: ShiftClock_PWM_WritePeriod
********************************************************************************
*
* Summary:
*  This function is used to change the period of the counter.  The new period
*  will be loaded the next time terminal count is detected.
*
* Parameters:
*  period:  Period value. May be between 1 and (2^Resolution)-1.  A value of 0
*           will result in the counter remaining at zero.
*
* Return:
*  None
*
*******************************************************************************/
void ShiftClock_PWM_WritePeriod(uint8 period) 
{
    #if(ShiftClock_PWM_UsingFixedFunction)
        CY_SET_REG16(ShiftClock_PWM_PERIOD_LSB_PTR, (uint16)period);
    #else
        CY_SET_REG8(ShiftClock_PWM_PERIOD_LSB_PTR, period);
    #endif /* (ShiftClock_PWM_UsingFixedFunction) */
}

#if (ShiftClock_PWM_UseOneCompareMode)


    /*******************************************************************************
    * Function Name: ShiftClock_PWM_WriteCompare
    ********************************************************************************
    *
    * Summary:
    *  This funtion is used to change the compare1 value when the PWM is in Dither
    *  mode. The compare output will reflect the new value on the next UDB clock.
    *  The compare output will be driven high when the present counter value is
    *  compared to the compare value based on the compare mode defined in
    *  Dither Mode.
    *
    * Parameters:
    *  compare:  New compare value.
    *
    * Return:
    *  None
    *
    * Side Effects:
    *  This function is only available if the PWM mode parameter is set to
    *  Dither Mode, Center Aligned Mode or One Output Mode
    *
    *******************************************************************************/
    void ShiftClock_PWM_WriteCompare(uint8 compare) \
                                       
    {
        #if(ShiftClock_PWM_UsingFixedFunction)
            CY_SET_REG16(ShiftClock_PWM_COMPARE1_LSB_PTR, (uint16)compare);
        #else
            CY_SET_REG8(ShiftClock_PWM_COMPARE1_LSB_PTR, compare);
        #endif /* (ShiftClock_PWM_UsingFixedFunction) */

        #if (ShiftClock_PWM_PWMMode == ShiftClock_PWM__B_PWM__DITHER)
            #if(ShiftClock_PWM_UsingFixedFunction)
                CY_SET_REG16(ShiftClock_PWM_COMPARE2_LSB_PTR, (uint16)(compare + 1u));
            #else
                CY_SET_REG8(ShiftClock_PWM_COMPARE2_LSB_PTR, (compare + 1u));
            #endif /* (ShiftClock_PWM_UsingFixedFunction) */
        #endif /* (ShiftClock_PWM_PWMMode == ShiftClock_PWM__B_PWM__DITHER) */
    }


#else


    /*******************************************************************************
    * Function Name: ShiftClock_PWM_WriteCompare1
    ********************************************************************************
    *
    * Summary:
    *  This funtion is used to change the compare1 value.  The compare output will
    *  reflect the new value on the next UDB clock.  The compare output will be
    *  driven high when the present counter value is less than or less than or
    *  equal to the compare register, depending on the mode.
    *
    * Parameters:
    *  compare:  New compare value.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void ShiftClock_PWM_WriteCompare1(uint8 compare) \
                                        
    {
        #if(ShiftClock_PWM_UsingFixedFunction)
            CY_SET_REG16(ShiftClock_PWM_COMPARE1_LSB_PTR, (uint16)compare);
        #else
            CY_SET_REG8(ShiftClock_PWM_COMPARE1_LSB_PTR, compare);
        #endif /* (ShiftClock_PWM_UsingFixedFunction) */
    }


    /*******************************************************************************
    * Function Name: ShiftClock_PWM_WriteCompare2
    ********************************************************************************
    *
    * Summary:
    *  This funtion is used to change the compare value, for compare1 output.
    *  The compare output will reflect the new value on the next UDB clock.
    *  The compare output will be driven high when the present counter value is
    *  less than or less than or equal to the compare register, depending on the
    *  mode.
    *
    * Parameters:
    *  compare:  New compare value.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void ShiftClock_PWM_WriteCompare2(uint8 compare) \
                                        
    {
        #if(ShiftClock_PWM_UsingFixedFunction)
            CY_SET_REG16(ShiftClock_PWM_COMPARE2_LSB_PTR, compare);
        #else
            CY_SET_REG8(ShiftClock_PWM_COMPARE2_LSB_PTR, compare);
        #endif /* (ShiftClock_PWM_UsingFixedFunction) */
    }
#endif /* UseOneCompareMode */

#if (ShiftClock_PWM_DeadBandUsed)


    /*******************************************************************************
    * Function Name: ShiftClock_PWM_WriteDeadTime
    ********************************************************************************
    *
    * Summary:
    *  This function writes the dead-band counts to the corresponding register
    *
    * Parameters:
    *  deadtime:  Number of counts for dead time
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void ShiftClock_PWM_WriteDeadTime(uint8 deadtime) 
    {
        /* If using the Dead Band 1-255 mode then just write the register */
        #if(!ShiftClock_PWM_DeadBand2_4)
            CY_SET_REG8(ShiftClock_PWM_DEADBAND_COUNT_PTR, deadtime);
        #else
            /* Otherwise the data has to be masked and offset */
            /* Clear existing data */
            ShiftClock_PWM_DEADBAND_COUNT &= ((uint8)(~ShiftClock_PWM_DEADBAND_COUNT_MASK));

            /* Set new dead time */
            #if(ShiftClock_PWM_DEADBAND_COUNT_SHIFT)
                ShiftClock_PWM_DEADBAND_COUNT |= ((uint8)((uint8)deadtime << ShiftClock_PWM_DEADBAND_COUNT_SHIFT)) &
                                                    ShiftClock_PWM_DEADBAND_COUNT_MASK;
            #else
                ShiftClock_PWM_DEADBAND_COUNT |= deadtime & ShiftClock_PWM_DEADBAND_COUNT_MASK;
            #endif /* (ShiftClock_PWM_DEADBAND_COUNT_SHIFT) */

        #endif /* (!ShiftClock_PWM_DeadBand2_4) */
    }


    /*******************************************************************************
    * Function Name: ShiftClock_PWM_ReadDeadTime
    ********************************************************************************
    *
    * Summary:
    *  This function reads the dead-band counts from the corresponding register
    *
    * Parameters:
    *  None
    *
    * Return:
    *  Dead Band Counts
    *
    *******************************************************************************/
    uint8 ShiftClock_PWM_ReadDeadTime(void) 
    {
        /* If using the Dead Band 1-255 mode then just read the register */
        #if(!ShiftClock_PWM_DeadBand2_4)
            return (CY_GET_REG8(ShiftClock_PWM_DEADBAND_COUNT_PTR));
        #else

            /* Otherwise the data has to be masked and offset */
            #if(ShiftClock_PWM_DEADBAND_COUNT_SHIFT)
                return ((uint8)(((uint8)(ShiftClock_PWM_DEADBAND_COUNT & ShiftClock_PWM_DEADBAND_COUNT_MASK)) >>
                                                                           ShiftClock_PWM_DEADBAND_COUNT_SHIFT));
            #else
                return (ShiftClock_PWM_DEADBAND_COUNT & ShiftClock_PWM_DEADBAND_COUNT_MASK);
            #endif /* (ShiftClock_PWM_DEADBAND_COUNT_SHIFT) */
        #endif /* (!ShiftClock_PWM_DeadBand2_4) */
    }
#endif /* DeadBandUsed */

#if (ShiftClock_PWM_UseStatus || ShiftClock_PWM_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: ShiftClock_PWM_SetInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  This function configures the interrupts mask control of theinterrupt
    *  source status register.
    *
    * Parameters:
    *  uint8 interruptMode: Bit field containing the interrupt sources enabled
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void ShiftClock_PWM_SetInterruptMode(uint8 interruptMode) 
    {
        CY_SET_REG8(ShiftClock_PWM_STATUS_MASK_PTR, interruptMode);
    }


    /*******************************************************************************
    * Function Name: ShiftClock_PWM_ReadStatusRegister
    ********************************************************************************
    *
    * Summary:
    *  This function returns the current state of the status register.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8 : Current status register value. The status register bits are:
    *  [7:6] : Unused(0)
    *  [5]   : Kill event output
    *  [4]   : FIFO not empty
    *  [3]   : FIFO full
    *  [2]   : Terminal count
    *  [1]   : Compare output 2
    *  [0]   : Compare output 1
    *
    *******************************************************************************/
    uint8 ShiftClock_PWM_ReadStatusRegister(void) 
    {
        return (CY_GET_REG8(ShiftClock_PWM_STATUS_PTR));
    }

#endif /* (ShiftClock_PWM_UseStatus || ShiftClock_PWM_UsingFixedFunction) */


#if (ShiftClock_PWM_UseControl)


    /*******************************************************************************
    * Function Name: ShiftClock_PWM_ReadControlRegister
    ********************************************************************************
    *
    * Summary:
    *  Returns the current state of the control register. This API is available
    *  only if the control register is not removed.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8 : Current control register value
    *
    *******************************************************************************/
    uint8 ShiftClock_PWM_ReadControlRegister(void) 
    {
        uint8 result;

        result = CY_GET_REG8(ShiftClock_PWM_CONTROL_PTR);
        return (result);
    }


    /*******************************************************************************
    * Function Name: ShiftClock_PWM_WriteControlRegister
    ********************************************************************************
    *
    * Summary:
    *  Sets the bit field of the control register. This API is available only if
    *  the control register is not removed.
    *
    * Parameters:
    *  uint8 control: Control register bit field, The status register bits are:
    *  [7]   : PWM Enable
    *  [6]   : Reset
    *  [5:3] : Compare Mode2
    *  [2:0] : Compare Mode2
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void ShiftClock_PWM_WriteControlRegister(uint8 control) 
    {
        CY_SET_REG8(ShiftClock_PWM_CONTROL_PTR, control);
    }

#endif /* (ShiftClock_PWM_UseControl) */


#if (!ShiftClock_PWM_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: ShiftClock_PWM_ReadCapture
    ********************************************************************************
    *
    * Summary:
    *  Reads the capture value from the capture FIFO.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: The current capture value
    *
    *******************************************************************************/
    uint8 ShiftClock_PWM_ReadCapture(void) 
    {
        return (CY_GET_REG8(ShiftClock_PWM_CAPTURE_LSB_PTR));
    }

#endif /* (!ShiftClock_PWM_UsingFixedFunction) */


#if (ShiftClock_PWM_UseOneCompareMode)


    /*******************************************************************************
    * Function Name: ShiftClock_PWM_ReadCompare
    ********************************************************************************
    *
    * Summary:
    *  Reads the compare value for the compare output when the PWM Mode parameter is
    *  set to Dither mode, Center Aligned mode, or One Output mode.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: Current compare value
    *
    *******************************************************************************/
    uint8 ShiftClock_PWM_ReadCompare(void) 
    {
        #if(ShiftClock_PWM_UsingFixedFunction)
            return ((uint8)CY_GET_REG16(ShiftClock_PWM_COMPARE1_LSB_PTR));
        #else
            return (CY_GET_REG8(ShiftClock_PWM_COMPARE1_LSB_PTR));
        #endif /* (ShiftClock_PWM_UsingFixedFunction) */
    }

#else


    /*******************************************************************************
    * Function Name: ShiftClock_PWM_ReadCompare1
    ********************************************************************************
    *
    * Summary:
    *  Reads the compare value for the compare1 output.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: Current compare value.
    *
    *******************************************************************************/
    uint8 ShiftClock_PWM_ReadCompare1(void) 
    {
        return (CY_GET_REG8(ShiftClock_PWM_COMPARE1_LSB_PTR));
    }


    /*******************************************************************************
    * Function Name: ShiftClock_PWM_ReadCompare2
    ********************************************************************************
    *
    * Summary:
    *  Reads the compare value for the compare2 output.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: Current compare value.
    *
    *******************************************************************************/
    uint8 ShiftClock_PWM_ReadCompare2(void) 
    {
        return (CY_GET_REG8(ShiftClock_PWM_COMPARE2_LSB_PTR));
    }

#endif /* (ShiftClock_PWM_UseOneCompareMode) */


/*******************************************************************************
* Function Name: ShiftClock_PWM_ReadPeriod
********************************************************************************
*
* Summary:
*  Reads the period value used by the PWM hardware.
*
* Parameters:
*  None
*
* Return:
*  uint8/16: Period value
*
*******************************************************************************/
uint8 ShiftClock_PWM_ReadPeriod(void) 
{
    #if(ShiftClock_PWM_UsingFixedFunction)
        return ((uint8)CY_GET_REG16(ShiftClock_PWM_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG8(ShiftClock_PWM_PERIOD_LSB_PTR));
    #endif /* (ShiftClock_PWM_UsingFixedFunction) */
}

#if ( ShiftClock_PWM_KillModeMinTime)


    /*******************************************************************************
    * Function Name: ShiftClock_PWM_WriteKillTime
    ********************************************************************************
    *
    * Summary:
    *  Writes the kill time value used by the hardware when the Kill Mode
    *  is set to Minimum Time.
    *
    * Parameters:
    *  uint8: Minimum Time kill counts
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void ShiftClock_PWM_WriteKillTime(uint8 killtime) 
    {
        CY_SET_REG8(ShiftClock_PWM_KILLMODEMINTIME_PTR, killtime);
    }


    /*******************************************************************************
    * Function Name: ShiftClock_PWM_ReadKillTime
    ********************************************************************************
    *
    * Summary:
    *  Reads the kill time value used by the hardware when the Kill Mode is set
    *  to Minimum Time.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8: The current Minimum Time kill counts
    *
    *******************************************************************************/
    uint8 ShiftClock_PWM_ReadKillTime(void) 
    {
        return (CY_GET_REG8(ShiftClock_PWM_KILLMODEMINTIME_PTR));
    }

#endif /* ( ShiftClock_PWM_KillModeMinTime) */

/* [] END OF FILE */
