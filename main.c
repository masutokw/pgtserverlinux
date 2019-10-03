#include <stdio.h>
#include <stdlib.h>
#include "piclevel.h"
#include "mount.h"
#include "taki.h"
#include <math.h>
#include  "ir_remote.h"
#define COM_DEVICE "/dev/rfcomm1"
//"/dev/ttyS0"
#include "misc.h"
mount_t *mount_test;
c_star st_now, st_target, st_current;
extern void sserver(void *);
extern char port;
int main()
{
    float f,g;
    mount_test=create_mount();

    if  (initcom(&port)==-1) return 1 ;
    ir_init ();

    mount_test->th_motors=motor_speed_thread_create(mount_test);
    mount_test->th_encoders=encoder_read_thread_create(mount_test);
    mount_test->azmotor->targetspeed=0.0*SEC_TO_RAD;
    mount_test->altmotor->targetspeed=0.0*SEC_TO_RAD;
    clearScreen();
    sserver(NULL);
     closecom();
    destroy_mount(mount_test);

    return 0;
}
