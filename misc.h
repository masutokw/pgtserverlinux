#include <time.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  1
#endif
#ifndef RAD_TO_DEG
#define RAD_TO_DEG (180.0/M_PI) //  57.2958
#endif
//typedef struct timespec wallclock_t;
typedef struct timespec chrono_t;
chrono_t ti;
void chrono_set(chrono_t *const tptr);
double chrono_read(chrono_t *const tptr);
double sidereal_timeGMT (double longitude,double tz);
void lxprintde1(char* message,double ang);
void lxprintra1(char *message,double ang);
void clearScreen(void);


