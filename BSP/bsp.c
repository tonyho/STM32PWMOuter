/****************************************************************************
* Copyright (C), 2011 �ܶ�Ƕ��ʽ������ www.ourstm.net
*
* �������� �ܶ���STM32������MINI��V2,2.1,V3�ϵ���ͨ��           
* QQ: 9191274, ������sun68, Email: sun68@163.com 
* �Ա����̣�ourstm.taobao.com  
*
* �ļ���: bsp.c
* ���ݼ���:
*       �������ṩ��Ӳ��ƽ̨�ĳ�ʼ��
		
*
* �ļ���ʷ:
* �汾��  ����       ����    ˵��
* v0.2    2011-07-04 sun68  �������ļ�
*
*/
#include "includes.h"
#include "demo.h"

//#include "..\fwlib\inc\Stm32f10x_tim.h"


u16 CCR1_Val=24000;		                        //��������ֵ

/* �����˴���оƬ��SPIƬѡ���� */
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
* ��    �ƣ�void Light_PWM(void)
* ��    �ܣ�ͨ����ʱ��4��2ͨ��������PWM��ʽ����TFT��������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 

void Light_PWM(void){
  TIM_TimeBaseInitTypeDef  TIM4_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM4_OCInitStructure;
 
  GPIO_InitTypeDef GPIO_InitStructure;	   
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);   	  //ʹ��TIM4��ʱ��

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			  //����Ϊ����
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_PinRemapConfig(GPIO_Remap_TIM4 , ENABLE);			  //PD13����ΪTIM4��ͨ��2


   /*-------------------------------------------------------------------
  TIM3CLK=72MHz  Ԥ��Ƶϵ��Prescaler=2 ������Ƶ ��ʱ��ʱ��Ϊ24MHz
  ���ݹ�ʽ ͨ�����ռ�ձ�=TIM4_CCR2/(TIM_Period+1),���Եõ�TIM_Pulse�ļ���ֵ	 
  ����/�ȽϼĴ���2 TIM4_CCR2= CCR1_Val 	      
  -------------------------------------------------------------------*/
  TIM4_TimeBaseStructure.TIM_Prescaler = 0x02;		              //Ԥ��Ƶ��TIM4_PSC=3
  TIM4_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	  //���������ϼ���ģʽ TIM4_CR1[4]=0
  TIM4_TimeBaseStructure.TIM_Period = 24000;					  //�Զ���װ�ؼĴ���TIM4_APR  ȷ��Ƶ��Ϊ1KHz 
  TIM4_TimeBaseStructure.TIM_ClockDivision = 0x0;				  //ʱ�ӷ�Ƶ���� TIM4_CR1[9:8]=00
  TIM4_TimeBaseStructure.TIM_RepetitionCounter = 0x0;

  TIM_TimeBaseInit(TIM4,&TIM4_TimeBaseStructure);				  //дTIM4���Ĵ�������

  
  TIM4_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 					  //PWMģʽ2 TIM4_CCMR1[14:12]=111 �����ϼ���ʱ��
  																		  //һ��TIMx_CNT<TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ������Ϊ��Ч��ƽ
  TIM4_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 		  //����/����2�������  OC2�ź��������Ӧ���������PD13
  TIM4_OCInitStructure.TIM_Pulse = CCR1_Val; 							  //ȷ��ռ�ձȣ����ֵ��������Ч��ƽ��ʱ�䡣
  TIM4_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 			  //�������  �͵�ƽ��Ч TIM4_CCER[5]=1;


  TIM_OC2Init(TIM4,&TIM4_OCInitStructure); 
  TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
  


  /* TIM4ʹ�� */
  TIM_Cmd(TIM4,ENABLE);

 
}

#if 0

/*
*********************************************************************************************************
*	�� �� ��: SetPWM_TIM4
*	����˵��: ��ʼ��GPIO,����ΪPWMģʽ
*	��    �Σ�_bright ���ȣ�0����255������
*	�� �� ֵ: ��
*********************************************************************************************************
*/



