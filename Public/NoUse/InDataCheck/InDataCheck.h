#ifndef _InDataCheck_H
#define _InDataCheck_H




#include "stm32f10x.h"


#define USE_FULL_ASSERTCHECK    1  //�Ƿ��ά���������



/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERTCHECK

/**
  * @brief  assert_param�����ڼ�麯���Ĳ���.
  * @param  expr: ���exprΪfalse�������assert_failed�������б���
  *         Դ�ļ������ƺ����е��к�
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
