/****************************************************************************
* Copyright (C), 2011 奋斗嵌入式工作室 www.ourstm.net
*
* 本例程在 奋斗版STM32开发板MINI，V2,2.1,V3上调试通过           
* QQ: 9191274, 旺旺：sun68, Email: sun68@163.com 
* 淘宝店铺：ourstm.taobao.com  
*
* 文件名: bsp.c
* 内容简述:
*       本例程提供了硬件平台的初始化
		
*
* 文件历史:
* 版本号  日期       作者    说明
* v0.2    2011-07-04 sun68  创建该文件
*
*/
#include "includes.h"
#include "demo.h"

//#include "..\fwlib\inc\Stm32f10x_tim.h"


u16 CCR1_Val=24000;		                        //背光亮度值

/* 定义了触摸芯片的SPI片选控制 */
#define TP_CS()  GPIO_ResetBits(GPIOB,GPIO_Pin_7)	  
#define TP_DCS() GPIO_SetBits(GPIOB,GPIO_Pin_7)

void tp_Config(void) ;
u16 TPReadX(void);
u16 TPReadY(void);
void NVIC_Configuration(void);
u16 TPReadX(void);
u16 TPReadY(void);
extern void FSMC_LCD_Init(void); 

/****************************************************************************
* 名    称：void Light_PWM(void)
* 功    能：通过定时器4的2通道，采用PWM方式调节TFT背光亮度
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 

void Light_PWM(void){
  TIM_TimeBaseInitTypeDef  TIM4_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM4_OCInitStructure;
 
  GPIO_InitTypeDef GPIO_InitStructure;	   
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);   	  //使能TIM4的时钟

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			  //配置为复用
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_PinRemapConfig(GPIO_Remap_TIM4 , ENABLE);			  //PD13复用为TIM4的通道2


   /*-------------------------------------------------------------------
  TIM3CLK=72MHz  预分频系数Prescaler=2 经过分频 定时器时钟为24MHz
  根据公式 通道输出占空比=TIM4_CCR2/(TIM_Period+1),可以得到TIM_Pulse的计数值	 
  捕获/比较寄存器2 TIM4_CCR2= CCR1_Val 	      
  -------------------------------------------------------------------*/
  TIM4_TimeBaseStructure.TIM_Prescaler = 0x02;		              //预分频器TIM4_PSC=3
  TIM4_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	  //计数器向上计数模式 TIM4_CR1[4]=0
  TIM4_TimeBaseStructure.TIM_Period = 24000;					  //自动重装载寄存器TIM4_APR  确定频率为1KHz 
  TIM4_TimeBaseStructure.TIM_ClockDivision = 0x0;				  //时钟分频因子 TIM4_CR1[9:8]=00
  TIM4_TimeBaseStructure.TIM_RepetitionCounter = 0x0;

  TIM_TimeBaseInit(TIM4,&TIM4_TimeBaseStructure);				  //写TIM4各寄存器参数

  
  TIM4_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 					  //PWM模式2 TIM4_CCMR1[14:12]=111 在向上计数时，
  																		  //一旦TIMx_CNT<TIMx_CCR1时通道1为无效电平，否则为有效电平
  TIM4_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 		  //输入/捕获2输出允许  OC2信号输出到对应的输出引脚PD13
  TIM4_OCInitStructure.TIM_Pulse = CCR1_Val; 							  //确定占空比，这个值决定了有效电平的时间。
  TIM4_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 			  //输出极性  低电平有效 TIM4_CCER[5]=1;


  TIM_OC2Init(TIM4,&TIM4_OCInitStructure); 
  TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
  


  /* TIM4使能 */
  TIM_Cmd(TIM4,ENABLE);

 
}

#if 0

/*
*********************************************************************************************************
*	函 数 名: SetPWM_TIM4
*	功能说明: 初始化GPIO,配置为PWM模式
*	形    参：_bright 亮度，0是灭，255是最亮
*	返 回 值: 无
*********************************************************************************************************
*/



void SetPWM_TIM4(unsigned short _bright,unsigned int PWM_CH)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* 第1步：打开GPIOB RCC_APB2Periph_AFIO 的时钟	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);


	
	/* Configure the PIN */
	{
		/* 配置背光GPIO为复用推挽输出模式 */
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		/* 使能TIM3的时钟 */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	}
		
			
	{
		/*
			TIM4 配置: 产生2路PWM信号;
			TIM3CLK = 72 MHz, Prescaler = 0(不分频), TIM3 counter clock = 72 MHz
			计算公式：
			PWM输出频率 = TIM3 counter clock /(ARR + 1)

			我们期望设置为100Hz

			如果不对TIM3CLK预分频，那么不可能得到100Hz低频。
			我们设置分频比 = 1000， 那么  TIM3 counter clock = 72KHz
			TIM_Period = 720 - 1;
			频率下不来。
		 */
		TIM_TimeBaseStructure.TIM_Period = 720 - 1;	/* TIM_Period = TIM3 ARR Register */
		TIM_TimeBaseStructure.TIM_Prescaler = 0;
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

		/* PWM1 Mode configuration */
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;// 输出模式
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	}	

	/*
		_bright = 1 时, TIM_Pulse = 1
		_bright = 255 时, TIM_Pulse = TIM_Period
	*/
	

	
	if(PWM_PB8_CH3 == PWM_CH)
	{
		TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period * _bright) / BRIGHT_MAX;	/* 改变占空比 */

		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OC3Init(TIM4, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	}
	
	if(PWM_PB9_CH4 == PWM_CH)
	{
		TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period * _bright) / BRIGHT_MAX;	/* 改变占空比 */

		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OC4Init(TIM4, &TIM_OCInitStructure);
		TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	}
	
	TIM_ARRPreloadConfig(TIM4, ENABLE);

	/* TIM4 enable counter */
	TIM_Cmd(TIM4, ENABLE);
}
#endif

