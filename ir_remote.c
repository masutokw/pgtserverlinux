#include <sys/file.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/input.h>
#include <pthread.h>
#include "mount.h"
#include "ir_remote.h"
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  1
#endif
extern  char one;
int fd, rd;

extern mount_t *mount_test;

void move_ir(int fc)
{
    mount_test->altmotor->slewing= mount_test->azmotor->slewing=0;
    mount_test->is_tracking=0;
    int srate=mount_test->srate;
    switch (fc)
    {
    case KEY_UP:
        settargetspeed(mount_test->altmotor,SID_RATE*mount_test->rate[srate]*SEC_TO_RAD);
        break;
    case KEY_DOWN:
        settargetspeed(mount_test->altmotor,-SID_RATE*mount_test->rate[srate]*SEC_TO_RAD);
        break;
    case KEY_RIGHT:
        settargetspeed(mount_test->azmotor,SID_RATE*mount_test->rate[srate]*SEC_TO_RAD);

        break;
    case KEY_LEFT:
        settargetspeed(mount_test->azmotor,-SID_RATE*mount_test->rate[srate]*SEC_TO_RAD);

        break;
    case KEY_1:
        mount_test->srate=0;
        break;
    case KEY_2:
        mount_test->srate=1;
        break;
    case KEY_3:
        mount_test->srate=2;
        break;
    case KEY_4:
        mount_test->srate=3;
    case KEY_5:
        clearScreen();
        break;
        break;
    };
};

void stop_ir(int fc)
{
    mount_test->altmotor->slewing= mount_test->azmotor->slewing=FALSE;
    switch (fc)
    {
    case KEY_UP:
    case KEY_DOWN:
        mount_test->altmotor->targetspeed=0.0;
        do
        {
            usleep( 10000);
        }
        while (fabs(mount_test->altmotor->current_speed)>0.0);
        break;


    case KEY_LEFT:
    case KEY_RIGHT:
        mount_test->azmotor->targetspeed=0.0;
        do
        {
            usleep( 10000);
        }
        while (fabs(mount_test->azmotor->current_speed)>0.0);

        break;

    default:
        mount_test->altmotor->targetspeed=0.0;

        break;
    };
    one=FALSE;
    mount_test->is_tracking=TRUE;

}

void *thread_ir(void* m)
{
    struct input_event ev;
    while (1)
    {

        memset((void*)&ev, 0, sizeof(ev));

        rd = read (fd, (void*)&ev, sizeof(ev));

        if (rd <= 0)
        {
            printf ("rd: %d\n", rd);
            sleep(1);
        }

        if((rd>0) &&  (ev.type==1)&&(ev.value<2))
        {
            if (ev.value==1) move_ir(ev.code);
            if (ev.value==0) stop_ir(ev.code);

            printf("\033[s\033[17;10H type: %d, code: %d, value: %d, rd: %d \033[K\r\n\033[u", ev.type, ev.code, ev.value, rd);

        }
    }
}


pthread_t  IR_read_thread_create(void* mt)
{
    int t;
    pthread_t th;

    t=pthread_create(&th, NULL,thread_ir,mt);
    if (th)  printf("Read IR remote thread  running ID %d OK\r\n",t);
    else perror("Opps! IR  remote thread create failed");
    return th;
}


int ir_init (void)
{
    char *c;


    //Open Device
    if ((fd = open (DEV_IR_EVENT, O_RDONLY)) == -1)
    {
        printf ("not a vaild device.\n");
        return -1;
    }
    IR_read_thread_create(c);


    return 0;
}