void SetPWM_TIM4(unsigned short _bright,unsigned int PWM_CH)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* ��1������GPIOB RCC_APB2Periph_AFIO ��ʱ��	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);


	
	/* Configure the PIN */
	{
		/* ���ñ���GPIOΪ�����������ģʽ */
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		/* ʹ��TIM3��ʱ�� */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	}
		
			
	{
		/*
			TIM4 ����: ����2·PWM�ź�;
			TIM3CLK = 72 MHz, Prescaler = 0(����Ƶ), TIM3 counter clock = 72 MHz
			���㹫ʽ��
			PWM���Ƶ�� = TIM3 counter clock /(ARR + 1)

			������������Ϊ100Hz

			�������TIM3CLKԤ��Ƶ����ô�����ܵõ�100Hz��Ƶ��
			�������÷�Ƶ�� = 1000�� ��ô  TIM3 counter clock = 72KHz
			TIM_Period = 720 - 1;
			Ƶ���²�����
		 */
		TIM_TimeBaseStructure.TIM_Period = 720 - 1;	/* TIM_Period = TIM3 ARR Register */
		TIM_TimeBaseStructure.TIM_Prescaler = 0;
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

		/* PWM1 Mode configuration */
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;// ���ģʽ
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	}	

	/*
		_bright = 1 ʱ, TIM_Pulse = 1
		_bright = 255 ʱ, TIM_Pulse = TIM_Period
	*/
	

	
	if(PWM_PB8_CH3 == PWM_CH)
	{
		TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period * _bright) / BRIGHT_MAX;	/* �ı�ռ�ձ� */

		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OC3Init(TIM4, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	}
	
	if(PWM_PB9_CH4 == PWM_CH)
	{
		TIM_OCInitStructure.TIM_Pulse = (TIM_TimeBaseStructure.TIM_Period * _bright) / BRIGHT_MAX;	/* �ı�ռ�ձ� */

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
* ��    �ƣ�void RCC_Configuration(void)
* ��    �ܣ�ϵͳʱ������Ϊ72MHZ�� ����ʱ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void RCC_Configuration(void){
  SystemInit();  
}

/****************************************************************************
* ��    �ƣ�void GPIO_Configuration(void)
* ��    �ܣ�ͨ��IO������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/  
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  /* ʹ�ܸ��˿�ʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE , ENABLE);  
  	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9;				                 //LED1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
}



/****************************************************************************
* ��    �ƣ�void BSP_Init(void)
* ��    �ܣ��ܶ����ʼ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/  
void BSP_Init(void)
{   																		   
  RCC_Configuration();  	       //ϵͳʱ�ӳ�ʼ�����˿�����ʱ��ʹ��
  GPIO_Configuration();			   //״̬LED1�ĳ�ʼ�� 
  tp_Config();					   //SPI1 ������·��ʼ��
  FSMC_LCD_Init();			       //FSMC TFT�ӿڳ�ʼ��       
}

/****************************************************************************
* ��    �ƣ�void  OS_CPU_SysTickInit(void)
* ��    �ܣ�ucos ϵͳ����ʱ�ӳ�ʼ��  ��ʼ����Ϊ10msһ�ν���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void  OS_CPU_SysTickInit(void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    INT32U         cnts;
    RCC_GetClocksFreq(&rcc_clocks);		                        //���ϵͳʱ�ӵ�ֵ	 
    cnts = (INT32U)rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC;	//���ʱ�ӽ��ĵ�ֵ	
	SysTick_Config(cnts);										//����ʱ�ӽ���	     
}

/****************************************************************************
* ��    �ƣ�void tp_Config(void)
* ��    �ܣ�TFT ���������Ƴ�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void tp_Config(void) 
{ 
  GPIO_InitTypeDef  GPIO_InitStructure; 
  SPI_InitTypeDef   SPI_InitStructure; 

  /* SPI1 ʱ��ʹ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE); 
 
  /* SPI1 SCK(PA5)��MISO(PA6)��MOSI(PA7) ���� */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//�����ٶ�50MHZ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	        //����ģʽ
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* SPI1 ����оƬ��Ƭѡ�������� PB7 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//�����ٶ�50MHZ 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//�������ģʽ
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  
  /* ����SPI1�����Ϲҽ���4�����裬������ʹ�ô�����ʱ����Ҫ��ֹ����3��SPI1 ���裬 ������������ */  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;           		//SPI1 SST25VF016BƬѡ 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                //SPI1 VS1003Ƭѡ 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                 //SPI1 ����ģ��Ƭѡ
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOC, GPIO_Pin_4);							//SPI1 SST25VF016BƬѡ�ø� 
  GPIO_SetBits(GPIOB, GPIO_Pin_12);							//SPI1 VS1003Ƭѡ�ø�  
  GPIO_SetBits(GPIOA, GPIO_Pin_4);							//SPI1 ����ģ��Ƭѡ�ø� 
  
   /* SPI1���� ���� */ 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //ȫ˫��  
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						   //��ģʽ
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					   //8λ
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;						   //ʱ�Ӽ��� ����״̬ʱ��SCK���ֵ͵�ƽ
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						   //ʱ����λ ���ݲ����ӵ�һ��ʱ�ӱ��ؿ�ʼ
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							   //�������NSS
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;  //�����ʿ��� SYSCLK/64
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   //���ݸ�λ��ǰ
  SPI_InitStructure.SPI_CRCPolynomial = 7;							   //CRC����ʽ�Ĵ�����ʼֵΪ7 
  SPI_Init(SPI1, &SPI_InitStructure);
  
  /* SPI1 ʹ�� */  
  SPI_Cmd(SPI1,ENABLE);  
}

