/* send sctp packet for test*/
/* Copyright 2010-2022 . */
/*
modification history
--------------------
2010-12-08,jimx update
2010-08-1,jimx create
*/
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

#define LOG_PATH	"/opt/CTIR/log/"
#define LOG_FILE_SIZE_LIMIT 5*1024*1024
char progName[64]="unknown";
char* getTimeString()
{
	static char timeStr[32];
	time_t timep;
	struct tm *ptm;
	
	time(&timep);
	ptm = localtime(&timep);
	sprintf(timeStr,"%.4d%.2d%.2d_%.2d%.2d%.2d",\
		ptm ->tm_year + 1900, ptm ->tm_mon + 1, ptm ->tm_mday,\
		ptm ->tm_hour, ptm ->tm_min ,ptm ->tm_sec);	
	return timeStr;
}

#if 0
static FILE* fpOut=NULL;
void writeLogFile(char* inbuf,int num)
{
	static char sbuf[64];
	static int newf=1;
	int nmemb;
	
	if(newf) {
		if(access(LOG_PATH,R_OK)){
			mkdir(LOG_PATH,0700);
		}
		sprintf(sbuf,"%s%s_%s.log",LOG_PATH,progName,getTimeString());
		for(nmemb=1;//文件已存在，则添加下标来创建
			!access(sbuf,R_OK);
			nmemb++)
		{
			//printf(">>file exist,creat new file with subscript\n");
			sprintf(sbuf,"%s%s_%s_%d.log",LOG_PATH,progName,getTimeString(),nmemb);
		}
		if(!fpOut) fpOut=fopen(sbuf,"a+");
		newf=0;
	}
	if(!fpOut) fpOut=fopen(sbuf,"a+");
	nmemb=fwrite(inbuf,sizeof(char),num,fpOut);
	//printf(">>write %d byte to %s\n",nmemb,sbuf);
	//fseek(fpCapp, 0, SEEK_END);
	if(ftell(fpOut) > LOG_FILE_SIZE_LIMIT) {
		fclose(fpOut);
		fpOut=NULL;
		newf=1;
	}
	return;
}
#else //modified 20120107 to control log size
void writeLogFile(char* inbuf,int num)
{
#define FILE_NM_LEN	64
		static FILE* fpOut=NULL;
		static char sbuf[FILE_NM_LEN],preFile[FILE_NM_LEN],nowFile[FILE_NM_LEN];
		static int newf=1,fileNum=0;
		int nmemb;
		
		if(newf) {
			if(access(LOG_PATH,R_OK)){
				mkdir(LOG_PATH,0700);
			}
			sprintf(sbuf,"%s%s_%s.log",LOG_PATH,progName,getTimeString());
			for(nmemb=1;//文件已存在，则添加下标来创建
				!access(sbuf,R_OK);
				nmemb++)
			{
				//printf(">>file exist,creat new file with subscript\n");
				sprintf(sbuf,"%s%s_%s_%d.log",LOG_PATH,progName,getTimeString(),nmemb);
			}
			if(!fpOut) {
				if (++fileNum>2){
					fileNum=2;
					rename(preFile,sbuf);
				}			
				fpOut=fopen(sbuf,"w+");
			}
			strcpy(preFile,nowFile);
			newf=0;
		}
		if(!fpOut) fpOut=fopen(sbuf,"a+");
		nmemb=fwrite(inbuf,sizeof(char),num,fpOut);
		if(ftell(fpOut) > LOG_FILE_SIZE_LIMIT) {
			fclose(fpOut);
			fpOut=NULL;
			newf=1;
			strcpy(nowFile,sbuf);
		}

}
#endif
void log_printf(char *fmt, ...)
{
	static char logbuf[1024];
	va_list args;
	va_start(args, fmt);
	vsprintf(logbuf, fmt, args);//return n=strlen
	va_end(args);
	writeLogFile(logbuf,strlen(logbuf));
}


