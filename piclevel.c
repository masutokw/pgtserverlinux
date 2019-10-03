#include "piclevel.h"
#include "serial.h"
//PICMSG msg = {  {0, 's', 'd'},0 };
#define ASGN msg.id = msg.idd= id;msg.command

picmsg  msg= {CMD_FCODE,0xFE,0xFE,0xFF,0,0};
int com_handler=0;
//pthread_t thr;
int h,v;
int
readcounters (char id)
{
    PICRSP rsp;

    int  count,countr;
    ASGN = COUNTERS_GET_CMD;
    Clean_Buffer(com_handler);
    Write_Port(com_handler,(char *) &msg,4);
    // usleep(10000);
    countr=0;
    count = 9;
// countr=read(com_handler,(char *)&rsp,count);
    countr=Read_Port(com_handler,(char *)&rsp,count);
    if ((countr==9)&&(rsp.control=RESPONSE_OK))
    {
        // printf("com  %d %x \n\r",countr,id);
        return rsp.counter;
    }

    else
    {
        printf("com error %d %d \n\r",countr,id);
        //   Clean_Buffer(com_handler);
        return -1;
    }
    Clean_Buffer(com_handler);
};

char *
motor_set_period (char id, int base, char post)
{
    picmsg  msg;
    msg.code=(CMD_FCODE);
    ASGN = M1_SPEED_SET_CMD;
    //msg1.id = msg1.idd= id;msg1.command=M1_SPEED_SET_CMD;
    msg.data.dint = base;
    msg.data.buff[2]=post;
    msg.len=7;
    //sendcommand();
    int n=write(com_handler,&msg,7);
//  return msg.data.buff;

}

char *
aux_set_period (char id, int base)
{
    ASGN = M2_SPEED_SET_CMD;
    msg.data.dint = base;
    msg.len=6;
    sendcommand();
    return msg.data.buff;

}
char *
set_motor_target (char id, int base)
{
    ASGN = M1_SET_CMD_TARGET;
    msg.data.dint = base;

    msg.len=8;
    sendcommand();
    return msg.data.buff;
}
char *
set_aux_target (char id, int base)
{
    ASGN = M2_SET_CMD_TARGET;
    msg.data.dint = base;
    msg.len=8;
    sendcommand();
    return msg.data.buff;
}
char* save_counters(char id)
{
    ASGN = COUNTERS_SAVE_CMD;
    msg.len=4;
    sendcommand();
    return msg.data.buff;

}
char*
setmotorbackslash(char id,int back)
{
    ASGN = M1_SET_BACKSLASH_CMD;
    msg.data.dint = back;
    msg.len=6;
    sendcommand();
    return msg.data.buff;
}
char*
setauxbackslash(char id,int back)
{
    ASGN = M2_SET_BACKSLASH_CMD;
    msg.data.dint = back;
    msg.len=6;
    sendcommand();
    return msg.data.buff;
}
char *
changePWM(char id,unsigned char index ,char value)
{
    ASGN = M1_CHANGE_PMW_CMD;
    msg.data.buff[0] = index;
    msg.data.buff[1] =value;
    sendcommand();
    msg.len=6;
    return msg.data.buff;
}

char*
changespeedtable(char id,int value, unsigned char index)
{
    ASGN = SPEED_TABLE_CHANGE_CMD;
    msg.data.dint=value;
    msg.data.buff[2] = index;

    msg.len=7;
    sendcommand();
    return msg.data.buff;
}
char* saveconf(char id)

{
    ASGN = SPEED_TABLE_SAVE_CMD;
    msg.len=4;
    sendcommand();
    return msg.data.buff;
}
char*
set_motor_max_counter(char id,unsigned int max)
{
    ASGN = M1_MAX_COUNT_SAVE_CMD;
    msg.data.dint =max;
    msg.len=8;
    sendcommand();
    return msg.data.buff;
}

char*
set_aux_max_counter(char id,unsigned int max)
{
    ASGN = M1_MAX_COUNT_SAVE_CMD;
    msg.data.dint =max;
    msg.len=8;
    sendcommand();
    return msg.data.buff;
}
char*
set_motor_counter(char id, int current)
{
    ASGN = M1_COUNTER_SET_CMD;
    msg.data.dint =current;
    msg.len=8;
    sendcommand();
    return msg.data.buff;

}
char*
set_aux_counter(char id,unsigned int current)
{
    ASGN = M2_COUNTER_SET_CMD;
    msg.data.dint =current;
    msg.len=8;
    sendcommand();
    return msg.data.buff;
}
char*
set_start_up(char id)
{
    ASGN = M1_STARTUP_SET_CMD;
    msg.len=4;
    sendcommand();
    return msg.data.buff;
}
char*
unset_start_up(char id)
{
    ASGN = M1_STARTUP_UNSET_CMD;
    msg.len=4;
    sendcommand();
    return msg.data.buff;
}
char*
set_baud_rate(char id,char baud)
{
    ASGN = BAUDRATE_SET;
    msg.data.buff[0] = baud;
    msg.len=5;
    sendcommand();
    return msg.data.buff;
}
char*
set_prescaler(char id,char prescaler)
{
    ASGN = PRESCALER_SET;
    msg.data.buff[0] =  prescaler;
    msg.len=5;
    sendcommand();
    return msg.data.buff;
}

int initcom(char* a )
{
    char var[] = "8N1";
    if (com_handler==0)
    {
        com_handler=Open_Port(a);
        Configure_Port(com_handler,B19200,var);
        IO_Blocking(com_handler,TRUE);
        Set_Time(com_handler,0);
    }
    return  com_handler;
}
int sendcommand(void)
{
    int n=Write_Port(com_handler,(char *) &msg,msg.len);
    if (n!=msg.len) printf("send %d\r\n",n);
    //msg.len=0;

    return n;

}
int sendcommandt(picmsg msg)
{
    int n=Write_Port(com_handler,(char *) &msg,msg.len);
    //if (n!=msg.len) printf("send %d\r\n",n);
    //msg.len=0;
    return n;

}
void closecom(void)
{
    Close_Port(com_handler);
}



