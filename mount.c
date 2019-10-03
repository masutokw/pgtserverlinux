#include "mount.h"
#include <unistd.h>
#include <pthread.h>
#include "misc.h"
#include "taki.h"
#define SID_RATE 15.041
extern void* sserver(void *);
extern c_star st_now, st_target, st_current;
char port[]="/dev/tty     ";
char one=FALSE;
mount_t* create_mount(void)
{
    FILE  *fp;

    int maxcounter;
    int maxcounteralt=maxcounter=4000*6*180;
    mount_t *m;
    m=(mount_t*)malloc(sizeof(mount_t));
    //if (m) return NULL;
    m->azmotor=(motor_t*)malloc(sizeof(motor_t));
    m->altmotor=(motor_t*)malloc(sizeof(motor_t));
    if ((fp=fopen("./pgt.conf","r"))!=NULL)
    {
        //fscanf(fp,"%d%f",&maxcounter,&fg);
        fscanf(fp,"%d %d %lf %lf %lf %lf %lf %lf %s",&maxcounter,&maxcounteralt,&(m->rate[0]), &(m->rate[1]), &(m->rate[2]), &(m->rate[3]),&(m->longitude),&(m->lat),&port);
       // fscanf(fp,"%s",port);
        fclose(fp);
    }
    else
    {
        m->track=0;
        m->rate[3]=400.0;
        m->rate[2]=50.0;
        m->rate[1]=8.0;
        m->rate[0]=2.0;
        m->srate=0;
        m->longitude=-4.2;
        m->lat=36.71;
    }
    printf("\r\nlongitude:%f %d %d  %s\r\n",m->longitude,maxcounter,maxcounteralt,port);
    m->maxspeed=(m->rate[3]*SID_RATE*SEC_TO_RAD);
    init_motor( m->azmotor,0xfe,maxcounter);
    init_motor( m->altmotor,0xfd,maxcounteralt);
    m->is_tracking=FALSE;
    m->mount_mode=EQ;
    m->mount_mode=ALTAZ;
    return m;

}
int  destroy_mount(mount_t* m)
{
    free(m->azmotor);
    free(m->altmotor);
    free(m);

}


void *thread_motor(void* m)
{    do
    {
        speed_up_down(((mount_t*)m)->azmotor);
        speed_up_down(((mount_t*)m)->altmotor);
        usleep(30000);

    }
    while (1);
}
pthread_t motor_speed_thread_create(void* mt)
{
    pthread_t th;
    int t= pthread_create(&th, NULL, thread_motor, mt);
    if (th)  printf("Motor control thread running ID %d OK \r\n",th);
    else perror("Opps! thread create failed");
    return th;
}

void *track(void * mt1)
{
    char timeStr[50];


    mount_t *mt;
    mt=(mount_t*)mt1;
    mt->is_tracking=TRUE;
    double d_az_r, d_alt_r,ct;
       do
    {


            readcounter_n(mt->altmotor);
          // usleep(10000);
           readcounter(mt->azmotor);
           st_target.timer_count = st_current.timer_count = ct=chrono_read(&ti);;
            //compute correct actual alt-az mount values
            //translate counter values to current position alt/az values
            st_current.az = mt->azmotor->position;
            st_current.alt = mt->altmotor->position;
            //compute ecuatorial current equatorial values to be send out from LX200 protocol interface
            to_equatorial(&st_current);

            if (mt->is_tracking && one)
            {
                //compute next alt/az mount values  for target next lap second
                st_target.timer_count += 1.0;
                to_alt_az(&st_target);
                //compute delta values :next values from actual values for desired target coordinates
                d_az_r = st_target.az- st_current.az;
                if (fabs(d_az_r) > (M_PI)) d_az_r -= M_2PI;
                d_alt_r = st_target.alt- st_current.alt;;
                if (fabs(d_alt_r) > (M_PI)) d_alt_r -= M_2PI;
                // Compute and set timer speed for stepper  rates
                settargetspeed(mt->azmotor, d_az_r);
                settargetspeed(mt->altmotor, d_alt_r);

            }
            else
            {
                // settargetspeed(mt->azmotor, az_frate);
                // settargetspeed(mt->altmotor, alt_frate);
                //  if (trackmode == 1) {
                //    tracking = (speed_y == 0.0) && (speed_x == 0.0);
                st_target.ra = st_current.ra;
                st_target.dec = st_current.dec;
                 one= TRUE;
            }
        getTime(timeStr);printf("\033[s\033[1;4H Date: %s  \033[K\r\n\033[u", timeStr);
         printf("\033[s\033[2;4H LST: %f TRT:%f \033[K\r\n\033[u", sidereal_timeGMT (mt->longitude, mt->time_zone),ct);
        printf("\033[s\033[3;4H Counters:%d %d \033[K\r\n\033[u",(int)mt->azmotor->counter,(int)mt->altmotor->counter);

        printf("\033[s\033[5;4H TARGET:  %f %+f  RA/DEC:%f %+f \033[K\r\n\033[u", st_target.az*RAD_TO_DEG ,st_target.alt*RAD_TO_DEG, st_target.ra*RAD_TO_DEG/15.0 ,st_target.dec*RAD_TO_DEG );
        printf("\033[s\033[6;4H CURRENT: %f %+f         %f %+f \033[K\r\n\033[u", st_current.az*RAD_TO_DEG ,st_current.alt*RAD_TO_DEG,st_current.ra*RAD_TO_DEG/15.0,st_current.dec*RAD_TO_DEG);
        printf("\033[s\033[7;4H DELTA :  %+f %+f          %+f %+f \033[K\r\n\033[u", d_az_r ,d_alt_r,(st_target.ra-st_current.ra)*RAD_TO_DEG/15.0,(st_target.dec-st_current.dec)*RAD_TO_DEG);

//

        usleep(100000);
    }
    while (1);

}
pthread_t  encoder_read_thread_create(void* mt)
{
    int t;
    pthread_t th;
    tak_init(mt);
    t=pthread_create(&th, NULL,track,mt);
    if (th)  printf("Read counters thread  running ID %d OK\r\n",t);
    else perror("Opps! thread create failed");
    return th;
}
pthread_t server_thread_create(void* t)
{
    pthread_t th;
    int tt= pthread_create(&th, NULL, sserver,t);
    if (th)  printf("TCP server thread  running ID %d OK\r\n",tt);
    else perror("Opps! thread create failed");
    return th;
}



