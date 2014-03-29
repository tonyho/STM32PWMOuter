/****************************************************************************
* Copyright (C), 2011 奋斗嵌入式工作室 www.ourstm.net
*
* 本例程在 奋斗版STM32开发板V2,2.1,V3上调试通过           
* QQ: 9191274, 旺旺：sun68, Email: sun68@163.com 
* 淘宝店铺：ourstm.taobao.com  
*
* 文件名: LCDCONF.c
* 内容简述:
*       本例程提供了奋斗板配2.4寸屏（ILI9325 240X320)模块的emwin驱动程序
		
*
* 文件历史:
* 版本号  日期       作者    说明
* v0.1    2011-12-31 sun68  创建该文件
*
*/

#include "GUI.h"
#include "GUIDRV_FlexColor.h"
#include "stm32f10x.h"


#define Bank1_LCD_D    ((uint32_t)0x60020000)    //disp Data ADDR
#define Bank1_LCD_C    ((uint32_t)0x60000000)	 //disp Reg ADDR


void LCD_WR_REG(unsigned short index);
void LCD_WR_CMD(unsigned short index,unsigned short val);
void LCD_WR_Data(unsigned short val);
void LCD_WR_M_Data(U16 * pData, int NumWords);
void LCD_RD_M_Data(U16 * pData, int NumWords);
unsigned short LCD_RD_data(void);
void FSMC_LCD_Init(void);
void Delay(__IO uint32_t nCount);
/*********************************************************************
*
*       Layer configuration
*
**********************************************************************
*/
//
// 实际显示尺寸
//
#define XSIZE_PHYS 240
#define YSIZE_PHYS 320

//
// 颜色转换
//
#define COLOR_CONVERSION GUICC_565

//
// 显示驱动
//
#define DISPLAY_DRIVER GUIDRV_FLEXCOLOR

//
// Buffers / VScreens
//
#define NUM_BUFFERS   1
#define NUM_VSCREENS  1

//
// Display orientation
//
//    #define DISPLAY_ORIENTATION  0
//    #define DISPLAY_ORIENTATION (GUI_MIRROR_X | GUI_MIRROR_Y)
    #define DISPLAY_ORIENTATION (GUI_SWAP_XY | GUI_MIRROR_Y )
//    #define DISPLAY_ORIENTATION (GUI_SWAP_XY | GUI_MIRROR_X)
//
// Touch screen
//
#define USE_TOUCH   1

#define TOUCH_X_MIN 3869
#define TOUCH_X_MAX 377
#define TOUCH_Y_MIN 235
#define TOUCH_Y_MAX 3878



/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   COLOR_CONVERSION
  #error Color conversion not defined!
#endif
#ifndef   DISPLAY_DRIVER
  #error No display driver defined!
#endif
#ifndef   NUM_VSCREENS
  #define NUM_VSCREENS 1
#else
  #if (NUM_VSCREENS <= 0)
    #error At least one screeen needs to be defined!
  #endif
#endif
#if (NUM_VSCREENS > 1) && (NUM_BUFFERS > 1)
  #error Virtual screens and multiple buffers are not allowed!
#endif
#ifndef   DISPLAY_ORIENTATION
  #define DISPLAY_ORIENTATION  0
#endif

#if ((DISPLAY_ORIENTATION & GUI_SWAP_XY) != 0)
#define LANDSCAPE   1
#else
#define LANDSCAPE   0
#endif

#if (LANDSCAPE == 1)
#define WIDTH       YSIZE_PHYS  /* Screen Width (in pixels)         */
#define HEIGHT      XSIZE_PHYS  /* Screen Hight (in pixels)         */
#else
#define WIDTH       XSIZE_PHYS  /* Screen Width (in pixels)         */
#define HEIGHT      YSIZE_PHYS  /* Screen Hight (in pixels)         */
#endif

#if ((DISPLAY_ORIENTATION & GUI_SWAP_XY) != 0)
  #if ((DISPLAY_ORIENTATION & GUI_MIRROR_X) != 0)
    #define TOUCH_TOP    TOUCH_X_MAX
    #define TOUCH_BOTTOM TOUCH_X_MIN
  #else
    #define TOUCH_TOP    TOUCH_X_MIN
    #define TOUCH_BOTTOM TOUCH_X_MAX
  #endif
  #if ((DISPLAY_ORIENTATION & GUI_MIRROR_Y) != 0)
    #define TOUCH_LEFT   TOUCH_Y_MAX
    #define TOUCH_RIGHT  TOUCH_Y_MIN
  #else
    #define TOUCH_LEFT   TOUCH_Y_MIN
    #define TOUCH_RIGHT  TOUCH_Y_MAX
  #endif
