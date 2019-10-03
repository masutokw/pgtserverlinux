#include "piclevel.h"
#include <math.h>
#include "motor.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
//#include "com/serial.h"
int sign(double t)
{
    if (t<0.0) return -1;
    else if (t==0.0 )return 0;
    else return 1;

}
double absd(double n)
{
    if (n<0.0) return -n;
    else return n;
}
double getposition(motor_t* mt)
{
    return mt->position;
}

void init_motor(motor_t* mt,char ref,int maxcounter)
{
    mt->position=0;
    mt->timertick=4e-7;
    mt->maxcounter=maxcounter;
    mt->resolution=(2.0* M_PI) /mt->maxcounter;
    mt->acceleration=20e-4;
    mt->id=ref;
    mt->slewing=0;
    mt->maxspeed= M_PI /45.0;

}

void setspeed(motor_t* mt ,double tspeed)
{
    int base,postscaler;//ispeed
    mt->current_speed=tspeed;
    base=0;    //timer2 preload
    postscaler=0;  //PIC timer2 iterations
    if (tspeed!=0.0)
    {
        base=fabs((mt->resolution)/( tspeed*(mt->timertick)));
        postscaler=base / 0xFFFF;
        if (postscaler > 0)
        {
            base/=(postscaler+1);
        }   ;
        postscaler++;
    }
    else base= 0xFFFF;
    motor_set_period (mt->id, 0xFFFF-base,sign(tspeed)*postscaler);
}

void setposition(motor_t* mt,int pos)
{
    set_motor_counter(mt->id,pos);
    mt->counter=pos;
}
void go_to(motor_t* mt,double position,double speed)
{}


void readcounter(motor_t* mt)
{
    int temp =readcounters(mt->id);
    if (temp>=0)
    {
        mt->position= mt->resolution*(mt->counter=temp);
        mt->delta= mt->position-mt->target;
    }
}
void readcounter_n(motor_t* mt)
{
    int temp =readcounters(mt->id);
    if (temp>=0)
    {
        mt->counter=temp;
        if (temp>(mt->maxcounter/2))
        {
            temp-=mt->maxcounter;
        }
        mt->position= mt->resolution*(temp);
        mt->delta= mt->position-mt->target;
    }

}
void setmaxcounter(motor_t* M,int value)
{
    set_motor_max_counter(M->id,value);
    M->maxcounter=value;
}
void settarget(motor_t* mt,int pos)
{
    set_motor_target(mt->id,pos);


}

void speed_up_down(motor_t* mt)

{
    if  (mt->speed!=mt->targetspeed)
    {
        if  (absd(mt->targetspeed-mt->speed)<absd(mt->acceleration))
        {
            mt->speed= mt->targetspeed;
        }

        if (mt->speed< mt->targetspeed) mt->speed=mt->speed+mt->acceleration;
        else if (mt->speed> mt->targetspeed)   mt->speed=mt->speed-mt->acceleration;
        setspeed(mt,mt->speed);
    }

}
void  setcounter(motor_t* mt,int count)
{}
void  loadconf(motor_t* mt,char* name) {}
//----------------------------------------------------------------
//THREAD control de velocidad /aceleración
//----------------------------------------------------------------

void settargetspeed(motor_t* mt, double tspeed)

{
    if (fabs(tspeed) <= mt->maxspeed) mt->targetspeed = tspeed;
    else mt->targetspeed=mt->maxspeed *sign (tspeed);
}