int sync_ra_dec(mount_t *mt,double ra,double dec)
{
    // st_current.timer_count = ( sidereal_timeGMT (mt->longitude, mt->time_zone)*3600.0 );
    st_current.timer_count =chrono_read(&ti);
    st_current.dec= st_target.dec=dec;
    st_current.ra=st_target.ra=ra;
    to_alt_az(&st_current);

    setposition(mt->azmotor,st_current.az/mt->azmotor->resolution);

    if (st_current.alt>=0.0)
        setposition(mt->altmotor,st_current.alt/mt->altmotor->resolution);

    else
        setposition(mt->altmotor,( M_2PI+st_current.alt)/mt->altmotor->resolution);

}

int stop_mount(mount_t *mt,char direction)
{
    switch (direction)
    {
    case 'n':
    case's':
        mt->altmotor->targetspeed=0.0;
        break;

    case 'w':
    case 'e':
        mt->azmotor->targetspeed= SID_RATE*SEC_TO_RAD;
        break;

    default:
        mt->altmotor->targetspeed=0.0;
        mt->azmotor->targetspeed= SID_RATE*SEC_TO_RAD;
        break;
    };
}

void  tak_init(mount_t *mt)
{
    reset_transforms(0.0, 0.0, 0.0);
    double temp=sidereal_timeGMT (mt->longitude, mt->time_zone)*3600.0;
  chrono_set(&ti);
    if (mt->mount_mode == ALTAZ)
    {

        set_star(&st_target, sidereal_timeGMT (mt->longitude, mt->time_zone)*15 + 90.0, 0.0, 90.0, 0.0, 0);
        init_star(1, &st_target);
        set_star(&st_target,  sidereal_timeGMT (mt->longitude, mt->time_zone)*15, mt->lat, 180.00, 89.99, 0);
        init_star(2, &st_target);

    }
    else if (mt->mount_mode == EQ)
    {
        double ra    ;
        set_star(&st_target,  sidereal_timeGMT (mt->longitude, mt->time_zone)*15, 0.0, 180.0, 0.0, 0);
        init_star(1, &st_target);
        ra = st_target.ra + M_PI / 2.0;
        if (ra < 0) ra += M_2PI;
        if (mt->lat >= 0.0)
            set_star(&st_target, ra * RAD_TO_DEG, 45, 90, 45, 0);
        else
            set_star(&st_target, ra * RAD_TO_DEG, -45, 270, 45, 0);
        init_star(2, &st_target);


    }
    compute_trasform();
    set_star(&st_target,  sidereal_timeGMT (mt->longitude, mt->time_zone), 10.0, 0.0, 0.0, 0);
    to_alt_az(&st_target);
    //  is_aligned=0;
    //  is_slewing='0';
    //  counter_x=counter_y=0;

}

int goto_ra_dec(mount_t *mt,double ra,double dec)
{
    st_target.ra=ra;
    st_target.dec=dec;

}
