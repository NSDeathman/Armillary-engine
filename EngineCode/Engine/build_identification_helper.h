////////////////////////////////////////////////////////////////////////////////
// Created: 14.01.2025
// Author: Deathman
// Refactored code: Build identification
////////////////////////////////////////////////////////////////////////////////
#pragma once
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
////////////////////////////////////////////////////////////////////////////////
static const char* month_id[12] = 
{
	"Jan", 
	"Feb", 
	"Mar", 
	"Apr", 
	"May", 
	"Jun", 
	"Jul", 
	"Aug", 
	"Sep", 
	"Oct", 
	"Nov", 
	"Dec"
};

static int days_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static int start_day = 14;
static int start_month = 1;
static int start_year = 2025;
////////////////////////////////////////////////////////////////////////////////
u32 compute_build_id()
{
	LPCSTR build_date = __DATE__;

	int days;
	int months = 0;
	int years;
	string16 month;
	string256 buffer;
	strcpy_s(buffer, __DATE__);
	(void)sscanf(buffer, "%s %d %d", month, &days, &years);

	for (int i = 0; i < 12; i++)
	{
		if (_stricmp(month_id[i], month))
			continue;

		months = i;
		break;
	}

	u32 build_id = (years - start_year) * 365 + days - start_day;

	for (int i = 0; i < months; ++i)
	{			
		build_id += days_in_month[i];

		if (i == 2)
		{
			if (years % 4 == 0)
				build_id += 1;
		}
	}

	for (int i = 0; i < start_month - 1; ++i)
		build_id -= days_in_month[i];

	return build_id;
}
////////////////////////////////////////////////////////////////////////////////