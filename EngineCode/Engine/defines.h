#pragma once

#include "stdafx.h"


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

#define ASSERT(expession, error_message, ...)										\
{																					\
	if (expession == false)															\
	{																				\
		Log->Print(error_message);													\
		MessageBox(NULL, error_message, "Armillary engine error message", MB_OK);	\
	}																				\
}																					\

#define ERROR_MESSAGE(error_message, ...)												\
	{																					\
			MessageBox(NULL, error_message, "Armillary engine error message", MB_OK);	\
	}																					\

#ifdef _DEBUG

#define VERIFY(expession, error_message, ...)                                                                          \
	{                                                                                                                  \
		if (expession == false)                                                                                        \
		{                                                                                                              \
			Log->Print(error_message);                                                                                 \
			MessageBox(NULL, error_message, "Armillary engine error message", MB_OK);                                  \
		}                                                                                                              \
	}                                                                                                                  \

#else

#define VERIFY(expession, error_message, ...)                                                                          \
	{                                                                                                                  \
		if (expession == false)                                                                                        \
			Log->Print(error_message);                                                                                 \
	}                                                                                                                  \

#endif
