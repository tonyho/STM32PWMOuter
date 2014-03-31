#if !defined( _BSP_H )
#define _BSP_H

#define PWM_PB0_CH3    3
#define PWM_BACK_LIGHT 4

#define PWM_PB9_CH4  5
#define PWM_PB8_CH3  6


#define BRIGHT_MAX		255		/* 最大亮度 */
#define BRIGHT_MIN		0		/* 最小亮度,本例设置为0 */
#define BRIGHT_DEFAULT	200		/* 缺省亮度 */


void RCC_Configuration(void);

void GPIO_Configuration(void);

void NVIC_Configuration(void);

void Light_PWM(void);

void SetPWM_TIM4(unsigned short _bright,unsigned int PWM_CH);


CPU_INT32U  BSP_CPU_ClkFreq (void);

//INT32U  OS_CPU_SysTickClkFreq (void);

void  OS_CPU_SysTickInit(void);

void BSP_Init(void);


#ifdef  DEBUG
void assert_failed(u8* file, u32 line)
#endif



#endif
