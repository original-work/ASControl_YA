//============================================================================
//
//  MODULE NAME:           as_control.cpp
//
//  MODULE DESCRIPTION:
//					   switch Primary/Standby for two cspBoards
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
//  09/03/2011  		JiMeixiang         				 Original
//  06/01/2012            WangXinXin                                 Modified
//
//============================================================================

#include "ASControl.h"

//extern char* getTimeString();
/********************************/
//void* pserver_thread(void *arg);
//void* pclient_thread(void *arg);

/*added by wangxx begin 20120827 20:37*/


ASControl* ASControl::_instance = NULL;

ASControl::ASControl()
{
	_ignore_dnar_duration=2*60;
	_app_name="ASControl_App";
	_dnar_status=0;
	_curr_as=0;
	memset((void*)&_Info,0,sizeof(AlarmInfo));
	_dbIF=CDbInterface::instance();
}

ASControl* ASControl::instance()
{
  	if (_instance == NULL)
  	{
    		_instance = new  ASControl();
 	}
  	return (_instance);
}

bool ASControl::get_AlarmType()
{
	if(0==strcmp(_business_string,"OUTVISIT"))
	{
		strcpy(_Info.C2_AlarmGroup,"6302");
		strcpy(_Info.C3_AlarmType,"6302704");
	}
	else if(0==strcmp(_business_string,"INVISIT"))
	{
		strcpy(_Info.C2_AlarmGroup,"6303");
		strcpy(_Info.C3_AlarmType,"6303704");
	}
	else if(0==strcmp(_business_string,"AUTOROAM"))
	{
		strcpy(_Info.C2_AlarmGroup,"6304");
		strcpy(_Info.C3_AlarmType,"6304704");
	}
	else
	{
		printf("business_string wrong! _business_string is %s\n", _business_string);
		return false;
	}
	printf("Info.C3_AlarmType is %s\n", _Info.C3_AlarmType);
	return true;
}


void ASControl::ignore_func(S_TEST_MSG* Msg)
{
	struct tm *tblock;
	_ignore_dnar_duration=Msg->msgbody.ignore.duration_seconds;
	_base_time=time(NULL);
	tblock = localtime(&_base_time);
	printf("_ignore_dnar_duration is %d\n",_ignore_dnar_duration);
	printf("_base_time is: %s\n",asctime(tblock));
}

void ASControl::show_time()
{
	struct tm *tblock;
	tblock = localtime(&_start_time);
	printf("system start time is: %s\n",asctime(tblock));	
}
/*added by wangxx end 20120827 20:37*/

long ASControl::getlocalhostip(int eth)
{
	int  MAXINTERFACES=16;
	unsigned long ip = 0;
	int fd, intrface = 0;
 	struct ifreq buf[MAXINTERFACES]; ///if.h
	struct ifconf ifc; ///if.h
	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) //socket.h
	{
 		ifc.ifc_len = sizeof buf;
 		ifc.ifc_buf = (caddr_t) buf;
 		if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc)) //ioctl.h
		{
 			intrface = ifc.ifc_len / sizeof (struct ifreq);
			if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[eth])))
			{
				ip=inet_addr( inet_ntoa( ((struct sockaddr_in*)(&buf[eth].ifr_addr))->sin_addr) );//types
				//printf("^^^^^^^^^^get ip:%lx\n", ip);
			}
			else {
				printf("ERROR:appointed network card[eth%d] abnormal\n",(eth-1));
			}
		}
 		close (fd);
 	}
	return ip;
}

char* ASControl::
timeStamp()
{
//#include <sys/time.h>
//#include <time.h>
	static char s_tmstamp[64];
	struct timeval tv;
	time_t timep;
	struct tm *ptm;
	
	time(&timep);
	ptm = localtime(&timep);
	gettimeofday(&tv, NULL);
	sprintf(s_tmstamp,"timeStamp[%.4d/%.2d/%.2d %.2d:%.2d:%.2d(%ld)]",\
		ptm ->tm_year + 1900, ptm ->tm_mon + 1, ptm ->tm_mday,\
		ptm ->tm_hour, ptm ->tm_min ,ptm ->tm_sec, tv.tv_usec);		
	return s_tmstamp;
}
int ASControl::reverseCfg(int config)
{
	if(AS_ACTIVE == config) {
		return AS_STANDBY;
	}
	else {
		return AS_ACTIVE;
	}
}
int ASControl::selectTcpPort (int loc_as)
{
	if (AS_ACTIVE == loc_as) {
		return S_SVR_PORT;
	}
	else {
		return P_SVR_PORT;
	}
}

