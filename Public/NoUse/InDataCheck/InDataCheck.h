#ifndef _InDataCheck_H
#define _InDataCheck_H




#include "stm32f10x.h"


#define USE_FULL_ASSERTCHECK    1  //是否打开维护参数检查



/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERTCHECK

/**
  * @brief  assert_param宏用于检查函数的参数.
  * @param  expr: 如果expr为false，则调用assert_failed函数进行报告
  *         源文件的名称和运行的行号
  *         that failed. If expr is true, it returns no value.
  * @retval None
  */
  #define Myassert_param(expr) ((expr) ? (void)0 : Mytassert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void Myassert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */











#endif
