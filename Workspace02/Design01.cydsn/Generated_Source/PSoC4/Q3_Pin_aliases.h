/*******************************************************************************
* File Name: Q3_Pin.h  
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

#if !defined(CY_PINS_Q3_Pin_ALIASES_H) /* Pins Q3_Pin_ALIASES_H */
#define CY_PINS_Q3_Pin_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define Q3_Pin_0			(Q3_Pin__0__PC)
#define Q3_Pin_0_PS		(Q3_Pin__0__PS)
#define Q3_Pin_0_PC		(Q3_Pin__0__PC)
#define Q3_Pin_0_DR		(Q3_Pin__0__DR)
#define Q3_Pin_0_SHIFT	(Q3_Pin__0__SHIFT)
#define Q3_Pin_0_INTR	((uint16)((uint16)0x0003u << (Q3_Pin__0__SHIFT*2u)))

#define Q3_Pin_INTR_ALL	 ((uint16)(Q3_Pin_0_INTR))


#endif /* End Pins Q3_Pin_ALIASES_H */


/* [] END OF FILE */
