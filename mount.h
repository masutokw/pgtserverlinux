#include "motor.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define SID_RATE 15.041
typedef enum {ALTAZ,EQ,ALT_ALT} mount_mode_t;
typedef struct
{
    mount_mode_t mount_mode;
    motor_t *azmotor,*altmotor;
    pthread_t th_motors,th_encoders;
    double dec_target,ra_target;//radians
    double alt_target,az_target;
    double current_dec,current_ra;//radians
    double lat,longitude;//arc seconds
    double rate[4];
    double maxspeed;//radians/sec
    int srate;
    int track;
    int time_zone;

       char is_tracking;
} mount_t;
void readconfig(void);
mount_t* create_mount(void);
int    destroy_mount(mount_t* m);
pthread_t motor_speed_thread_create(void* mt);
pthread_t  encoder_read_thread_create(void* mt);
pthread_t server_thread_create(void* t);
int set_mount_target( mount_t *mt,double ra,double dec);
int stop_mount(mount_t *mt,char direction);
int sync_ra_dec(mount_t *mt,double ra,double dec);
int goto_ra_dec(mount_t *mt,double ra,double dec);
void  tak_init(mount_t *mt);

