/****************************************************************************
* Copyright (C), 2011 �ܶ�Ƕ��ʽ������ www.ourstm.net
*
* �������� �ܶ���STM32������V2,2.1,V3,MINI�ϵ���ͨ��           
* QQ: 9191274, ������sun68, Email: sun68@163.com 
* �Ա����̣�ourstm.taobao.com  
*
* �ļ���: app.c
* ���ݼ���:
*       �����̲���ϵͳ����ucos2.86a�汾�� ������4������
			������											 ���ȼ�
			APP_TASK_START_PRIO                               11	        ������
			APP_TASK_USER_IF_PRIO                             13		ucgui��������
            APP_TASK_KBD_PRIO                                 12		����������
            Task_NMEA_PRIO                                    10		GPS NMEA��������
		 ��Ȼ��������ϵͳ����
		    OS_TaskIdle                  ��������-----------------���ȼ����
			OS_TaskStat                  ͳ������ʱ�������-------���ȼ��ε�
*
* �ļ���ʷ:
* �汾��  ����       ����    ˵��
* v0.2    2011-7-06 sun68  �������ļ�
*
*/

#define GLOBALS
#include "includes.h"
#include "demo.h"

extern void GUIDEMO_main(void);	

//============var
const int ID_CURVE_BTN_SWTITCH_2_PARA = 0x200; 
const int ID_CURVE_BTN_CLEAR = 0x201;

const int ID_CFG_PID_BTN_SWTITCH_2_CURVE = 0x202;
const int ID_CFG_PID_BTN_CLEAR = 0x203;

int ClosePWM = 0;

/*********************************************************************
*
*       Dialog resource
*
* This table conatins the info required to create the dialog.
* It has been created by ucGUIbuilder.
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { FRAMEWIN_CreateIndirect,  "PWM Configeration", 0,                       0,  0,  320,240,0,0},
    { BUTTON_CreateIndirect,    "0",                 GUI_ID_BUTTON0,          10, 76, 75, 31, 0,0},
    { BUTTON_CreateIndirect,    "1",                 GUI_ID_BUTTON1,          91, 76, 72, 31, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT0,            10, 41, 75, 29, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT1,            91, 41, 72, 29, 0,0},
    { BUTTON_CreateIndirect,    "2",                 GUI_ID_BUTTON2,          172,76, 75, 31, 0,0},
    { BUTTON_CreateIndirect,    "3",                 GUI_ID_BUTTON3,          10, 113,75, 31, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT2,            172,41, 75, 29, 0,0},
//    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT3,            253,41, 55, 29, 0,0},
    { BUTTON_CreateIndirect,    "4",                 GUI_ID_BUTTON4,          91, 113,72, 31, 0,0},
    { BUTTON_CreateIndirect,    "5",                 GUI_ID_BUTTON5,          172,113,75, 31, 0,0},
    { BUTTON_CreateIndirect,    "6",                 GUI_ID_BUTTON6,          10, 150,75, 31, 0,0},
    { BUTTON_CreateIndirect,    "7",                 GUI_ID_BUTTON7,          91, 148,72, 31, 0,0},
    { BUTTON_CreateIndirect,    "8",                 GUI_ID_BUTTON8,          172,150,75, 31, 0,0},
    { BUTTON_CreateIndirect,    "9",                 GUI_ID_BUTTON9,          10, 185,75, 31, 0,0},
    { BUTTON_CreateIndirect,    ".",                 GUI_ID_BUTTON10,          91, 185,72, 31, 0,0},
    { BUTTON_CreateIndirect,    "<-",                GUI_ID_BUTTON11,          172,187,75, 31, 0,0},
    { BUTTON_CreateIndirect,    "Ch1",                GUI_ID_BUTTON12,          10, 11, 75, 23, 0,0},
    { BUTTON_CreateIndirect,    "Ch2",                GUI_ID_BUTTON13,          91, 11, 72, 23, 0,0},
    { BUTTON_CreateIndirect,    "Ch3",                GUI_ID_BUTTON14,          172,11, 75, 23, 0,0},
//    { BUTTON_CreateIndirect,    "E",                 GUI_ID_BUTTON15,          253,11, 55, 23, 0,0},
    { BUTTON_CreateIndirect,    "OK",                GUI_ID_BUTTON16,         316,187,75, 34, 0,0},
    { SLIDER_CreateIndirect,     NULL,               GUI_ID_SLIDER0,          254,76, 40, 31, 0,0},
    { BUTTON_CreateIndirect,    "Set",               GUI_ID_BUTTON17,         314,12, 75, 23, 0,0},
  //  { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT4,            316,41, 69, 29, 0,0},
  //  { BUTTON_CreateIndirect,    "Next",          	 GUI_ID_BUTTON18,         316,113,75, 31, 0,0},
  //  { BUTTON_CreateIndirect,    "Prev",          	 GUI_ID_BUTTON19,         316,148,75, 31, 0,0},
  //  { TEXT_CreateIndirect,      "0-Order",           GUI_ID_TEXT0,            258,85, 35, 12, 0,0}
};


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_STK App_TaskStartStk[APP_TASK_START_STK_SIZE];
static  OS_STK AppTaskUserIFStk[APP_TASK_USER_IF_STK_SIZE];
static  OS_STK AppTaskKbdStk[APP_TASK_KBD_STK_SIZE];

/*
Controlers
*/
WM_HWIN  hWM_HBKWIN_CURVE,hWN_PWM_CFG_Frame;
BUTTON_Handle btn,btnClear,
              btn1,btnClear1;
