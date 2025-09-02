////////////////////////////////////////////////////////////////////////////////
// Created: 14.01.2025
// Author: DeepSeek, ChatGPT, NS_Deathman
// New Build Identification Code Generator
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "build_identificator.h"
#include "log.h"
////////////////////////////////////////////////////////////////////////////////
// ��������� ��� ���������� ���� ������
namespace BuildConstants
{
constexpr const char* monthNames[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
										"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

constexpr int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

constexpr int startYear = 2025; // ��� ������ ������� ������
constexpr int startMonth = 1;	// ������
constexpr int startDay = 15;	// 15-� �����
} // namespace BuildConstants

// ������� ��� ��������� ����� ��� ����� �������� (������������������)
int strCaseCmp(const char* s1, const char* s2)
{
	while (*s1 && *s2)
	{
		int diff = std::tolower(static_cast<unsigned char>(*s1)) - std::tolower(static_cast<unsigned char>(*s2));
		if (diff != 0)
			return diff;
		s1++;
		s2++;
	}
	return std::tolower(static_cast<unsigned char>(*s1)) - std::tolower(static_cast<unsigned char>(*s2));
}

// �������� �� ���������� ���
constexpr bool isLeapYear(int year)
{
	return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// ��������� ���������� ���� � ������ � ������ ����������� ����
constexpr int getDaysInMonth(int month, int year)
{
	if (month == 1) // �������
		return BuildConstants::daysInMonth[month] + (isLeapYear(year) ? 1 : 0);
	return BuildConstants::daysInMonth[month];
}

// ���������� �������� ����� ID ������ (�� ������ ����)
uint32_t computeBuildIdMajor()
{
	// �������� ���� ������ �� �������������
	const char* buildDate = __DATE__;

	char month[16] = {0};
	int day = 0;
	int year = 0;

	// ������ ���� � ������� "MMM DD YYYY" ��� "MMM  D YYYY"
	if (std::sscanf(buildDate, "%s %d %d", month, &day, &year) != 3)
	{
		// Fallback: ���������� ������� ����
		std::time_t now = std::time(nullptr);
		std::tm* tm = std::localtime(&now);
		day = tm->tm_mday;
		year = tm->tm_year + 1900;
		std::strncpy(month, BuildConstants::monthNames[tm->tm_mon], 3);
		month[3] = '\0';
	}

	// ������������ �������� ������ � ������ (0-11)
	int monthIndex = -1;
	for (int i = 0; i < 12; ++i)
	{
		if (strCaseCmp(BuildConstants::monthNames[i], month) == 0)
		{
			monthIndex = i;
			break;
		}
	}

	if (monthIndex == -1)
	{
		// �� ������� ���������� �����, ���������� ������
		monthIndex = 0;
	}

	// ��������� ����� ���������� ���� �� ��������� ����
	uint32_t totalDays = 0;

	// ��������� ������ ����
	for (int y = BuildConstants::startYear; y < year; ++y)
	{
		totalDays += isLeapYear(y) ? 366 : 365;
	}

	// ��������� ��� � ������� ���� �� �������� ������
	for (int m = 0; m < monthIndex; ++m)
	{
		totalDays += getDaysInMonth(m, year);
	}

	// ��������� ��� �������� ������
	totalDays += day - 1; // -1 ������ ��� ��� ��������� � 1

	// �������� ��� �� ��������� ����
	uint32_t startDays = 0;
	for (int m = 0; m < BuildConstants::startMonth - 1; ++m)
	{
		startDays += getDaysInMonth(m, BuildConstants::startYear);
	}
	startDays += BuildConstants::startDay - 1;

	return totalDays - startDays;
}

// ���������� �������������� ����� ID ������ (�� ������ �������)
uint32_t computeBuildIdMinor()
{
	const char* buildTime = __TIME__;

	int hour = 0;
	int minute = 0;
	int second = 0;

	if (std::sscanf(buildTime, "%d:%d:%d", &hour, &minute, &second) != 3)
	{
		// Fallback: ���������� ������� �����
		std::time_t now = std::time(nullptr);
		std::tm* tm = std::localtime(&now);
		hour = tm->tm_hour;
		minute = tm->tm_min;
		second = tm->tm_sec;
	}

	// ���������� ���������� ����� � �������� (0-1439)
	return static_cast<uint32_t>(hour * 60 + minute);
}

// ��������� ������ ���������� � ������
struct BuildInfo
{
	uint32_t majorId;
	uint32_t minorId;
	const char* buildType;
	const char* architecture;
	const char* buildDate;
	const char* buildTime;
};

BuildInfo getBuildInfo()
{
	return BuildInfo{computeBuildIdMajor(),
					 computeBuildIdMinor(),
#ifdef _DEBUG
					 "Debug",
#else
					 "Release",
#endif
#ifdef _WIN64
					 "x64",
#elif defined(_WIN32)
					 "x86",
#else
					 "Unknown",
#endif
					 __DATE__,
					 __TIME__};
}

// ����� ���������� � ������
void PrintBuildData()
{
	BuildInfo info = getBuildInfo();

	Msg("Armillary Engine");
	Msg("Build ID: %u.%u", info.majorId, info.minorId);
	Msg("Build type: %s", info.buildType);
	Msg("Architecture: %s", info.architecture);
	Msg("Build date: %s", info.buildDate);
	Msg("Build time: %s", info.buildTime);
	Msg("Compilation timestamp: %s %s\n", __DATE__, __TIME__);
}
////////////////////////////////////////////////////////////////////////////////
