#include "rbt_delay.h"


static uint8_t  FAC_MICROSECOND = 0;		//us延时倍乘数			   
static uint16_t FAC_MILLISECOND = 0;		//ms延时倍乘数,在ucos下,代表每个节拍的ms数

/**
 * SysTick->LOAD 重装载寄存器（滴答的总量）
 * SysTick->VAL 递减计数器
 */
	   
/**
 * @brief 初始化延迟函数
 * @note 本系列函数使用SYSTICK作为计时器。SYSTICK的时钟固定为HCLK时钟的1/8
 * @param SYSCLK 系统时钟频率，单位为MHz
 */
void RBT_Delay_Init(void)
{
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8); // 外部时钟源8分频
	FAC_MICROSECOND = SystemCoreClock / 8000000;	//比如72MHz，那么FAC_MICROSECOND = 9 (9次滴答对应1uS）
	FAC_MILLISECOND = (uint16_t)FAC_MICROSECOND * 1000; //非OS下,代表每个ms需要的systick时钟数   
}								    

/**
 * @brief 延时多少微秒
 * @param nMicroseconds 要延时的微秒数.
 * @retval None
 */
void RBT_Delay_US(uint32_t nMicroseconds)
{		
	uint32_t ctrl;	    	 
	SysTick->LOAD = nMicroseconds * FAC_MICROSECOND; 	//时间加载	  		 
	SysTick->VAL = 0x00;        											//清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; 	 			//启动计数器
	do
	{
		ctrl = SysTick->CTRL;
	} while( (ctrl & SysTick_CTRL_ENABLE_Msk) && !(ctrl & SysTick_CTRL_COUNTFLAG_Msk));			//等待时间到达   
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;     		//关闭计数器
	SysTick->VAL = 0X00;       												//清空计数器	 
}

/**
 * @brief 延时一定的毫秒数
 * @note 注意nMilliseconds的范围。SysTick->LOAD为24位寄存器,所以,最大延时为:
 * 				nms<=0xffffff*8*1000/SYSCLK，
 *				SYSCLK单位为Hz, nMilliseconds单位为ms，
 * 				对72M条件下,nMilliseconds<=1864 
 * @param nMilliseconds 要延时的毫秒数
 * @retval None
 */
void RBT_Delay_MS(uint16_t nMilliseconds)
{	 		  	  
	uint32_t ctrl;		   
	SysTick->LOAD = (uint32_t)nMilliseconds * FAC_MILLISECOND;//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL = 0x00;           														//清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;          				//开始倒数  
	do
	{
		ctrl = SysTick->CTRL;
	} while( (ctrl & SysTick_CTRL_ENABLE_Msk) && !(ctrl & SysTick_CTRL_COUNTFLAG_Msk));								//等待时间到达   
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;   							//关闭计数器
	SysTick->VAL = 0X00;       																//清空计数器	  	    
} 

/**
 * @brief 延时多少秒
 * @param nSeconds 要延时的秒数
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
