/*******************************************************************************
* File Name: STCP_Pin.h  
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

#if !defined(CY_PINS_STCP_Pin_ALIASES_H) /* Pins STCP_Pin_ALIASES_H */
#define CY_PINS_STCP_Pin_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define STCP_Pin_0			(STCP_Pin__0__PC)
#define STCP_Pin_0_PS		(STCP_Pin__0__PS)
#define STCP_Pin_0_PC		(STCP_Pin__0__PC)
#define STCP_Pin_0_DR		(STCP_Pin__0__DR)
#define STCP_Pin_0_SHIFT	(STCP_Pin__0__SHIFT)
#define STCP_Pin_0_INTR	((uint16)((uint16)0x0003u << (STCP_Pin__0__SHIFT*2u)))

#define STCP_Pin_INTR_ALL	 ((uint16)(STCP_Pin_0_INTR))


#endif /* End Pins STCP_Pin_ALIASES_H */


/* [] END OF FILE */
