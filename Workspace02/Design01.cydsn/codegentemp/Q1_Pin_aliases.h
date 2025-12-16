/*******************************************************************************
* File Name: Q1_Pin.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Q1_Pin_ALIASES_H) /* Pins Q1_Pin_ALIASES_H */
#define CY_PINS_Q1_Pin_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define Q1_Pin_0			(Q1_Pin__0__PC)
#define Q1_Pin_0_PS		(Q1_Pin__0__PS)
#define Q1_Pin_0_PC		(Q1_Pin__0__PC)
#define Q1_Pin_0_DR		(Q1_Pin__0__DR)
#define Q1_Pin_0_SHIFT	(Q1_Pin__0__SHIFT)
#define Q1_Pin_0_INTR	((uint16)((uint16)0x0003u << (Q1_Pin__0__SHIFT*2u)))

#define Q1_Pin_INTR_ALL	 ((uint16)(Q1_Pin_0_INTR))


#endif /* End Pins Q1_Pin_ALIASES_H */


/* [] END OF FILE */
