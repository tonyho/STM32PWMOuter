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
File        : Dialog.h
Purpose     : Dialog box include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef DIALOG_H
#define DIALOG_H

#include "WM.h"
#include "BUTTON.h"
#include "CHECKBOX.h"
#include "DROPDOWN.h"
#include "EDIT.h"
#include "FRAMEWIN.h"
#include "GRAPH.h"
#include "HEADER.h"
#include "ICONVIEW.h"
#include "LISTBOX.h"
#include "LISTVIEW.h"
#include "LISTWHEEL.h"
#include "MENU.h"
#include "MULTIEDIT.h"
#include "PROGBAR.h"
#include "RADIO.h"
#include "SCROLLBAR.h"
#include "SLIDER.h"
#include "TEXT.h"
#include "TREEVIEW.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

WM_HWIN WINDOW_CreateEx         (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, WM_CALLBACK * cb);
WM_HWIN WINDOW_CreateUser       (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, WM_CALLBACK * cb, int NumExtraBytes);
WM_HWIN WINDOW_CreateIndirect   (const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);
int     WINDOW_GetUserData      (WM_HWIN hObj, void * pDest, int NumBytes);
void    WINDOW_SetBkColor       (WM_HWIN hObj, GUI_COLOR Color);
void    WINDOW_SetDefaultBkColor(GUI_COLOR Color);
int     WINDOW_SetUserData      (WM_HWIN hObj, const void * pSrc, int NumBytes);

GUI_COLOR WINDOW_GetDefaultBkColor(void);

void WINDOW_Callback(WM_MESSAGE * pMsg);

#if defined(__cplusplus)
  }
#endif

#endif

#endif

