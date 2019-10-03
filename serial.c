#include "serial.h"
//#define FIONREAD  0x541B
//BOOL ERROR_CONFIGURE_PORT=FALSE;
int Kbhit_Port(int fd)
{
    int bytes;
    ioctl(fd, FIONREAD, &bytes);
    return bytes;

}

int Close_Port(int fd)
{
    if (fd != INVALID_HANDLE_VALUE)
    {

        if (close(fd)!=0)
        {
            printf("Error closing serial port\n");
            return FALSE;
        }
        else
        {
            fd = INVALID_HANDLE_VALUE;
            return TRUE;
        }
    }
    return FALSE;
}
int Open_Port(char COMx[])
{
    int fd; // File descriptor for the port.

    fd = open(COMx, O_RDWR);// | O_NOCTTY );//| O_NDELAY);
//   fd = open(COMx, O_RDWR | O_NOCTTY | O_NDELAY);


    if (fd <0)
    {
        printf("open_port:fd=%d: cannot open port %s\n",fd,COMx);
        return INVALID_HANDLE_VALUE;
    }
    printf("open_port:fd=%d:  port opened %s\n",fd,COMx);
    tcflush(fd, TCIOFLUSH);
    return (fd);
}
DCB Configure_Port(int fd,unsigned int BaudRate,char CharParity[])
{
    DCB newtio;
    bzero(&newtio, sizeof(newtio));    //limpiamos struct para recibir los
    //nuevos parámetros del puerto.
    //tcflush(fd, TCIOFLUSH);

    //CLOCAL  : conexion local, sin control de modem.
    //CREAD   : activa recepcion de caracteres.
    newtio.c_cflag |=(CLOCAL | CREAD) ;

    cfsetispeed(&newtio,BaudRate);
    cfsetospeed(&newtio,BaudRate);

    if(strncmp(CharParity,"8N1",3)==0)	//CS8     : 8n1 (8bit,no paridad,1 bit de parada)
    {
        newtio.c_cflag &= ~PARENB;
        newtio.c_cflag &= ~CSTOPB;
        newtio.c_cflag &= ~CSIZE;
        newtio.c_cflag |= CS8;
    }


    // IGNPAR  : ignora los bytes con error de paridad.
    // ICRNL   : mapea CR a NL (en otro caso una entrada CR del otro ordenador
    // no terminaría la entrada) en otro caso hace un dispositivo en bruto
    // (sin otro proceso de entrada).
    newtio.c_iflag = 0;
    //newtio.c_iflag = IGNPAR;
    //newtio.c_iflag |= ICRNL;

    //Salida en bruto.
    newtio.c_oflag = 0;

    //ICANON  : activa entrada canónica(Modo texto).
    //desactiva todas las funcionalidades del eco, y no envía señales al
    //programa llamador.
    //newtio.c_lflag = ICANON;
    newtio.c_lflag = 0;

    // inicializa todos los caracteres de control.
    // Los valores por defecto se pueden encontrar en /usr/include/termios.h,
    // y vienen dadas en los comentarios, pero no los necesitamos aquí.

    newtio.c_cc[VTIME]    = 0;     /* temporizador entre caracter, no usado */
    newtio.c_cc[VMIN]     = 1;     /* bloquea lectura hasta llegada de un caracter */

    if(tcsetattr(fd,TCSANOW,&newtio)!=0)
    {
        printf("ERROR: No se pudo poner la configuración del puerto serie\n" );
//        ERROR_CONFIGURE_PORT=TRUE;
        return newtio;
    }

    return newtio;
}
int Clean_Buffer(int fd)
{
    if(tcflush(fd, TCIFLUSH)!=0)
    {
        printf("Error Limpiando el Buffer  de entrada y salida.\n");
        return FALSE;
    }
    return TRUE;
}

long Write_Port(int fd,char Data[],int SizeData)
{
    return  write(fd,Data,SizeData);
}
int IO_Blocking(int fd,int Modo)
{
    struct termios newtio;
    /* almacenamos la configuracion actual del puerto */
    if(tcgetattr(fd,&newtio)!=0)
    {
        printf("Error obteniendo configuración time-out actual\n");
        return FALSE;
    }

    if(Modo==FALSE)
    {
        newtio.c_cc[VTIME]    = 5;     /* Temporizador entre caracter.*/
        newtio.c_cc[VMIN]     = 0;     /* No bloquea lectura hasta llegada de un caracter. */
    }
    if(Modo==TRUE)
    {
        newtio.c_cc[VTIME]    = 0;     /* Temporizador entre caracter.*/
        newtio.c_cc[VMIN]     = 1;     /* Bloquea lectura hasta llegada de un caracter. */
    }

    if(tcsetattr(fd, TCSANOW, &newtio)!=0)
    {
        printf("Error estableciendo nueva configuración bloqueante/no-bloqueante.\n");
        return FALSE;
    }

    return TRUE;
}
int Set_Time(int fd,unsigned int Time) //t =Time*0.1 s)
{
    struct termios newtio;
    /* almacenamos la configuracion actual del puerto */
    if(tcgetattr(fd,&newtio)!=0)
    {
        printf("Error obteniendo configuración time-out actual\n");
        return FALSE;
    }

    newtio.c_cc[VTIME]    = Time;/*temporizador entre caracter*/
    newtio.c_cc[VMIN]     = 1;   /*bloquea lectura hasta llegada de un
                                       caracter  */

    if(tcsetattr(fd, TCSANOW, &newtio)!=0)
    {
        printf("Error estableciendo nueva configuración time-out\n");
        return FALSE;
    }

    return TRUE;
}
long Read_Port(int fd,char *Data,int SizeData)
{
    struct termios newtio;
    struct timeval inic,fin,temp;
    float tiempo,t;
    int bytes;
    int ESTADO;
    int TEMPO;

    if(tcgetattr(fd,&newtio)!=0) return -1;
    else
    {
        ESTADO=newtio.c_cc[VMIN];
        TEMPO=newtio.c_cc[VTIME];
        if( (ESTADO==0)&&(TEMPO==0) )
        {
            return read(fd,Data,SizeData);
        }
        else
        {
            if(TEMPO==0)
            {
                do
                {
                    ioctl(fd, FIONREAD, &bytes);
                }
                while(bytes<SizeData);

                return read(fd,Data,SizeData);
            }
            else
            {
                gettimeofday(&inic,NULL);
                tiempo=newtio.c_cc[VTIME];
                do
                {
                    gettimeofday(&fin,NULL);
                    temp.tv_sec=fin.tv_sec-inic.tv_sec;
                    temp.tv_usec=fin.tv_usec-inic.tv_usec ;
                    t=((temp.tv_usec/1000.0)+temp.tv_sec*1000.0)/100.0;
                    usleep(1000);
                }
                while( (t<tiempo*SizeData) && (Kbhit_Port(fd)<SizeData) );

                if(Kbhit_Port(fd)!=0)	return read(fd,Data,SizeData);
                else			return 0;
            }
        }
    }
}