EDIT_Handle PWM_EDIT = GUI_ID_EDIT0;


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void App_TaskCreate(void);

static  void App_TaskStart(void* p_arg);
static  void  AppTaskUserIF (void *p_arg);

static  void AppTaskKbd(void* p_arg);

void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,uint16_t Len);


/****************************************************************************
* ��    �ƣ�int main(void)
* ��    �ܣ����������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
int main(void)
{
   CPU_INT08U os_err; 
   /* ��ֹ�����ж� */
   CPU_IntDis();
   
   /* ucosII ��ʼ�� */
   OSInit();                                                  

   /* Ӳ��ƽ̨��ʼ�� */
   BSP_Init();                               
   
   //���������� ���ȼ����  ���������������һ����;��Ϊ���Ժ�ʹ��ͳ������
   os_err = OSTaskCreate((void (*) (void *)) App_TaskStart,               		    //ָ����������ָ��
                          (void *) 0,												//����ʼִ��ʱ�����ݸ�����Ĳ�����ָ��
               		     (OS_STK *) &App_TaskStartStk[APP_TASK_START_STK_SIZE - 1],	//���������Ķ�ջ��ջ��ָ��   �Ӷ����µݼ�
                         (INT8U) APP_TASK_START_PRIO);								//�������������ȼ�
             
   OSTimeSet(0);			 //ucosII�Ľ��ļ�������0    ���ļ�������0-4294967295  
   OSStart();                //����ucosII�ں�   
   return (0);
}




/****************************************************************************
* ��    �ƣ�static  void App_TaskStart(void* p_arg)
* ��    �ܣ���ʼ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
static  void App_TaskStart(void* p_arg)
{
 
  (void) p_arg;
   //��ʼ��ucosIIʱ�ӽ���
   OS_CPU_SysTickInit();
                               
   //ʹ��ucos ��ͳ������
#if (OS_TASK_STAT_EN > 0)
   
   OSStatInit();                //----ͳ�������ʼ������                                 
#endif

   App_TaskCreate();			//��������������

   while (1)
   {  
      LED_LED1_ON();		      	  
	  OSTimeDlyHMSM(0, 0, 0, 100);

	  LED_LED1_OFF();
	  OSTimeDlyHMSM(0, 0, 0, 100);
   }
}

/****************************************************************************
* ��    �ƣ�static  void App_TaskCreate(void)
* ��    �ܣ�������������ĺ���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
static  void App_TaskCreate(void)
{
  
   NMEA_MBOX=OSSemCreate(1);		                                           //����NMEAָ��������ź���
   /*  �����û��������� */
   OSTaskCreateExt(AppTaskUserIF,											   //ָ����������ָ��
   					(void *)0,												   //����ʼִ��ʱ�����ݸ�����Ĳ�����ָ��
   					(OS_STK *)&AppTaskUserIFStk[APP_TASK_USER_IF_STK_SIZE-1],  //���������Ķ�ջ��ջ��ָ��   �Ӷ����µݼ�
					APP_TASK_USER_IF_PRIO,									   //�������������ȼ�
					APP_TASK_USER_IF_PRIO,									   //Ԥ�����Ժ�汾�������ʶ���������а汾ͬ�������ȼ�
					(OS_STK *)&AppTaskUserIFStk[0],							   //ָ�������ջջ�׵�ָ�룬���ڶ�ջ�ļ���
                    APP_TASK_USER_IF_STK_SIZE,									//ָ����ջ�����������ڶ�ջ�ļ���
                    (void *)0,													//ָ���û����ӵ��������ָ�룬������չ�����������ƿ�
                    OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);					//ѡ�ָ���Ƿ������ջ���飬�Ƿ񽫶�ջ��0,�����Ƿ�Ҫ
					                                                            //���и�������ȵȡ�
                    
   /*   ���������������� */
   OSTaskCreateExt(AppTaskKbd,
   					(void *)0,
					(OS_STK *)&AppTaskKbdStk[APP_TASK_KBD_STK_SIZE-1],
					APP_TASK_KBD_PRIO,
					APP_TASK_KBD_PRIO,
					(OS_STK *)&AppTaskKbdStk[0],
                    APP_TASK_KBD_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);	
}