/*access shared memory interface*/
void ASControl::SHM_updateAS(int as_state)
{
	TsASControl* p_asTbl;
	usleep(100);/*syscall gap*/
	if(!_dbIF->TransASControl(&p_asTbl)) {
		printf("TransASControl fail,in SHM_updateAS(),prog exit\n");
		exit(1);
	}
	p_asTbl->d_as_state = as_state;
	printf("updated AS to %s at %s\n",AS_str[as_state],timeStamp());
}
void ASControl::SHM_ASControl_init()
{
	TsASControl* p_asTbl;
	if(!_dbIF->TransASControl(&p_asTbl)) {
		printf("TransASControl fail,in SHM_checkDnarAlive(),prog exit\n");
		exit(1);
	}
	p_asTbl->d_as_state = _curr_as;
	p_asTbl->d_dnar_refresh = 0;
	strcpy(p_asTbl->d_local_ip,_localcfg.local_ip);
	strcpy(p_asTbl->d_peer_ip,_localcfg.peer_ip);
	printf("SHM_ASControl_init ok\n");
}

void ASControl::SendDnarDeadAlarm()
{
	strcpy(_Info.C7_HostIp,_localcfg.local_ip);
	strcpy(_Info.C8_AlarmPortId,"0");
	strcpy(_Info.C13_AlarmLevel,"1");
	strcpy(_Info.C18_AlarmSwitch,"1");
	strcpy(_Info.C20_AlarmContent,_business_string);
	strcat(_Info.C20_AlarmContent," DEAD");

	g_AlarmObjPtr->SendAlarm(&_Info);
}

void ASControl::SendDnarDeadAlarmResume()
{	
	strcpy(_Info.C7_HostIp,_localcfg.local_ip);
	strcpy(_Info.C8_AlarmPortId,"0");
	strcpy(_Info.C13_AlarmLevel,"1");
	strcpy(_Info.C18_AlarmSwitch,"2");
	strcpy(_Info.C20_AlarmContent,_business_string);
	strcat(_Info.C20_AlarmContent," DEAD RESUME");

	g_AlarmObjPtr->SendAlarm(&_Info);
}

void ASControl::SHM_checkDnarAlive()
{
#define CHECK_TIMES	18
	static int last_rd=0,tms=0;
	static time_t first_time=0,check_time=0;
	int now_rd;
	TsASControl* p_asTbl;
	usleep(100);
	if(!_dbIF->TransASControl(&p_asTbl)) 
	{
		printf("TransASControl fail,in SHM_checkDnarAlive(),prog exit\n");
		exit(1);
	}
	//if(!(now_rd =  p_asTbl->d_dnar_refresh)) return;

	now_rd =  p_asTbl->d_dnar_refresh;
	printf("now_rd=%d, last_rd=%d\n", now_rd, last_rd);
	printf("_dnar_status=%d\n", _dnar_status);
	if (last_rd != now_rd)
	{
		tms = 0;
		last_rd = now_rd;
		printf("(_dnar_status&L_DNAR_ALIVE)=%d\n",(_dnar_status&L_DNAR_ALIVE));
		if (!(_dnar_status&L_DNAR_ALIVE))
		{
			printf("LOCAL DNAR RESUME detected.%s\n",timeStamp());
			SendDnarDeadAlarmResume();
			
			_dnar_status|=L_DNAR_ALIVE;
			_dnar_status|=DNAR_UPDATE;
		}
	}
	else 
	{
		printf("tms=%d\n", tms);
		//第一次检测到
		if(1==++tms){
			first_time=time(NULL);
			check_time=first_time;}
		else{check_time=time(NULL);}
		printf("(check_time-first_time)=%d\n",(check_time-first_time));
		if((check_time-first_time)> CHECK_TIMES) 
		{
			if (_dnar_status&L_DNAR_ALIVE) 
			{
				printf("LOCAL DNAR_DEAD detected.%s\n",timeStamp());
				SendDnarDeadAlarm();

				_dnar_status&=~L_DNAR_ALIVE;
				_dnar_status|=DNAR_UPDATE;
			}
			//tms = CHECK_TIMES;
			tms=0;//replaced by wangxx 20120825
		}
	}
	return;
}
/**/

