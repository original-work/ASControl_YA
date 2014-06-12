//============================================================================
//
//  MODULE NAME:           ASControl.h
//
//  MODULE DESCRIPTION:
//
//   
//
//
//  SPECIAL CONSIDERATIONS:
//
//
//****************************************************************************
//
//  REVISION HISTORY:
//
//  DATE      			DESIGNER      				REASON         
//  -------- 		    ------------ 	  --------------------------------------
//  09/05/2011  		JiMeixiang         				 Original
//  06/01/2012            WangXinXin                                 Modified
//
//============================================================================
#ifndef _ASControl_H
#define _ASControl_H
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <pthread.h> 
#include <errno.h> 
#include <sys/ipc.h> 

#include <fstream>
#include <netdb.h>
#include <net/if.h>
#include <sys/ioctl.h>



#include "se_thread.h"
#include "Program.H"
#include "AppTable.H"
#include "AppControl.H"
#include "SeAddr.H"
#include "SeMsg.H"
#include "Misc.H"
#include "se_error.h"
#include "SeIntf.H"

#include "CDbInterface.h"//shm interface
#include "Ss7BaseMsg.h"
#include "NnsfCommonDefs.h"
//for ptest_thread added by wangxx 20120827
#include "SeMsg.H"
#include "SeAddr.H"
#include <sys/time.h>
#include <time.h>

#include "ReadCfgFile.h"
#include "writeLogFile.h"
#include "Alarm.h"


//#include "SpRptAlarmReport.hh"
//#include "SpRptSubmission.hh"
/*******视具体情形填写*************/
#define BOARD_ETH	ETH2  //板上使用网口
#define	CFG_FILE_PATH	"/opt/CTIR/etc/config.cfg"
/******************************************/
#define SA      struct sockaddr
#define MAXLINE         4096    /* max text line length */

#define P_SVR_PORT	6001//primary(config active) 
#define S_SVR_PORT	6002
#define ASHB_HDR	0x0a0b0c0d
#define ACTIVE_BOARD 	"Active_Board"
#define STANDBY_BOARD 	"Standby_Board"
#define MAXBUF 1024
#define L_DNAR_ALIVE	0x01
#define DNAR_UPDATE	0x08
#define   TO_APP_PORT		5091
#define   LOCAL_DFLT_IP		"127.0.0.1"
#define   SOCK_INADDR_LEN	sizeof(struct sockaddr_in)
#define	MSU_HDR_SIZE		sizeof(S_MSU_HDR)
#define	DREVSYNC_CFG_FILE_PATH	"/opt/CTIR/etc/DRevSync.cfg"

extern Alarm* g_AlarmObjPtr;

typedef struct 
{
  	int header;		   // AS_HDR 0x0a0b0c0d
	int dIpAdrs;        /*destination IP*/
	int sIpAdrs;        /*source IP*/
	int hbSeq;
	int invoke;    
	int board_as;
	int dnar_alive;
	//char* pData;          /*start address of data area.*/
}HB4ASCTRL;

typedef struct
{
	int local_as;
	char local_ip[32];
	char peer_ip[32];
}LOCAL_CFG;


enum hb_type
{
	HB_SND = 1,
	HB_REPLY,
};
enum {
	IP_POS=0,
	AS_POS,
};

union ipu{
	long ip;
	unsigned char ipchar[4];
};

enum eth_sequence
{
	//127.0.0.1=0
	ETH0=1,
	ETH1,
	ETH2,
	ETH3,
	ETH4,
};

enum AS_TYPE//两块  CSP board 主备状态
{
	AS_PEND=0,
	AS_STANDBY,
	AS_ACTIVE,
	
};

const char AS_str[3][16]={
"AS_PEND",
"AS_STANDBY",
"AS_ACTIVE"
};



typedef struct{
	unsigned int duration_seconds;
}S_IGNORE_DNAR;

typedef struct{
	unsigned char msgId;
	union{
		   S_IGNORE_DNAR ignore;
		}msgbody;
}S_TEST_MSG;

enum MessageId
{
	begin,
	ignore_dnar,
	show_start_time,
	end,
};


/*for reporting peer-board linkset-status,add by jimx 20111213,begin*/
typedef struct
{
	UINT4 _msgseq;
	
	UINT2 _msgProvId;
   	UINT2 _destModule;
   	UINT2 _msgLen;
   	UINT2 _msgId;
}S_MSU_HDR;


class ASControl
{
	public:
		static ASControl* instance();
		void ignore_func(S_TEST_MSG* Msg);
		void SendDnarDeadAlarmResume();
		void show_time();
		long getlocalhostip(int eth);
		char* timeStamp();
		int reverseCfg(int config);
		int selectTcpPort (int loc_as);
		void SHM_updateAS(int as_state);
		void SHM_ASControl_init();
		void SendDnarDeadAlarm();
		void SHM_checkDnarAlive();
		void notify_as(int req_as);
		void readLocalConfig();
		void readDRevSyncConfig();
		void init_udp_sock(int &sock_snd, sockaddr_in &s_addr);
		void peerBoardDown();
		void err_doit(int errnoflag, const char *fmt, va_list ap);
		void err_sys(const char *fmt, ...);
		void err_quit(const char *fmt, ...);
		int Fcntl(int fd, int cmd, int arg);
		int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
		int connect_nonb(int sockfd, const SA *saptr, socklen_t salen, int nsec);
		void server_func();
		void test_func();
		void client_func();
		void autorun_register();
		LOCAL_CFG getlocalcfg();
		void setbasetime(time_t t);
		void setstarttime(time_t t);
		time_t getstarttime();
		void set_dnar_status(int v);
		void set_curr_as(int v);
		bool get_AlarmType();
		
	private:
		ASControl(void);
		static ASControl* _instance;
		time_t _ignore_dnar_duration;
		LOCAL_CFG _localcfg;//from config file
		time_t _start_time;
		time_t _base_time;
		const char* _app_name;//="ASControl_App";
		int _dnar_status;
		int _curr_as;//当前active/standby
		char* _business_string;
		AlarmInfo _Info;	
		CDbInterface* _dbIF;
};

#endif

