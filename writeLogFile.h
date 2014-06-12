
#ifndef writeLogFile_h
#define	writeLogFile_h
#include <sys/socket.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
extern char progName[64];
extern void log_printf(char *fmt, ...);
#define printf log_printf
#endif
