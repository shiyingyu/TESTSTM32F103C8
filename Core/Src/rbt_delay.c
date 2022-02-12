#include "rbt_delay.h"


static uint8_t  FAC_MICROSECOND = 0;		//us��ʱ������			   
static uint16_t FAC_MILLISECOND = 0;		//ms��ʱ������,��ucos��,����ÿ�����ĵ�ms��

/**
 * SysTick->LOAD ��װ�ؼĴ������δ��������
 * SysTick->VAL �ݼ�������
 */
	   
/**
 * @brief ��ʼ���ӳٺ���
 * @note ��ϵ�к���ʹ��SYSTICK��Ϊ��ʱ����SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
 * @param SYSCLK ϵͳʱ��Ƶ�ʣ���λΪMHz
 */
void RBT_Delay_Init(void)
{
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8); // �ⲿʱ��Դ8��Ƶ
	FAC_MICROSECOND = SystemCoreClock / 8000000;	//����72MHz����ôFAC_MICROSECOND = 9 (9�εδ��Ӧ1uS��
	FAC_MILLISECOND = (uint16_t)FAC_MICROSECOND * 1000; //��OS��,����ÿ��ms��Ҫ��systickʱ����   
}								    

/**
 * @brief ��ʱ����΢��
 * @param nMicroseconds Ҫ��ʱ��΢����.
 * @retval None
 */
void RBT_Delay_US(uint32_t nMicroseconds)
{		
	uint32_t ctrl;	    	 
	SysTick->LOAD = nMicroseconds * FAC_MICROSECOND; 	//ʱ�����	  		 
	SysTick->VAL = 0x00;        											//��ռ�����
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; 	 			//����������
	do
	{
		ctrl = SysTick->CTRL;
	} while( (ctrl & SysTick_CTRL_ENABLE_Msk) && !(ctrl & SysTick_CTRL_COUNTFLAG_Msk));			//�ȴ�ʱ�䵽��   
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;     		//�رռ�����
	SysTick->VAL = 0X00;       												//��ռ�����	 
}

/**
 * @brief ��ʱһ���ĺ�����
 * @note ע��nMilliseconds�ķ�Χ��SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
 * 				nms<=0xffffff*8*1000/SYSCLK��
 *				SYSCLK��λΪHz, nMilliseconds��λΪms��
 * 				��72M������,nMilliseconds<=1864 
 * @param nMilliseconds Ҫ��ʱ�ĺ�����
 * @retval None
 */
void RBT_Delay_MS(uint16_t nMilliseconds)
{	 		  	  
	uint32_t ctrl;		   
	SysTick->LOAD = (uint32_t)nMilliseconds * FAC_MILLISECOND;//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL = 0x00;           														//��ռ�����
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;          				//��ʼ����  
	do
	{
		ctrl = SysTick->CTRL;
	} while( (ctrl & SysTick_CTRL_ENABLE_Msk) && !(ctrl & SysTick_CTRL_COUNTFLAG_Msk));								//�ȴ�ʱ�䵽��   
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;   							//�رռ�����
	SysTick->VAL = 0X00;       																//��ռ�����	  	    
} 

/**
 * @brief ��ʱ������
 * @param nSeconds Ҫ��ʱ������
 * @retval None
 */
void RBT_Delay_Second(uint8_t nSeconds)
{
	while (nSeconds --)
		RBT_Delay_MS(1000);
	
}

void HAL_Delay(uint32_t Delay)
{
	RBT_Delay_MS(Delay);
}
