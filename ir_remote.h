#ifndef IR_REMOTE_H_INCLUDED
#define IR_REMOTE_H_INCLUDED
#include <sys/file.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/input.h>
#include <pthread.h>
#define DEV_IR_EVENT "/dev/input/event3"
int fd, rd;
int ir_init (void);

#endif // IR_REMOTE_H_INCLUDED