/*****************************************************************
**      FunctionName:void InitDialog(WM_MESSAGE * pMsg)
**      Function: to initialize the Dialog items
**                                                      
**      call this function in _cbCallback --> WM_INIT_DIALOG
*****************************************************************/

void InitDialog(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;

    FRAMEWIN_SetTextAlign(hWin,GUI_TA_VCENTER|GUI_TA_CENTER);
	
    FRAMEWIN_AddCloseButton(hWin, FRAMEWIN_BUTTON_RIGHT, 0);
    FRAMEWIN_AddMaxButton(hWin, FRAMEWIN_BUTTON_RIGHT, 1);
    FRAMEWIN_AddMinButton(hWin, FRAMEWIN_BUTTON_RIGHT, 2);
	FRAMEWIN_SetTitleHeight(hWin,18);

	SLIDER_SetRange(WM_GetDialogItem(hWin,GUI_ID_SLIDER0),5,250);
	SLIDER_SetValue(WM_GetDialogItem(hWin,GUI_ID_SLIDER0),200);

	EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT0),GUI_TA_VCENTER|GUI_TA_CENTER);
	EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT1),GUI_TA_VCENTER|GUI_TA_CENTER);
	EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT2),GUI_TA_VCENTER|GUI_TA_CENTER);
	EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT3),GUI_TA_VCENTER|GUI_TA_CENTER);
	EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT4),GUI_TA_VCENTER|GUI_TA_CENTER);	

}


/*****************************************************************
**      FunctionName:void PaintDialog(WM_MESSAGE * pMsg)
**      Function: to initialize the Dialog items
**                                                      
**      call this function in _cbCallback --> WM_PAINT
*****************************************************************/

void PaintDialog(WM_MESSAGE * pMsg)
{
}


/*********************************************************************
*
*       Dialog callback routine
*/
static void _cbBk_PWM_CFG(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
        case WM_PAINT:
            PaintDialog(pMsg);
			GUI_SetBkColor(GUI_BLUE);
	 		GUI_Clear();
            break;
        case WM_INIT_DIALOG:
            InitDialog(pMsg);
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_ESCAPE:
                    GUI_EndDialog(hWin, 1);
                    break;
                case GUI_KEY_ENTER:
                    GUI_EndDialog(hWin, 0);
                    break;						
            }
            break;
        case WM_NOTIFY_PARENT:
				GUI_DispStringAt("Frame",10,220);
            	Id = WM_GetId(pMsg->hWinSrc); 
            	NCode = pMsg->Data.v;            
				switch(NCode)
				{
					 case WM_NOTIFICATION_RELEASED: 
					 {																			
						if(Id >= GUI_ID_BUTTON0 && Id <= GUI_ID_BUTTON9){
								EDIT_AddKey(WM_GetDialogItem(hWN_PWM_CFG_Frame,PWM_EDIT),'0'+Id-GUI_ID_BUTTON0);
						}
																	
						if(Id == GUI_ID_BUTTON10){
								EDIT_AddKey(WM_GetDialogItem(hWN_PWM_CFG_Frame,PWM_EDIT),'.');										
						}
						
						/* BackSpace */
						if(Id == GUI_ID_BUTTON11)
						{
							 //EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT0),NULL);//�������򣬰���ɾ������������
							 WM_SetFocus(WM_GetDialogItem(hWN_PWM_CFG_Frame,PWM_EDIT));
							 GUI_SendKeyMsg(GUI_KEY_BACKSPACE,1);  
						}
						
						/* Up and down of the order of the PID Array */

						if(Id == GUI_ID_SLIDER0)
						{	
							#if 0
									BK_Light = SLIDER_GetValue(WM_GetDialogItem(hWN_PWM_CFG_Frame,GUI_ID_SLIDER0));
									SetPWM(BK_Light,PWM_BACK_LIGHT);																									
							#endif
						}
				
					 }
						break;				
				}														
        default:
            WM_DefaultProc(pMsg);
    }
}