void ASControl::notify_as(int req_as)
{
	if (_curr_as == req_as) {
		return;
	}

	if((AS_STANDBY==_curr_as)&&(AS_ACTIVE==req_as))
	{
		cout << "notify_as  alarm !!" << endl;
	}
	
	SHM_updateAS(req_as);
	_curr_as = req_as;

	cout << "notify_as!!" << endl;
}

void ASControl::readLocalConfig()
{
	_LINE_ITEMS* pitems=itemsptr();
	char* itemstr;
	char* lstr;
	int i,totalline,find;
	union ipu eth_ip;
	printf("..reading AS config file\n");
	totalline = readCfgItems(CFG_FILE_PATH);
	eth_ip.ip = getlocalhostip(BOARD_ETH);
	sprintf(_localcfg.local_ip,"%u.%u.%u.%u",eth_ip.ipchar[0],eth_ip.ipchar[1],eth_ip.ipchar[2],eth_ip.ipchar[3]);
	printf("local board ip:%s\n",_localcfg.local_ip);
	//通过本地IP 来读取本机配置
	find=0;
	for (i=0;i<=totalline;i++) {
		itemstr = pitems[i].item[IP_POS].content;
		if (!strcmp(itemstr,_localcfg.local_ip)) {
			itemstr = pitems[i].item[AS_POS].content;
			if (!strcmp(itemstr,ACTIVE_BOARD)) {
				printf("local board config:AS_ACTIVE\n");
				_localcfg.local_as=AS_ACTIVE; 
			}
			else {
				printf("local board config:AS_STANDBY\n");
				_localcfg.local_as=AS_STANDBY; 
			}
			find=1;
			break;
		}
	}
	if (!find) {
		printf("ERROR WARNING:can't find item[%s],pls check cfg file\n", _localcfg.local_ip);
		exit(0);
	}
	//通过active/standby获取对端机器IP
	if (AS_ACTIVE == reverseCfg(_localcfg.local_as)) {
		lstr=ACTIVE_BOARD;
	}
	else {
		lstr=STANDBY_BOARD;
	}
	find=0;
	for (i=0;i<=totalline;i++) {
		itemstr = pitems[i].item[AS_POS].content;
		if (!strcmp(itemstr,lstr)) {
			strcpy(_localcfg.peer_ip,pitems[i].item[IP_POS].content);
			find=1;
			break;
		}
	}
	if (!find) {
		printf("ERROR WARNING:can't find item[%s],pls check cfg file\n",lstr);
		exit(0);
	}
}


void ASControl::readDRevSyncConfig()
{
	int ret=0;
	_LINE_ITEMS* pitems=itemsptr();
	printf("..reading DRevSync config file\n");
	ret=readCfgItems(DREVSYNC_CFG_FILE_PATH);
	if(ret>=0)
	{
		_business_string = pitems[1].item[IP_POS].content;
	}
}