#else
  #if ((DISPLAY_ORIENTATION & GUI_MIRROR_X) != 0)
    #define TOUCH_LEFT   TOUCH_X_MAX
    #define TOUCH_RIGHT  TOUCH_X_MIN
  #else
    #define TOUCH_LEFT   TOUCH_X_MIN
    #define TOUCH_RIGHT  TOUCH_X_MAX
  #endif
  #if ((DISPLAY_ORIENTATION & GUI_MIRROR_Y) != 0)
    #define TOUCH_TOP    TOUCH_Y_MAX
    #define TOUCH_BOTTOM TOUCH_Y_MIN
  #else
    #define TOUCH_TOP    TOUCH_Y_MIN
    #define TOUCH_BOTTOM TOUCH_Y_MAX
  #endif
#endif



/****************************************************************************
* 名    称：FSMC_LCD_Init(void)
* 功    能：FSMC接口配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void FSMC_LCD_Init(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;	
  GPIO_InitTypeDef GPIO_InitStructure;	    
  //使能FSMC外设时钟
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);   
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE , ENABLE);  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; 			  //LCD背光控制
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 	 		  //LCD复位
  GPIO_Init(GPIOE, &GPIO_InitStructure);   	   	
  // 复用端口为FSMC接口 FSMC-D0--D15
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);   
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure);    
  //FSMC NE1  LCD片选
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  //FSMC RS---LCD指令 指令/数据	切换
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; 
  GPIO_Init(GPIOD, &GPIO_InitStructure); 	
  GPIO_SetBits(GPIOD, GPIO_Pin_13);			           //LCD背光打开
  
  
  //FSMC接口特性配置
  p.FSMC_AddressSetupTime = 0x02;
  p.FSMC_AddressHoldTime = 0x00;
  p.FSMC_DataSetupTime = 0x05;
  p.FSMC_BusTurnAroundDuration = 0x00;
  p.FSMC_CLKDivision = 0x00;
  p.FSMC_DataLatency = 0x00;
  p.FSMC_AccessMode = FSMC_AccessMode_B;

 
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;
 
  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 		
  /* Enable FSMC Bank1_SRAM Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  
}




/****************************************************************************
* 名    称：LCD_WR_REG(unsigned int index)
* 功    能：FSMC写显示器寄存器地址函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void LCD_WR_REG(unsigned short index)
{
	*(__IO uint16_t *) (Bank1_LCD_C)= index;

}			 

/****************************************************************************
* 名    称：void LCD_WR_CMD(unsigned int index,unsigned int val)
* 功    能：FSMC写显示器寄存器数据函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void LCD_WR_CMD(unsigned short index,unsigned short val)
{	
	*(__IO uint16_t *) (Bank1_LCD_C)= index;	
	*(__IO uint16_t *) (Bank1_LCD_D)= val;
}


/****************************************************************************
* 名    称：unsigned short LCD_RD_data(void)
* 功    能：FSMC读显示区16位数据函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
unsigned short LCD_RD_data(void){
	unsigned int a=0;
	a=*(__IO uint16_t *) (Bank1_LCD_D);   //空操作
	a=*(__IO uint16_t *) (Bank1_LCD_D);   //读出的实际16位像素数据	  
	return(a);	
}



/****************************************************************************
* 名    称：LCD_WR_Data(unsigned int val)
* 功    能：FSMC写16位数据函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void LCD_WR_Data(unsigned short val)
{   
	*(__IO uint16_t *) (Bank1_LCD_D)= val; 	
}

/****************************************************************************
* 名    称：void LCD_WR_M_Data(U16 * pData, int NumWords)
* 功    能：FSMC写多字节的16位数据函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void LCD_WR_M_Data(U16 * pData, int NumWords){
  for (; NumWords; NumWords--) {
    *(__IO uint16_t *) (Bank1_LCD_D)= *pData++;
  }
}

/****************************************************************************
* 名    称：void LCD_RD_M_Data(U16 * pData, int NumWords)
* 功    能：FSMC读多字节的16位数据函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void LCD_RD_M_Data(U16 * pData, int NumWords){
  for (; NumWords; NumWords--){
    *pData++ = *(__IO uint16_t *) (Bank1_LCD_D); 
  }
}

/****************************************************************************
* 名    称：void Delay(__IO uint32_t nCount)
* 功    能：延时函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

/****************************************************************************
* 名    称：static void _InitController(void) 
* 功    能：2.4寸屏(ILI9325  240X320)的初始化
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
static void _InitController(void) 
{
  U32 i=0;
  GPIO_ResetBits(GPIOE, GPIO_Pin_1);	  //硬件复位
  Delay(0xaFFf);			   
  GPIO_SetBits(GPIOE, GPIO_Pin_1 );		 
  Delay(0xaFFf);	

  LCD_WR_CMD(0x00E3, 0x3008); // Set internal timing
  LCD_WR_CMD(0x00E7, 0x0012); // Set internal timing
  LCD_WR_CMD(0x00EF, 0x1231); // Set internal timing
  LCD_WR_CMD(0x0000, 0x0001); // Start Oscillation
  LCD_WR_CMD(0x0001, 0x0100); // set SS and SM bit
  LCD_WR_CMD(0x0002, 0x0700); // set 1 line inversion

  LCD_WR_CMD(0x0003, 0x1030); // set GRAM write direction and BGR=0,262K colors,1 transfers/pixel.
  LCD_WR_CMD(0x0004, 0x0000); // Resize register
  LCD_WR_CMD(0x0008, 0x0202); // set the back porch and front porch
  LCD_WR_CMD(0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
  LCD_WR_CMD(0x000A, 0x0000); // FMARK function
  LCD_WR_CMD(0x000C, 0x0000); // RGB interface setting
  LCD_WR_CMD(0x000D, 0x0000); // Frame marker Position
  LCD_WR_CMD(0x000F, 0x0000); // RGB interface polarity
//Power On sequence 
  LCD_WR_CMD(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
  LCD_WR_CMD(0x0011, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
  LCD_WR_CMD(0x0012, 0x0000); // VREG1OUT voltage
  LCD_WR_CMD(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
  Delay(200); // Dis-charge capacitor power voltage
  LCD_WR_CMD(0x0010, 0x1690); // SAP, BT[3:0], AP, DSTB, SLP, STB
  LCD_WR_CMD(0x0011, 0x0227); // R11h=0x0221 at VCI=3.3V, DC1[2:0], DC0[2:0], VC[2:0]
  Delay(50); // Delay 50ms
  LCD_WR_CMD(0x0012, 0x001C); // External reference voltage= Vci;
  Delay(50); // Delay 50ms
  LCD_WR_CMD(0x0013, 0x1800); // R13=1200 when R12=009D;VDV[4:0] for VCOM amplitude
  LCD_WR_CMD(0x0029, 0x001C); // R29=000C when R12=009D;VCM[5:0] for VCOMH
  LCD_WR_CMD(0x002B, 0x000D); // Frame Rate = 91Hz
  Delay(50); // Delay 50ms
  LCD_WR_CMD(0x0020, 0x0000); // GRAM horizontal Address
  LCD_WR_CMD(0x0021, 0x0000); // GRAM Vertical Address
// ----------- Adjust the Gamma Curve ----------//
  LCD_WR_CMD(0x0030, 0x0007);
  LCD_WR_CMD(0x0031, 0x0302);
  LCD_WR_CMD(0x0032, 0x0105);
  LCD_WR_CMD(0x0035, 0x0206);
  LCD_WR_CMD(0x0036, 0x0808);
  LCD_WR_CMD(0x0037, 0x0206);
  LCD_WR_CMD(0x0038, 0x0504);
  LCD_WR_CMD(0x0039, 0x0007);
  LCD_WR_CMD(0x003C, 0x0105);
  LCD_WR_CMD(0x003D, 0x0808);
//------------------ Set GRAM area ---------------//
  LCD_WR_CMD(0x0050, 0x0000); // Horizontal GRAM Start Address
  LCD_WR_CMD(0x0051, 0x00EF); // Horizontal GRAM End Address
  LCD_WR_CMD(0x0052, 0x0000); // Vertical GRAM Start Address
  LCD_WR_CMD(0x0053, 0x013F); // Vertical GRAM Start Address
  LCD_WR_CMD(0x0060, 0xA700); // Gate Scan Line
  LCD_WR_CMD(0x0061, 0x0001); // NDL,VLE, REV
  LCD_WR_CMD(0x006A, 0x0000); // set scrolling line
//-------------- Partial Display Control ---------//
  LCD_WR_CMD(0x0080, 0x0000);
  LCD_WR_CMD(0x0081, 0x0000);
  LCD_WR_CMD(0x0082, 0x0000);
  LCD_WR_CMD(0x0083, 0x0000);
  LCD_WR_CMD(0x0084, 0x0000);
  LCD_WR_CMD(0x0085, 0x0000);
//-------------- Panel Control -------------------//
  LCD_WR_CMD(0x0090, 0x0010);
  LCD_WR_CMD(0x0092, 0x0000);
  LCD_WR_CMD(0x0093, 0x0003);
  LCD_WR_CMD(0x0095, 0x0110);
  LCD_WR_CMD(0x0097, 0x0000);
  LCD_WR_CMD(0x0098, 0x0000);
  LCD_WR_CMD(0x0007, 0x0133); // 262K color and display ON

  LCD_WR_CMD(32, 0);
  LCD_WR_CMD(33, 0x013F);
  *(__IO uint16_t *) (Bank1_LCD_C)= 34;
  for(i=0;i<76800;i++)
  {
    LCD_WR_Data(0xffff);
  }   
}

/****************************************************************************
* 名    称：LCD_X_Config(void)
* 功    能：显示器的驱动配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void LCD_X_Config(void) 
{
  GUI_DEVICE * pDevice;
  GUI_PORT_API PortAPI = {0}; 
  CONFIG_FLEXCOLOR Config = {0};
  
  #if (NUM_BUFFERS > 1)
    GUI_MULTIBUF_Config(NUM_BUFFERS);
  #endif
  /* 设置第一层的显示驱动和颜色转换 */
  pDevice = GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
  /* 公共显示驱动配置 */
   LCD_SetSizeEx(0, YSIZE_PHYS, XSIZE_PHYS);               // 实际显示像素
  if (LCD_GetSwapXY()) 
  {
    LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
    LCD_SetVSizeEx(0, YSIZE_PHYS * NUM_VSCREENS, XSIZE_PHYS);
  } 
  else 
  {
    LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS * NUM_VSCREENS);
  }							     

  /* 设置命令和数据模式 */ 	   
  PortAPI.pfWrite16_A0  = LCD_WR_REG;
  PortAPI.pfWrite16_A1  = LCD_WR_Data;
  PortAPI.pfWriteM16_A1 = LCD_WR_M_Data;
  PortAPI.pfReadM16_A1  = LCD_RD_M_Data;
  GUIDRV_FlexColor_SetFunc(pDevice, &PortAPI, GUIDRV_FLEXCOLOR_F66708, GUIDRV_FLEXCOLOR_M16C0B16);
  /* 显示方向和偏移	*/ 
  Config.Orientation   = DISPLAY_ORIENTATION;
  Config.RegEntryMode  = 0;
  GUIDRV_FlexColor_Config(pDevice, &Config);
  #if (USE_TOUCH == 1)
    /*设置显示方向*/
    GUI_TOUCH_SetOrientation(DISPLAY_ORIENTATION);
    
    /* 校准触摸屏 */   
    GUI_TOUCH_Calibrate(GUI_COORD_X, 0, HEIGHT - 1, TOUCH_LEFT, TOUCH_RIGHT);
    GUI_TOUCH_Calibrate(GUI_COORD_Y, 0,  WIDTH  - 1,TOUCH_TOP,  TOUCH_BOTTOM);
  #endif
}


/****************************************************************************
* 名    称：LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData)
* 功    能：显示器的驱动初始化
* 入口参数：LayerIndex - 配置层的序号
*   		Cmd        - 请参考下面的switch语句的细节
*   		pData      - LCD_X_DATA指针结构
* 出口参数：< -1 - 错误
*     		  -1 - 命令不处理
*      		   0 - 正确
* 说    明：
* 调用方法：无 
****************************************************************************/ 
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r;

  GUI_USE_PARA(LayerIndex);
  GUI_USE_PARA(pData);

  switch (Cmd){  
  	case LCD_X_INITCONTROLLER: 
  	{	   
       _InitController();      //LCD初始化
       return 0;
    }
    default:  r = -1;
  }
  return r;
}

/*************************** End of file ****************************/