static int _FRAMEWIN_DrawSkinFlex(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo) {
  switch (pDrawItemInfo->Cmd) {
  case WIDGET_ITEM_CREATE:
    FRAMEWIN_SetTextAlign(pDrawItemInfo->hWin, GUI_TA_HCENTER | GUI_TA_VCENTER);
    FRAMEWIN_SetTextColor(pDrawItemInfo->hWin, GUI_BLACK);
    break;
  default:
    return FRAMEWIN_DrawSkinFlex(pDrawItemInfo);
  }
  return 0;
}



/*********************************************************************
*
*       _cbBk
*/
static void _cbBk(WM_MESSAGE * pMsg) 
{
	static unsigned int GirdFlag = 0;
	int NCode, Id;
  switch (pMsg->MsgId) {
  case WM_PAINT:
		GUI_SetBkColor(GUI_LIGHTRED);
	  	GUI_Clear();
    	GUI_DispStringAt("PWM Output", 100, 10);
   		//GUIDEMO_DrawBk(1);
    break;
	case WM_TOUCH:
		GirdFlag++;
		//GRAPH_SetGridVis(hGraph,GirdFlag%3);  //!< Display the  if touch the screen
		break;

	/*******************/
	case WM_NOTIFY_PARENT:
      Id    = WM_GetId(pMsg->hWinSrc);    /* Id of widget */
      NCode = pMsg->Data.v;               /* Notification code */
      switch (NCode) {
        case WM_NOTIFICATION_RELEASED:    /* React only if released */
        	if (Id == ID_CURVE_BTN_SWTITCH_2_PARA)  /* ID =210 btn Button */  
			{                   	
				ClosePWM = ! ClosePWM;
				BUTTON_SetText(btn,"Closed");
				WM_ShowWindow(hWN_PWM_CFG_Frame);
				WM_HideWindow(hWM_HBKWIN_CURVE);
          	} 	
			if (Id == 202)  /* ID =210 btn Button */  
			{                   	
				BUTTON_SetText(btnClear,"PID_Set");
				WM_HideWindow(hWM_HBKWIN_CURVE);
			 } 
			break;      
			
			default:          
					{
					}
      }
	break;
	
  default:
    WM_DefaultProc(pMsg);
  }
}


/****************************************************************************
* ��    �ƣ�static  void  AppTaskUserIF (void *p_arg)
* ��    �ܣ��û���������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
static  void  AppTaskUserIF (void *p_arg)
{				
	FRAMEWIN_SKINFLEX_PROPS Framewin_Props;

	(void)p_arg;								    
	GUI_Init();	                             //emWIN��ʼ�� 
	WM_SetDesktopColor(GUI_YELLOW);

	GUI_Clear();

	WM_SetCreateFlags(WM_CF_MEMDEV);
	//GUIDEMO_Main();						 //����������

	/* The First window to display the Temperature Curve */
	//hWM_HBKWIN_CURVE =WM_GetDesktopWindow();
	hWM_HBKWIN_CURVE=WM_CreateWindow(0,0,320,240,WM_CF_SHOW | WM_CF_MEMDEV ,0,0);
	WM_SetCallback(hWM_HBKWIN_CURVE, _cbBk);

	//WM_SelectWindow(hWM_HBKWIN_CURVE);		
	

	//GUI_SetBkColor(GUI_BLUE);
#if (GUI_SUPPORT_CURSOR | GUI_SUPPORT_TOUCH)
	GUI_CURSOR_Show();
#endif
#if 1 
	BUTTON_SetReactOnLevel();
	FRAMEWIN_GetSkinFlexProps(&Framewin_Props, FRAMEWIN_SKINFLEX_PI_ACTIVE);
	Framewin_Props.Radius = 0;
	FRAMEWIN_SetSkinFlexProps(&Framewin_Props, FRAMEWIN_SKINFLEX_PI_ACTIVE);
	FRAMEWIN_GetSkinFlexProps(&Framewin_Props, FRAMEWIN_SKINFLEX_PI_INACTIVE);
	Framewin_Props.Radius = 0;
	FRAMEWIN_SetSkinFlexProps(&Framewin_Props, FRAMEWIN_SKINFLEX_PI_INACTIVE);

	FRAMEWIN_SetDefaultSkin  (_FRAMEWIN_DrawSkinFlex);
	PROGBAR_SetDefaultSkin   (PROGBAR_SKIN_FLEX);
	BUTTON_SetDefaultSkin    (BUTTON_SKIN_FLEX);
	SCROLLBAR_SetDefaultSkin (SCROLLBAR_SKIN_FLEX);
	SLIDER_SetDefaultSkin    (SLIDER_SKIN_FLEX);
	HEADER_SetDefaultSkin    (HEADER_SKIN_FLEX);
