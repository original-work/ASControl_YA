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
//  08/28/2012            WangXinXin                                 original
//
//============================================================================

#include "ASControl.h"


ASControl* g_obj=ASControl::instance();
Alarm* g_AlarmObjPtr=Alarm::instance();


void* pclient_thread(void *arg)
{
	//ASControl* obj=ASControl::instance();
	g_obj->client_func();
	return 0;
}
void* pserver_thread(void *arg)
{
	//ASControl* obj=ASControl::instance();
	g_obj->server_func();
	return 0;
}
/*added by wangxx begin 20120827 20:37*/
void* ptest_thread(void *arg)
{
	//ASControl* obj=ASControl::instance();
	g_obj->test_func();
	return 0;
}
/*added by wangxx end 20120827 20:37*/



int main(int argc, char **argv)
{
	
       pthread_t id1,id2;
	int ret;
	time_t s_time;
	strcpy(progName,"ASControl");
	printf("PROG ASControl BUILD[%s,%s]\n",__DATE__,__TIME__);
	print_version(argc, argv);

	g_obj->autorun_register();
	g_obj->readLocalConfig();
	g_obj->readDRevSyncConfig();
	g_obj->get_AlarmType();
	g_obj->set_curr_as(AS_PEND);
	sleep(1);
	g_obj->SHM_ASControl_init();
	g_obj->set_dnar_status(L_DNAR_ALIVE);
	if (AS_STANDBY == g_obj->getlocalcfg().local_as) {
		g_obj->notify_as(AS_STANDBY);
	}

	s_time=time(NULL);
	g_obj->setstarttime(s_time);
	g_obj->setbasetime(s_time);	
	
	ret=pthread_create(&id1,NULL,pserver_thread, NULL);       
	if(ret!=0)               perror("pthread create1");
          
	ret=pthread_create(&id2,NULL,pclient_thread, NULL);       
	if(ret!=0)               perror("pthread create2");

	pthread_join(id1,NULL);
	pthread_join(id2,NULL); 
	printf("exit\n");
	return 0;
}

