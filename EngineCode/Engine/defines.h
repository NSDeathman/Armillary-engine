/////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////
#include "stdafx.h"
/////////////////////////////////////////////////////////////////////
extern bool g_bNeedCloseApplication;
/////////////////////////////////////////////////////////////////////
#define ERROR_MESSAGE_NAME "Armillary engine error message"
/////////////////////////////////////////////////////////////////////
#define SAFE_DELETE(p)       \
	{                        \
		if (p)               \
		{                    \
			delete (p);      \
			(p) = NULL;      \
		}                    \
	}


#define SAFE_DELETE_ARRAY(p)\
	{                       \
		if (p)              \
		{                   \
			delete[](p);    \
			(p) = NULL;     \
		}                   \
	}


#define SAFE_RELEASE(x)		\
	{						\
		if (x)				\
		{					\
			(x)->Release();	\
			(x) = NULL;		\
		}					\
	}
/////////////////////////////////////////////////////////////////////
#define ASSERT(expession, error_message, ...)										\
{																					\
	if (expession == false)															\
	{																				\
		Msg(error_message);															\
		MessageBox(NULL, error_message, ERROR_MESSAGE_NAME, MB_OK);					\
		g_bNeedCloseApplication = true;												\
	}																				\
}																					\

#define ERROR_MESSAGE(error_message, ...)												\
	{																					\
			MessageBox(NULL, error_message, ERROR_MESSAGE_NAME, MB_OK);					\
			g_bNeedCloseApplication = true;												\
	}																					\
/////////////////////////////////////////////////////////////////////
#ifdef _DEBUG

#define VERIFY(expession, error_message, ...)                                                                          \
	{                                                                                                                  \
		if (expession == false)                                                                                        \
		{                                                                                                              \
			Msg(error_message);																						   \
			MessageBox(NULL, error_message, ERROR_MESSAGE_NAME, MB_OK);												   \
			g_bNeedCloseApplication = true;																			   \
		}                                                                                                              \
	}                                                                                                                  \

#else

#define VERIFY(expession, error_message, ...)                                                                          \
	{                                                                                                                  \
		if (expession == false)                                                                                        \
			Msg(error_message);																						   \
	}                                                                                                                  \

#endif
/////////////////////////////////////////////////////////////////////
