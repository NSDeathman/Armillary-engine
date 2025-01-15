#pragma once

#define _RELEASE(x)                                                                                                    \
	{                                                                                                                  \
		if (x)                                                                                                         \
		{                                                                                                              \
			(x)->Release();                                                                                            \
			(x) = NULL;                                                                                                \
		}                                                                                                              \
	}