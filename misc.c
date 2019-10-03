#include "misc.h"

void chrono_set(chrono_t *const tptr)

{

    clock_gettime(CLOCK_REALTIME, tptr);

}

double chrono_read(chrono_t *const tptr)
{
    struct timespec  now;
    clock_gettime(CLOCK_REALTIME, &now);

    return difftime(now.tv_sec, tptr->tv_sec)
            + ((double)now.tv_nsec - (double)tptr->tv_nsec) / 1000000000.0;
}
/*
double ln_range_degrees (double angle)
{
    double temp;

    if (angle >= 0.0 && angle < 360.0)
        return angle;

    temp = (int)(angle / 360);
    if (angle < 0.0)
        temp --;
    temp *= 360;
    return angle - temp;
}
*/

double sidereal_timeGMT (double longitude,double tz)
{
    struct timespec now;
    double JD;
    double sidereal,tmp;
    double T;
    clock_gettime(CLOCK_REALTIME,&now);


    JD = (now.tv_sec+now.tv_nsec/ 1000000000.0)/86400.0+2440587.5;

    T = (JD - 2451545.0) / 36525.0;

    /* calc mean angle */
    sidereal = 280.46061837 + (360.98564736629 * (JD - 2451545.0)) + (0.000387933 * T * T) - (T * T * T / 38710000.0);

    /* add a convenient multiple of 360 degrees */

  //  sidereal = ln_range_degrees (sidereal+longitude);
    sidereal = fmod (sidereal+longitude,360.0);


    /* change to hours */
    sidereal /= 15.0;

    return sidereal;
}

void getTime (char* _timeStr)
{
    time_t milli;
    struct timespec curTime;
    struct tm* info;

    clock_gettime(CLOCK_REALTIME, &curTime);
    milli = curTime.tv_nsec;

    info = localtime(&curTime.tv_sec);
    sprintf(_timeStr, "%d-%d-%d %d:%d:%d.%d", 1900 + info->tm_year, info->tm_mon, info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec,  (int)milli);

}



void lxprintde1(char* message,double ang)

{
    if (ang>M_PI) ang=ang-(M_PI*2.0);

    int x = ang*RAD_TO_DEG*3600.0;
    char c='+';
    if (x<0)
    {
        x=-x;
        c= '-';
    }
    int gra=x/3600;
    int temp=(x %3600);
    int min=temp/60;
    int sec=temp%60;
    sprintf(message,"%c%02d%c%02d:%02d#",c,gra,223,min,sec);

};
void lxprintra1(char *message,double ang)
{
    int x = ang*RAD_TO_DEG*3600.0/15.0;
    int gra=x/3600;
    int temp=(x %3600);
    int min=temp/60;
    int sec=temp%60;
    sprintf(message,"%02d:%02d:%02d#",gra,min,sec);
    //APPEND
};
void clearScreen(void)
{
  const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
  write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
}
