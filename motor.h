#include <math.h>
typedef struct
{
    double
    position,       //angle radians
    target,         //angle radians
    delta,          //angle radians
    resolution,     //mstep size radians
    targetspeed,    //angular speed rad/s
    current_speed,  //angular speed rad/s
    acceleration,   //angular acc rad/s
  //  maxrange, min_range,
    timertick,//microcontroller timertick period
    speed,//angular speed rad/s
    maxspeed;//angular speed rad/s
    int counter,maxcounter,
   // range,
    slewing;
    char id;
} motor_t;
//#define M_PI 3.14159265359
#define SEC_TO_RAD M_PI/(3600.0*180.0)


//int fport;

int sign(double t);
void init_motor(motor_t* mt,char ref,int maxcounter);
double getposition(motor_t* mt);
void  setspeed(motor_t* mt,double tspeed);
void  setmaxcounter(motor_t* M,int value);
void  readcounter(motor_t* mt);
void  readcounter_n(motor_t* mt);
void  setposition(motor_t* mt,int pos);
void  settarget(motor_t* mt,int pos);
void  go_to(motor_t* mt,double position,double speed);
void  speed_up_down(motor_t* mt);
void setcounter(motor_t* mt,int count);
void loadconf(motor_t* mt,char* name);
void settargetspeed(motor_t* mt, double tspeed);








