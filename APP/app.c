/****************************************************************************
* Copyright (C), 2011 奋斗嵌入式工作室 www.ourstm.net
*
* 本例程在 奋斗版STM32开发板V2,2.1,V3,MINI上调试通过           
* QQ: 9191274, 旺旺：sun68, Email: sun68@163.com 
* 淘宝店铺：ourstm.taobao.com  
*
* 文件名: app.c
* 内容简述:
*       本例程操作系统采用ucos2.86a版本， 建立了4个任务
			任务名											 优先级
			APP_TASK_START_PRIO                               11	        主任务
			APP_TASK_USER_IF_PRIO                             13		ucgui界面任务
            APP_TASK_KBD_PRIO                                 12		触摸屏任务
            Task_NMEA_PRIO                                    10		GPS NMEA解析任务
		 当然还包含了系统任务：
		    OS_TaskIdle                  空闲任务-----------------优先级最低
			OS_TaskStat                  统计运行时间的任务-------优先级次低
*
* 文件历史:
* 版本号  日期       作者    说明
* v0.2    2011-7-06 sun68  创建该文件
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
WM_HWIN  hWM_HBKWIN_CURVE;
BUTTON_Handle btn,btnClear,
              btn1,btnClear1;


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
* 名    称：int main(void)
* 功    能：主函数入口
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
int main(void)
{
   CPU_INT08U os_err; 
   /* 禁止所有中断 */
   CPU_IntDis();
   
   /* ucosII 初始化 */
   OSInit();                                                  

   /* 硬件平台初始化 */
   BSP_Init();                               
   
   //建立主任务， 优先级最高  建立这个任务另外一个用途是为了以后使用统计任务
   os_err = OSTaskCreate((void (*) (void *)) App_TaskStart,               		    //指向任务代码的指针
                          (void *) 0,												//任务开始执行时，传递给任务的参数的指针
               		     (OS_STK *) &App_TaskStartStk[APP_TASK_START_STK_SIZE - 1],	//分配给任务的堆栈的栈顶指针   从顶向下递减
                         (INT8U) APP_TASK_START_PRIO);								//分配给任务的优先级
             
   OSTimeSet(0);			 //ucosII的节拍计数器清0    节拍计数器是0-4294967295  
   OSStart();                //启动ucosII内核   
   return (0);
}




/****************************************************************************
* 名    称：static  void App_TaskStart(void* p_arg)
* 功    能：开始任务建立
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
static  void App_TaskStart(void* p_arg)
{
 
  (void) p_arg;
   //初始化ucosII时钟节拍
   OS_CPU_SysTickInit();
                               
   //使能ucos 的统计任务
#if (OS_TASK_STAT_EN > 0)
   
   OSStatInit();                //----统计任务初始化函数                                 
#endif

   App_TaskCreate();			//建立其他的任务

   while (1)
   {  
      LED_LED1_ON();		      	  
	  OSTimeDlyHMSM(0, 0, 0, 100);

	  LED_LED1_OFF();
	  OSTimeDlyHMSM(0, 0, 0, 100);
   }
}

/****************************************************************************
* 名    称：static  void App_TaskCreate(void)
* 功    能：建立其余任务的函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
static  void App_TaskCreate(void)
{
  
   NMEA_MBOX=OSSemCreate(1);		                                           //建立NMEA指令解析的信号量
   /*  建立用户界面任务 */
   OSTaskCreateExt(AppTaskUserIF,											   //指向任务代码的指针
   					(void *)0,												   //任务开始执行时，传递给任务的参数的指针
   					(OS_STK *)&AppTaskUserIFStk[APP_TASK_USER_IF_STK_SIZE-1],  //分配给任务的堆栈的栈顶指针   从顶向下递减
					APP_TASK_USER_IF_PRIO,									   //分配给任务的优先级
					APP_TASK_USER_IF_PRIO,									   //预备给以后版本的特殊标识符，在现行版本同任务优先级
					(OS_STK *)&AppTaskUserIFStk[0],							   //指向任务堆栈栈底的指针，用于堆栈的检验
                    APP_TASK_USER_IF_STK_SIZE,									//指定堆栈的容量，用于堆栈的检验
                    (void *)0,													//指向用户附加的数据域的指针，用来扩展任务的任务控制块
                    OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);					//选项，指定是否允许堆栈检验，是否将堆栈清0,任务是否要
					                                                            //进行浮点运算等等。
                    
   /*   建立触摸驱动任务 */
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
		GUI_SetBkColor(GUI_BLUE);
	  GUI_Clear();
    GUI_DispStringAt("PWM Output", 100, 10);
   // GUIDEMO_DrawBk(1);
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

#if 0						
							if(1 == ClosePWM)
							{
									BUTTON_SetText(btn,"Closed");
							}
							else
							{
									BUTTON_SetText(btn,"Running");
							}
#endif

          } 	
					if (Id == 202)  /* ID =210 btn Button */  
					{                   	
						BUTTON_SetText(btnClear,"PID_Set");
					
						//WM_HideWindow(hWM_HBKWIN_CURVE);
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
* 名    称：static  void  AppTaskUserIF (void *p_arg)
* 功    能：用户界面任务
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
static  void  AppTaskUserIF (void *p_arg)
{				
	FRAMEWIN_SKINFLEX_PROPS Framewin_Props;

	(void)p_arg;								    
	GUI_Init();	                             //emWIN初始化 
	WM_SetDesktopColor(GUI_BLUE);

	GUI_Clear();

	WM_SetCreateFlags(WM_CF_MEMDEV);
	//GUIDEMO_Main();						 //界面主程序

	/* The First window to display the Temperature Curve */
	hWM_HBKWIN_CURVE =WM_GetDesktopWindow();
	//hWM_HBKWIN_CURVE=WM_CreateWindow(0,0,320,240,WM_CF_SHOW | WM_CF_MEMDEV ,0,0);
	WM_SetCallback(hWM_HBKWIN_CURVE, _cbBk);

	WM_SelectWindow(hWM_HBKWIN_CURVE);		
	

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

	btn = BUTTON_CreateAsChild(215, 10, 75, 35,hWM_HBKWIN_CURVE, ID_CURVE_BTN_SWTITCH_2_PARA, WM_CF_SHOW);
	btnClear = BUTTON_CreateAsChild(215, 60, 75, 35,hWM_HBKWIN_CURVE, 202, WM_CF_SHOW);
	BUTTON_SetText(btn,"Start");
	BUTTON_SetText(btnClear,"Back");
#endif
	//WM_ShowWindow(hWM_HBKWIN_CURVE);

	
    while(1) 
	{
		GUI_Exec();	
	 	OSTimeDlyHMSM(0,0,0,50);
	}
}
/****************************************************************************
* 名    称：static  void  AppTaskKbd (void *p_arg)
* 功    能：触摸屏坐标获取任务
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
static  void  AppTaskKbd (void *p_arg)
{
  (void)p_arg;   
   while(1) 
   { 
   	  /* 延时10ms会读取一次触摸坐标	*/
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
/******************* (C) COPYRIGHT 2011 奋斗STM32 *****END OF FILE****/
