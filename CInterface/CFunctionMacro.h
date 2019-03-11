/********************************************************************
created:	2014/10/01
filename: 	CFunctionMacro.h
file base:	CFunctionMacro
purpose:	cross platform c function macro;
author:		fanxiangdong;
mail:		fanxiangdong7@126.com;
qq:			435337751;
*********************************************************************/
#ifndef __C_FUNCTION_MACRO_H__
#define __C_FUNCTION_MACRO_H__

#include <stdio.h>

#ifndef VSNPRINTF
#if defined(_MSC_VER) && (_MSC_VER >= 1200 )
#define VSNPRINTF		_vsnprintf
#elif defined(__GNUC__) && (__GNUC__ >= 3 )
#define VSNPRINTF		vsnprintf
#else
#define VSNPRINTF		vsnprintf
#endif
#endif // !VSNPRINTF

#ifndef SNPRINTF
#if defined(_MSC_VER) && (_MSC_VER >= 1200 )
#define SNPRINTF		_snprintf
#elif defined(__GNUC__) && (__GNUC__ >= 3 )
#define SNPRINTF		snprintf
#else
#define SNPRINTF		snprintf
#endif
#endif // !SNPRINTF


#endif // !__C_FUNCTION_MACRO_H__