#endif

	btn = BUTTON_CreateAsChild(215, 10, 75, 35,hWM_HBKWIN_CURVE, ID_CURVE_BTN_SWTITCH_2_PARA, WM_CF_SHOW);
	btnClear = BUTTON_CreateAsChild(215, 60, 75, 35,hWM_HBKWIN_CURVE, 202, WM_CF_SHOW);
	BUTTON_SetText(btn,"Start");
	BUTTON_SetText(btnClear,"Back");


	hWN_PWM_CFG_Frame = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbBk_PWM_CFG, 0, 0, 0);	
	GUI_ExecCreatedDialog(hWN_PWM_CFG_Frame);

	WM_ValidateWindow(hWM_HBKWIN_CURVE);
	WM_ValidateWindow(WM_GetDesktopWindow());
	WM_ValidateWindow(hWN_PWM_CFG_Frame);	

	WM_HideWindow(hWM_HBKWIN_CURVE);
	WM_HideWindow(WM_GetDesktopWindow());
	WM_ShowWindow(hWN_PWM_CFG_Frame);


    while(1) 
	{
		GUI_Exec();	
	 	OSTimeDlyHMSM(0,0,0,50);
	}
}
/****************************************************************************
* ��    �ƣ�static  void  AppTaskKbd (void *p_arg)
* ��    �ܣ������������ȡ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
static  void  AppTaskKbd (void *p_arg)
{
  (void)p_arg;   
   while(1) 
   { 
   	  /* ��ʱ10ms���ȡһ�δ�������	*/
      OSTimeDlyHMSM(0,0,0,10); 	               
	  GUI_TOUCH_Exec();    
   }
}


#if (OS_APP_HOOKS_EN > 0)
/*
*********************************************************************************************************
*                                      TASK CREATION HOOK (APPLICATION)
*
* Description : This function is called when a task is created.
*
* Argument : ptcb   is a pointer to the task control block of the task being created.
*
* Note     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void App_TaskCreateHook(OS_TCB* ptcb)
{
}

/*
*********************************************************************************************************
*                                    TASK DELETION HOOK (APPLICATION)
*
* Description : This function is called when a task is deleted.
*
* Argument : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void App_TaskDelHook(OS_TCB* ptcb)
{
   (void) ptcb;
}

/*
*********************************************************************************************************
*                                      IDLE TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskIdleHook(), which is called by the idle task.  This hook
*               has been added to allow you to do such things as STOP the CPU to conserve power.
*
* Argument : none.
*
* Note     : (1) Interrupts are enabled during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 251
void App_TaskIdleHook(void)
{
}
#endif

/*
*********************************************************************************************************
*                                        STATISTIC TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskStatHook(), which is called every second by uC/OS-II's
*               statistics task.  This allows your application to add functionality to the statistics task.
*
* Argument : none.
*********************************************************************************************************
*/

void App_TaskStatHook(void)
{
}

/*
*********************************************************************************************************
*                                        TASK SWITCH HOOK (APPLICATION)
*
* Description : This function is called when a task switch is performed.  This allows you to perform other
*               operations during a context switch.
*
* Argument : none.
*
* Note     : 1 Interrupts are disabled during this call.
*
*            2  It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                   will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the
*                  task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

#if OS_TASK_SW_HOOK_EN > 0
void App_TaskSwHook(void)
{
}
#endif

/*
*********************************************************************************************************
*                                     OS_TCBInit() HOOK (APPLICATION)
*
* Description : This function is called by OSTCBInitHook(), which is called by OS_TCBInit() after setting
*               up most of the TCB.
*
* Argument : ptcb    is a pointer to the TCB of the task being created.
*
* Note     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 204
void App_TCBInitHook(OS_TCB* ptcb)
{
   (void) ptcb;
}
#endif

#endif
/******************* (C) COPYRIGHT 2011 �ܶ�STM32 *****END OF FILE****/
