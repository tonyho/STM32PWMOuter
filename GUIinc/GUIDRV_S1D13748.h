/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2011  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.12 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  ARM LIMITED whose registered office
is situated at  110 Fulbourn Road,  Cambridge CB1 9NJ,  England solely
for  the  purposes  of  creating  libraries  for  ARM7, ARM9, Cortex-M
series,  and   Cortex-R4   processor-based  devices,  sublicensed  and
distributed as part of the  MDK-ARM  Professional  under the terms and
conditions  of  the   End  User  License  supplied  with  the  MDK-ARM
Professional. 
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIDRV_S1D13748.h
Purpose     : Interface definition for GUIDRV_S1D13748 driver
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUIDRV_S1D13748_H
#define GUIDRV_S1D13748_H

#define GUIDRV_S1D13748_USE_MAIN 0
#define GUIDRV_S1D13748_USE_PIP1 1
#define GUIDRV_S1D13748_USE_PIP2 2

/*********************************************************************
*
*       Configuration structure
*/
typedef struct {
  //
  // Driver specific configuration items
  //
  U32 BufferOffset;
  int UseLayer;
  int WriteBufferSize;
} CONFIG_S1D13748;

/*********************************************************************
*
*       Display drivers
*/
//
// Addresses
//
extern const GUI_DEVICE_API GUIDRV_S1D13748_16_API;
extern const GUI_DEVICE_API GUIDRV_S1D13748_OY_16_API;
extern const GUI_DEVICE_API GUIDRV_S1D13748_OX_16_API;
extern const GUI_DEVICE_API GUIDRV_S1D13748_OXY_16_API;
extern const GUI_DEVICE_API GUIDRV_S1D13748_OS_16_API;
extern const GUI_DEVICE_API GUIDRV_S1D13748_OSY_16_API;
extern const GUI_DEVICE_API GUIDRV_S1D13748_OSX_16_API;
extern const GUI_DEVICE_API GUIDRV_S1D13748_OSXY_16_API;

//
// Macros to be used in configuration files
//
#if defined(WIN32) && !defined(LCD_SIMCONTROLLER)

  #define GUIDRV_S1D13748_16       &GUIDRV_Win_API
  #define GUIDRV_S1D13748_OY_16    &GUIDRV_Win_API
  #define GUIDRV_S1D13748_OX_16    &GUIDRV_Win_API
  #define GUIDRV_S1D13748_OXY_16   &GUIDRV_Win_API
  #define GUIDRV_S1D13748_OS_16    &GUIDRV_Win_API
  #define GUIDRV_S1D13748_OSY_16   &GUIDRV_Win_API
  #define GUIDRV_S1D13748_OSX_16   &GUIDRV_Win_API
  #define GUIDRV_S1D13748_OSXY_16  &GUIDRV_Win_API

#else

  #define GUIDRV_S1D13748_16       &GUIDRV_S1D13748_16_API
  #define GUIDRV_S1D13748_OY_16    &GUIDRV_S1D13748_OY_16_API
  #define GUIDRV_S1D13748_OX_16    &GUIDRV_S1D13748_OX_16_API
  #define GUIDRV_S1D13748_OXY_16   &GUIDRV_S1D13748_OXY_16_API
  #define GUIDRV_S1D13748_OS_16    &GUIDRV_S1D13748_OS_16_API
  #define GUIDRV_S1D13748_OSY_16   &GUIDRV_S1D13748_OSY_16_API
  #define GUIDRV_S1D13748_OSX_16   &GUIDRV_S1D13748_OSX_16_API
  #define GUIDRV_S1D13748_OSXY_16  &GUIDRV_S1D13748_OSXY_16_API

#endif

/*********************************************************************
*
*       Public routines
*/
void GUIDRV_S1D13748_Config  (GUI_DEVICE * pDevice, CONFIG_S1D13748 * pConfig);
void GUIDRV_S1D13748_SetBus16(GUI_DEVICE * pDevice, GUI_PORT_API * pHW_API);

#endif

/*************************** End of file ****************************/
