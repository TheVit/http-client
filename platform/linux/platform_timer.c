/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-10 22:16:41
 * @LastEditTime: 2020-05-25 10:13:26
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "platform_timer.h"

void platform_timer_init(platform_timer_t* timer)
{
    timer->time = {0, 0};
}

void platform_timer_cutdown(platform_timer_t* timer, unsigned int timeout)
{
    struct timeval now;
    //gettimeofday(&now, NULL);
	SYSTEMTIME time;
	GetSystemTime(&time);
	now.tv_sec = time.wSecond;
	now.tv_usec = time.wMilliseconds;

    struct timeval interval = {timeout / 1000, (timeout % 1000) * 1000};
    timeradd(&now, &interval, &timer->time);
}

char platform_timer_is_expired(platform_timer_t* timer)
{
    struct timeval now, res;
    //gettimeofday(&now, NULL);
	SYSTEMTIME time;
	GetSystemTime(&time);
	now.tv_sec = time.wSecond;
	now.tv_usec = time.wMilliseconds;
    
	timersub(&timer->time, &now, &res);
    return ((res.tv_sec < 0) || (res.tv_sec == 0 && res.tv_usec <= 0));
}

int platform_timer_remain(platform_timer_t* timer)
{
    struct timeval now, res;
    //gettimeofday(&now, NULL);
	SYSTEMTIME time;
	GetSystemTime(&time);
	now.tv_sec = time.wSecond;
	now.tv_usec = time.wMilliseconds;

    timersub(&timer->time, &now, &res);
	return (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000;
}

unsigned long platform_timer_now(void)
{
    return (unsigned long) time(NULL);
}



void platform_timer_usleep(unsigned long usec)
{
#ifdef WIN32
	Sleep(usec);
#else
    usleep(usec);
#endif
}
