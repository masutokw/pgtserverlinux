
#include <stdio.h>   /* Standard input/output definitions. */
#include <string.h>  /* String function definitions. */
#include <unistd.h>  /* UNIX standard function definitions. */
#include <fcntl.h>   /* File control definitions. */

#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <termios.h> /* POSIX terminal control definitions. */

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef BOOL
#define BOOL  int
#endif

#define INVALID_HANDLE_VALUE -1
#define NONE      0
#define RTSCTS    1
#define HARD      1
#define XONXOFF   2
#define SOFT      2



typedef  struct termios	DCB;
int Kbhit_Port(int fd);
int Close_Port(int fd);
int Open_Port(char COMx[]);
DCB Configure_Port(int fd,unsigned int BaudRate,char CharParity[]);
int Set_Time(int fd,unsigned int Time) ;
int Clean_Buffer(int fd);
long Write_Port(int fd,char Data[],int SizeData);
int IO_Blocking(int fd,int Modo);
int Set_Time(int fd,unsigned int Time);
long Read_Port(int fd,char *Data,int SizeData);