/****************************************************************************
* ��    �ƣ�unsigned char SPI_WriteByte(unsigned char data) 
* ��    �ܣ�SPI1 д����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/  
unsigned char SPI_WriteByte(unsigned char data) 
{ 
 unsigned char Data = 0; 

  //�ȴ����ͻ�������
  while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET); 
  // ����һ���ֽ�  
  SPI_I2S_SendData(SPI1,data); 

   //�ȴ��Ƿ���յ�һ���ֽ� 
  while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET); 
  // ��ø��ֽ�
  Data = SPI_I2S_ReceiveData(SPI1); 

  // �����յ����ֽ� 
  return Data; 
}  

/****************************************************************************
* ��    �ƣ�void SpiDelay(unsigned int DelayCnt) 
* ��    �ܣ�SPI1 д��ʱ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/  
void SpiDelay(unsigned int DelayCnt)
{
 unsigned int i;
 for(i=0;i<DelayCnt;i++);
}

/****************************************************************************
* ��    �ƣ�u16 TPReadX(void) 
* ��    �ܣ�������X�����ݶ���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/  
u16 TPReadX(void)
{ 
   u16 x=0;
   TP_CS();	                        //ѡ��XPT2046 
   SpiDelay(10);					//��ʱ
   SPI_WriteByte(0x90);				//����X���ȡ��־
   SpiDelay(10);					//��ʱ
   x=SPI_WriteByte(0x00);			//������ȡ16λ������ 
   x<<=8;
   x+=SPI_WriteByte(0x00);
   SpiDelay(10);					//��ֹXPT2046
   TP_DCS(); 					    								  
   x = x>>3;						//��λ�����12λ����Ч����0-4095
   return (x);
}
/****************************************************************************
* ��    �ƣ�u16 TPReadY(void)
* ��    �ܣ�������Y�����ݶ���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
u16 TPReadY(void)
{
   u16 y=0;
   TP_CS();	                        //ѡ��XPT2046 
   SpiDelay(10);					//��ʱ
   SPI_WriteByte(0xD0);				//����Y���ȡ��־
   SpiDelay(10);					//��ʱ
   y=SPI_WriteByte(0x00);			//������ȡ16λ������ 
   y<<=8;
   y+=SPI_WriteByte(0x00);
   SpiDelay(10);					//��ֹXPT2046
   TP_DCS(); 					    								  
   y = y>>3;						//��λ�����12λ����Ч����0-4095
   return (y);
}

/******************* (C) COPYRIGHT 2011 �ܶ�STM32 *****END OF FILE****/
