////////////////////////////////////////////////////////////////////////////////
// Created: 14.01.2025
// Author: ChatGPT, NS_Deathman
// New Build Identification Code Generator
////////////////////////////////////////////////////////////////////////////////
#pragma once
////////////////////////////////////////////////////////////////////////////////
#include <cstdio>
#include <cstring>
#include <string>

// Constants for build identification
static const char* monthNames[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
									 "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

static const int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// Starting point for the build ID computation
static const int startDay = 15;
static const int startMonth = 1;
static const int startYear = 2025;

////////////////////////////////////////////////////////////////////////////////
u32 computeBuildIdMajor()
{
	const char* buildDate = __DATE__;

	int day = 0;
	int monthIndex = 0;
	int year = 0;
	char month[16];

	if (sscanf(buildDate, "%s %d %d", month, &day, &year) != 3)
	{
		// Error handling: unable to parse build date
		return 0;
	}

	// Convert month name to index
	for (int i = 0; i < 12; ++i)
	{
		if (strcasecmp(monthNames[i], month) == 0)
		{
			monthIndex = i;
			break;
		}
	}

	u32 buildId = (year - startYear) * 365 + (day - startDay);

	// Calculate total days from previous months
	for (int i = 0; i < monthIndex; ++i)
	{
		buildId += daysInMonth[i];
	}

	// Account for leap year in February
	if (monthIndex > 1 && (year % 4 == 0))
	{
		buildId += 1; // Add an extra day for leap year
	}

	// Subtract days from months before the starting month
	for (int i = 0; i < startMonth - 1; ++i)
	{
		buildId -= daysInMonth[i];
	}

	return buildId;
}

u32 computeBuildIdMinor()
{
	const char* buildTime = __TIME__;

	int hour = 0;
	int minute = 0;
	int second = 0;

	if (sscanf(buildTime, "%d:%d:%d", &hour, &minute, &second) != 3)
	{
		// Error handling: unable to parse build time
		return 0;
	}

	return (hour * 60) + minute; // Return total minutes since midnight
}
////////////////////////////////////////////////////////////////////////////////