void ASControl::init_udp_sock(int &sock_snd, sockaddr_in &s_addr)
{
	if (sock_snd) {
		return;
	}
	/*1.init send socket,UDP*/
	if ((sock_snd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
	{
		perror("UDP socket");
		printf("UDP socket sterror:%s\n",strerror(errno));
		exit(errno);
	} 
	memset(&s_addr, 0, SOCK_INADDR_LEN);
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(TO_APP_PORT);
	s_addr.sin_addr.s_addr = inet_addr(LOCAL_DFLT_IP);
	printf("init UDP sock ok\n");
}
void ASControl::peerBoardDown()
{	
	static int sock_snd=0;
	static struct sockaddr_in s_addr;//for send
	int len;
	char sndbuf[128];
	init_udp_sock(sock_snd, s_addr);
	S_MSU_TABLE* pMsu;//#include "NnsfCommonDefs.h"
	memset(sndbuf, 0, MSU_HDR_SIZE);
	pMsu= (S_MSU_TABLE*)sndbuf;
	pMsu->_msgLen=MSU_HDR_SIZE;
	pMsu->_msgId=MATE_BOARD_DEAD;//#include "Ss7BaseMsg.h"
	len = sendto(sock_snd, sndbuf, MSU_HDR_SIZE, 0, (struct sockaddr *)&s_addr, SOCK_INADDR_LEN);
	printf("%s.",timeStamp());
	if (MSU_HDR_SIZE==len) {
		printf("UDP:sendto %s:%d,%d(bytes) ok\n", inet_ntoa(s_addr.sin_addr), ntohs(s_addr.sin_port), len);
	}
	else {
		printf("UDP:send %d(bytes) to %s,%u fail[%s]\n",len, inet_ntoa(s_addr.sin_addr), ntohs(s_addr.sin_port),strerror(errno));
	}
}
/*add by jimx 20111213,end*/


//added by wangxx 20120809 23:58 begin


//static
void ASControl::err_doit(int errnoflag, const char *fmt, va_list ap)
{
	int errno_save;
	char	buf[MAXLINE];

	errno_save = errno;		/* value caller might want printed */
	vsprintf(buf, fmt, ap);
	if (errnoflag)
		sprintf(buf+strlen(buf), ": %s", strerror(errno_save));
	strcat(buf, "\n");
	fflush(stdout);		/* in case stdout and stderr are the same */
	fputs(buf, stderr);
	fflush(stderr);		/* SunOS 4.1.* doesn't grok NULL argument */
	return;
}


void ASControl::err_sys(const char *fmt, ...)
{
	va_list  ap;

	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	exit(1);
}

void ASControl::err_quit(const char *fmt, ...)
{
	va_list  ap;

	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
	exit(1);
}


int ASControl::Fcntl(int fd, int cmd, int arg)
{
	int	n;

	if ( (n = fcntl(fd, cmd, arg)) == -1)
		err_sys("fcntl error");
	return(n);
}

int ASControl::Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	int n;

	if ( (n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0)
		err_sys("select error");
	return(n);		/* can return 0 on timeout */
}

int ASControl::connect_nonb(int sockfd, const SA *saptr, socklen_t salen, int nsec) 
{ 
	int flags, n, error; 
	socklen_t len; 
	fd_set rset, wset; 
	struct timeval tval; 

	flags = Fcntl(sockfd, F_GETFL, 0); 
	Fcntl(sockfd, F_SETFL, flags | O_NONBLOCK); 

	error = 0; 
	if((n = connect(sockfd, saptr, salen)) < 0) 
	if (errno != EINPROGRESS) 
	return (-1); 

	/* Do whatever we want while the connect is taking place. */ 

	if (n == 0) 
	goto done; /* connect completed immediately */ 

	FD_ZERO(&rset); 
	FD_SET(sockfd, &rset); 
	wset = rset; 
	tval.tv_sec = nsec; 
	tval.tv_usec = 0; 

	if ( (n = Select(sockfd + 1, &rset, &wset, NULL, nsec ? &tval : NULL)) == 0) 
	{ 
		close(sockfd); /* timeout */ 
		errno = ETIMEDOUT; 
		return (-1); 
	} 

	if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) 
	{ 
		len = sizeof(error); 
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) 
		return (-1); /* Solaris pending error */ 
	} 
	else 
	err_quit("select error: sockfd not set"); 

done: 
	Fcntl(sockfd, F_SETFL, flags); /* restore file status flags */ 

	if (error) 
	{ 
		close(sockfd); /* just in case */ 
		errno = error; 
		return (-1); 
	} 
	return (0); 
}
//added by wangxx 20120809 23:59 end
void ASControl::server_func()
{
#define	HB_FAIL_EX		10	
#define	HB_INTERVAL 	500*1000
	int lisfd=0;
	int new_fd=0;
	socklen_t len;
	struct sockaddr_in my_addr, their_addr;
	unsigned int myport, lisnum;
	char buf[MAXBUF + 1];
	int ret;
	int hc;
	struct timeval tvl, tva;
	time_t time_now;//current time
	fd_set inset;
	int response_fail = 0;
	//int yes = 1;
	myport = selectTcpPort(_localcfg.local_as);
	lisnum = 1;
	HB4ASCTRL* hb_ptr;
	int reportPBD=0;//report peer board down 
	
	if ((lisfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("S:socket");
		printf("S:socket sterror:%s\n",strerror(errno));
		exit(1);
	}
	else printf("S:socket created\n");
	//setsockopt(lisfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	
	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = PF_INET;
	my_addr.sin_port = htons(myport);
	my_addr.sin_addr.s_addr = INADDR_ANY;

	// lose the pesky "address already in use" error message
	int sock_opt=1;
	if (setsockopt(lisfd, SOL_SOCKET, SO_REUSEADDR, &sock_opt, sizeof(int))< 0) {//add by jimx 20111031
		perror("S:setsockopt");
		printf("S:setsockopt sterror:%s\n",strerror(errno));
		exit(1);
	}
	if (bind(lisfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("S:bind");
		printf("S:bind sterror:%s\n",strerror(errno));
		exit(1);
	}
	else printf("S:binded\n");
	
	if (listen(lisfd, lisnum) == -1) 
	{
		perror("S:listen");
		printf("S:listen sterror:%s\n",strerror(errno));
		exit(1);
	}
	else {
		printf("S:begin listen ");
		printf("at local port[%d]\n", ntohs(my_addr.sin_port));
	}

	while(1) 
	{
		for (;;) 
		{
RECONNECT:

			time_now=time(NULL);
			/*进程起来一个半小时后再根据DNAR  进程的状态上报告警added by wangxx 20120813
                      当系统只有单板运行的时候程序走不出这个for  循环，所以要在这里加
                      一个对DNAR  的检测，检测到DNAR  死掉的时候要上报告警*/
			if((time_now-_base_time)<_ignore_dnar_duration)
			{
				printf("time_now-_base_time=%d\n", time_now-_base_time);	
			}                      
			else
			{
				printf("%s,%d\n",__FILE__, __LINE__);
				SHM_checkDnarAlive();
			}


			tvl.tv_sec=4;  
			tvl.tv_usec=0;
			FD_ZERO(&inset);
			FD_SET(lisfd, &inset);
			usleep(100);
			ret = select(lisfd+1, &inset, NULL, NULL, &tvl);
			if(-1 == ret)
			{
				sleep(1);
				perror("S:select");
				printf("S:select sterror:%s\n",strerror(errno));
				break;//
			}
			else if(0 == ret)
			{
				printf("S:LISTEN timeout(4s)\n");
				notify_as(AS_ACTIVE);
				if (!reportPBD) {
					reportPBD=1;
					peerBoardDown();
				}
				continue;
			}
			len = sizeof(struct sockaddr);
			if ((new_fd = accept(lisfd, (struct sockaddr *)&their_addr, &len)) == -1)
			{
				perror("S:accept");
				printf("S:accept sterror:%s\n",strerror(errno));
				continue;
			}
			else
			{
				printf("S: got connection from %s, port %d, socket %d.%s\n",inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port), new_fd, timeStamp());
				reportPBD=0;
				break;
			}
		}
		_dnar_status|=DNAR_UPDATE;//add by jimx 20111028
		response_fail = 0;
		hc = 0;
		for (;;) 
		{	
			/*************send hb***************/
SND_HB:
			usleep(HB_INTERVAL);
			bzero(buf, MAXBUF);
			hb_ptr = (HB4ASCTRL*)buf;
			hb_ptr->header = ASHB_HDR;
			hb_ptr->invoke = HB_SND;
			hb_ptr->hbSeq = ++hc;
			len = send(new_fd, buf, sizeof(HB4ASCTRL), 0);
			if(len < 0)
			{
				printf("S:消息'%s'发送失败！错误代码是%d，错误信息是'%s'\n", buf, errno,strerror(errno));
				notify_as(AS_ACTIVE);
				close(new_fd);
				goto RECONNECT;
			}
			else
			{
				//printf("S:send hb[%d] ok\n",hc);
				//printf("S:消息'%s'发送成功，共发送了%d个字节！\n", buf, len);
			}
			
			/*************wait reply***************/
			while(1){
				tva.tv_sec=2;     
				tva.tv_usec= 0;
				FD_ZERO(&inset);
				FD_SET(new_fd, &inset);
				ret = select(new_fd+1, &inset, NULL, NULL, &tva);
				if(-1 == ret) 
				{
					sleep(1);
					perror("S:select");
					printf("S:select sterror:%s\n",strerror(errno));
					continue;
				}
				else if(0 == ret)
				{
					printf("S:hb response timeout\n");
					if (++response_fail > HB_FAIL_EX) 
					{
						printf("S:heartbeat fail exceed limit\n");
						notify_as(AS_ACTIVE);
						close(new_fd);
						goto RECONNECT;
					}
					goto SND_HB;
				}
				else 
				{
					len = recv(new_fd, buf, MAXBUF,0);
					if (len <=0) {
						printf("S:recv hb len=%d,error\n",len);
						if (++response_fail > HB_FAIL_EX) 
						{
							printf("S:response_fail more than ten times\n");
							notify_as(AS_ACTIVE);
						}
						close(new_fd);
						goto RECONNECT;
					}
					else {//recv hb ok,check it
						//printf("S:^^^^^recv hb[%d] ok\n", hb_ptr->hbSeq);
						response_fail=0;
						hb_ptr=(HB4ASCTRL*)buf;
						/*primary board up*/
						if (AS_PEND == _curr_as) {
							printf("S:update from AS_PEND\n");
							notify_as(reverseCfg(hb_ptr->board_as));
							goto SND_HB;
						}

						time_now=time(NULL);

						//进程起来一个半小时后再根据DNAR  进程的状态上报告警modified by wangxx 20120809
						if((time_now-_base_time)<_ignore_dnar_duration)
						{
							printf("time_now-_base_time=%d\n", time_now-_base_time);	
						}						
						else
						{
							//Dnar 状态影响主备
							printf("%s,%d\n",__FILE__, __LINE__);
							SHM_checkDnarAlive();
							//本板DNAR 活的
							if (_dnar_status&L_DNAR_ALIVE) 
							{
								if (_dnar_status&DNAR_UPDATE)
								{
									_dnar_status&=~DNAR_UPDATE;
									
									//RESUME & peer dnar dead,switch to active
									if (!(hb_ptr->dnar_alive&L_DNAR_ALIVE)) 
									{
										printf("RESUME & peer dnar dead,switch to AS_ACTIVE\n");
										notify_as(AS_ACTIVE);
									}
									goto SND_HB;
								}
								if (hb_ptr->board_as == _curr_as) 
								{
									//dnar都正常且冲突，则按配置
									/*while AS duplicate,According to config file*/
									if (hb_ptr->dnar_alive&L_DNAR_ALIVE) 
									{
										printf("while AS duplicate and both DNAR is alive, update AS_state according to config file\n");
										notify_as(_localcfg.local_as);
									}
									else 
									{//peer dnar dead
										printf("peer DNAR dead\n");
										notify_as(AS_ACTIVE);
									}
								}
							}
							//本板DNAR 死的
							else 
							{/*app dnar request switch*/
								if (_dnar_status&DNAR_UPDATE) 
								{
									_dnar_status&=~DNAR_UPDATE;
									if (hb_ptr->dnar_alive&L_DNAR_ALIVE) 
									{
										printf("local DNAR is dead, peer DNAR is alive, update to AS_STANDBY\n");
										notify_as(AS_STANDBY);
									}
									goto SND_HB;
								}
								if (hb_ptr->board_as == _curr_as)
								{
									if (!(hb_ptr->dnar_alive&L_DNAR_ALIVE)) 
									{
										/*while AS duplicate,According to config file*/
										printf("S:while AS duplicate and both DNAR is dead, ,update AS_state according cfgFile\n");
										notify_as(_localcfg.local_as);
									}
									else
									{
										printf("while AS duplicate and local DNAR is dead, peer DNAR is alive, update to AS_STANDBY\n");
										notify_as(AS_STANDBY);
									}
								}
							}//dnar not alive
						}
						goto SND_HB;
					}//recv hb
				}
			}
		}
	}	
}


void ASControl::test_func()
{
	char           inBuffer[4096];  	// Incoming message buffer

	SeMessage inMsg;
	inMsg.SetDataMsg((void*)inBuffer, sizeof(inBuffer));

	ServiceAddress sourceAddr;
	S_TEST_MSG* baseMsg = (S_TEST_MSG*)inBuffer;

       printf("ptest_thread start to recv\n");
	
	for(;;)
  	{
    		int err = inMsg.Receive(sourceAddr);
		if(err != SE_AOK)
    		{
		        printf("ptest_thread message is error\n");
			 
			 sleep(10);
			 continue;
    		}
		switch(baseMsg->msgId)
		{
			case ignore_dnar:
				ignore_func(baseMsg);		
				break;
			case show_start_time:
				show_time();
				break;
			default:
				printf("error msgId! msgId=%d\n", baseMsg->msgId);
				break;
		}
	}
}


void ASControl::client_func()
{
	int sockfd=-1;
	int len;
	struct sockaddr_in dest;
	char buffer[MAXBUF + 1];
	int myPort;
	HB4ASCTRL* hb_ptr;
	//int connect_c;
	struct timeval tva;
	fd_set inset, outset;
	int ret;
	myPort = selectTcpPort(reverseCfg(_localcfg.local_as));//clinet link to peer port

	/* 连接服务器 */
	//connect_c = 0;
	while(1) 
	{
		if(-1==sockfd)
		{
			for(int i=0; i<1; i++)
			{
				/* 创建一个 socket 用于 tcp 通信 */
				if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
				{
					perror("C:Socket");
					printf("C:Socket sterror:%s\n",strerror(errno));
					exit(errno);
				}
				
				printf("C:socket created %d\n",sockfd);
				/* 初始化服务器端（对方）的地址和端口信息 */
				bzero(&dest, sizeof(dest));
				dest.sin_family = AF_INET;
				dest.sin_port = htons(myPort);
				dest.sin_addr.s_addr = inet_addr(_localcfg.peer_ip);
				printf("C:address created,to connect %s,port %d.%s\n", inet_ntoa(dest.sin_addr), ntohs(dest.sin_port),timeStamp());
				

				//added by wangxx 20120809 begin
				int    optval=1;
				if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval))==-1)
				{
					close(sockfd);
					sockfd = -1;
					break;
				}
				//added by wangxx 20120809 end
				/*
				 //deleted by wangxx 20120809 because 非阻塞模式会导致报错connect Operation already in progress
				 //改用int connect_nonb(int sockfd, const SA *saptr, socklen_t salen, int nsec)   函数
				unsigned long ul = 1;
				//设置为非阻塞模式
		     		ioctl(sockfd, FIONBIO, &ul);  
		     		*/

				//阻塞模式下，IP不存在，会168秒超时
				//if (connect(sockfd, (struct sockaddr *) &dest, sizeof(dest)) < 0)
				//modified by wangxx 20120809 23:58
				if(connect_nonb(sockfd, (struct sockaddr *)&dest, sizeof(dest), 3))
				{
					//perror("C:Connect");
					printf("C:Connect sterror:%s,retry 3s later\n",strerror(errno));
					close(sockfd);
					sockfd=-1;
					sleep(3);
					break;
				}
				printf("C:server connected,%s\n",timeStamp());
			}
		}
		
		if(-1!=sockfd)
		{
			for(int i=0; i<1; i++)
			{
				/* 接收对方发过来的消息，最多接收 MAXBUF 个字节 */
				bzero(buffer, MAXBUF + 1);

				//sockfd  可读时才读
				tva.tv_sec=3;	  
				tva.tv_usec=0; 
				FD_ZERO(&inset);
				FD_SET(sockfd, &inset);
				ret = select(sockfd+1, &inset, NULL, NULL, &tva);
				if(-1 == ret) 
				{
					sleep(1);
					//perror("C:select");
					printf("C:inset select sterror:%s\n",strerror(errno));
					continue;
				}
				else if(0 == ret)
				{
					printf("C:none hb arrive in 3 seconds,need reconnect\n");
					close(sockfd);
					sockfd=-1;
					break;
				}

				/* 接收服务器来的消息 */		
				len = recv(sockfd, buffer, MAXBUF, 0);
				if(len > 0) 
				{
					//printf("C:接收消息成功:'%s'，共%d个字节的数据\n", buffer, len);
				}
				else 
				{
					printf("C:消息接收失败！错误代码是%d，错误信息是'%s'\n", errno, strerror(errno));
					close(sockfd);
					sockfd=-1;
					break;
				}

				//sockfd  可写时才写added by wangxx 20120810 1:02 begin
				tva.tv_sec=3;	  
				tva.tv_usec=0; 
				FD_ZERO(&outset);
				FD_SET(sockfd, &outset);
				ret = select(sockfd+1, NULL, &outset, NULL, &tva);
				if(-1 == ret) 
				{
					sleep(1);
					//perror("C:select");
					printf("C:outset select sterror:%s\n",strerror(errno));
					continue;
				}
				else if(0 == ret)
				{
					printf("C:sockfd can't be written in 3 seconds, need reconnect\n");
					close(sockfd);
					sockfd=-1;
					break;
				}
				//sockfd  可写时才写added by wangxx 20120810 1:02 end
				
				/*send hb reply,report STATUS*/
				hb_ptr=(HB4ASCTRL* )buffer;
				hb_ptr->header = ASHB_HDR;
				hb_ptr->invoke = HB_REPLY;
				hb_ptr->board_as = _curr_as;
				hb_ptr->dnar_alive=_dnar_status;
				len = send(sockfd, (void*)buffer, sizeof(HB4ASCTRL), 0);	

				if(len < 0) 
				{
					printf("C:消息'%s'发送失败！错误代码是%d，错误信息是'%s'\n", buffer,
					errno, strerror(errno));
					close(sockfd);
					sockfd=-1;
					break;
				}
				else 
				{
					//printf("C:消息'%s'发送成功，共发送了%d个字节！\n", buffer, len); 
				}
			}
		}
	}
	/* 关闭连接 */
	close(sockfd);
	sockfd=-1;
}

void ASControl::autorun_register()//系统启动后，autorun 带起3次
{
	int			         err;
	ServiceAddress     myAddr;
	AppControl       appOpts;
	ServicesElement    mySelf;

	appOpts.SetOption(SE_THRMSG | SE_CBNTFY);
	appOpts.SetRedundMode(SE_RM_DUPLEX);

	
	myAddr.SetApp("ASControl_App");
	myAddr.SetPrg("ASControl_Prg");
	

	if ((err = mySelf.InitStart(myAddr, appOpts)) != SE_AOK)
	{
		exit (-1);
	}
	printf("SE.InitStart ok.\n");  
	mySelf.InitComplete();
}

LOCAL_CFG ASControl::getlocalcfg()
{
	return _localcfg;
}

void ASControl::setbasetime(time_t t)
{
	_base_time=t;
	printf("setbasetime:  _base_time=%d\n",_base_time);  
}

void ASControl::setstarttime(time_t t)
{
	_start_time=t;
}

time_t ASControl::getstarttime()
{
	return _start_time;
}

void ASControl::set_dnar_status(int v)
{
	_dnar_status=v;
}

void ASControl::set_curr_as(int v)
{
	_curr_as=v;
}

/*end*/