/****************************************************************************
* 名    称：void RCC_Configuration(void)
* 功    能：系统时钟配置为72MHZ， 外设时钟配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void RCC_Configuration(void){
  SystemInit();  
}

/****************************************************************************
* 名    称：void GPIO_Configuration(void)
* 功    能：通用IO口配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/  
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  /* 使能各端口时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE , ENABLE);  
  	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9;				                 //LED1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
}



/****************************************************************************
* 名    称：void BSP_Init(void)
* 功    能：奋斗板初始化函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/  
void BSP_Init(void)
{   																		   
  RCC_Configuration();  	       //系统时钟初始化及端口外设时钟使能
  GPIO_Configuration();			   //状态LED1的初始化 
  tp_Config();					   //SPI1 触摸电路初始化
  FSMC_LCD_Init();			       //FSMC TFT接口初始化       
}

/****************************************************************************
* 名    称：void  OS_CPU_SysTickInit(void)
* 功    能：ucos 系统节拍时钟初始化  初始设置为10ms一次节拍
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void  OS_CPU_SysTickInit(void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    INT32U         cnts;
    RCC_GetClocksFreq(&rcc_clocks);		                        //获得系统时钟的值	 
    cnts = (INT32U)rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC;	//算出时钟节拍的值	
	SysTick_Config(cnts);										//设置时钟节拍	     
}

/****************************************************************************
* 名    称：void tp_Config(void)
* 功    能：TFT 触摸屏控制初始化
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void tp_Config(void) 
{ 
  GPIO_InitTypeDef  GPIO_InitStructure; 
  SPI_InitTypeDef   SPI_InitStructure; 

  /* SPI1 时钟使能 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE); 
 
  /* SPI1 SCK(PA5)、MISO(PA6)、MOSI(PA7) 设置 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//口线速度50MHZ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	        //复用模式
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* SPI1 触摸芯片的片选控制设置 PB7 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//口线速度50MHZ 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//推挽输出模式
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  
  /* 由于SPI1总线上挂接了4个外设，所以在使用触摸屏时，需要禁止其余3个SPI1 外设， 才能正常工作 */  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;           		//SPI1 SST25VF016B片选 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                //SPI1 VS1003片选 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                 //SPI1 网络模块片选
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOC, GPIO_Pin_4);							//SPI1 SST25VF016B片选置高 
  GPIO_SetBits(GPIOB, GPIO_Pin_12);							//SPI1 VS1003片选置高  
  GPIO_SetBits(GPIOA, GPIO_Pin_4);							//SPI1 网络模块片选置高 
  
   /* SPI1总线 配置 */ 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //全双工  
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						   //主模式
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					   //8位
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;						   //时钟极性 空闲状态时，SCK保持低电平
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						   //时钟相位 数据采样从第一个时钟边沿开始
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							   //软件产生NSS
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;  //波特率控制 SYSCLK/64
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   //数据高位在前
  SPI_InitStructure.SPI_CRCPolynomial = 7;							   //CRC多项式寄存器初始值为7 
  SPI_Init(SPI1, &SPI_InitStructure);
  
  /* SPI1 使能 */  
  SPI_Cmd(SPI1,ENABLE);  
}

/****************************************************************************
* 名    称：unsigned char SPI_WriteByte(unsigned char data) 
* 功    能：SPI1 写函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/  
unsigned char SPI_WriteByte(unsigned char data) 
{ 
 unsigned char Data = 0; 

  //等待发送缓冲区空
  while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET); 
  // 发送一个字节  
  SPI_I2S_SendData(SPI1,data); 

   //等待是否接收到一个字节 
  while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET); 
  // 获得该字节
  Data = SPI_I2S_ReceiveData(SPI1); 

  // 返回收到的字节 
  return Data; 
}  

/****************************************************************************
* 名    称：void SpiDelay(unsigned int DelayCnt) 
* 功    能：SPI1 写延时函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/  
void SpiDelay(unsigned int DelayCnt)
{
 unsigned int i;
 for(i=0;i<DelayCnt;i++);
}

/****************************************************************************
* 名    称：u16 TPReadX(void) 
* 功    能：触摸屏X轴数据读出
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/  
u16 TPReadX(void)
{ 
   u16 x=0;
   TP_CS();	                        //选择XPT2046 
   SpiDelay(10);					//延时
   SPI_WriteByte(0x90);				//设置X轴读取标志
   SpiDelay(10);					//延时
   x=SPI_WriteByte(0x00);			//连续读取16位的数据 
   x<<=8;
   x+=SPI_WriteByte(0x00);
   SpiDelay(10);					//禁止XPT2046
   TP_DCS(); 					    								  
   x = x>>3;						//移位换算成12位的有效数据0-4095
   return (x);
}
/****************************************************************************
* 名    称：u16 TPReadY(void)
* 功    能：触摸屏Y轴数据读出
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/
u16 TPReadY(void)
{
   u16 y=0;
   TP_CS();	                        //选择XPT2046 
   SpiDelay(10);					//延时
   SPI_WriteByte(0xD0);				//设置Y轴读取标志
   SpiDelay(10);					//延时
   y=SPI_WriteByte(0x00);			//连续读取16位的数据 
   y<<=8;
   y+=SPI_WriteByte(0x00);
   SpiDelay(10);					//禁止XPT2046
   TP_DCS(); 					    								  
   y = y>>3;						//移位换算成12位的有效数据0-4095
   return (y);
}

/******************* (C) COPYRIGHT 2011 奋斗STM32 *****END OF FILE****/
