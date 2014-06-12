CFLAGS    = -Wall -O3 -g
APP = ASControl 

INCLUDE = -I. -I/YAYTH/app/inc_YA/ -I/YAYTH/app/lib/api_YA/ -I/YAYTH/app/include_YA/ -I/YAYTH/app/lib/DB_YA/ \
    -I/VOBS/ngn_sx/sigtran_apps/intif/ -I/VOBS/ngn_sx/conn_mgt/extif/ -I/VOBS/ngn_thirdParty/CommServ/include/\
    -I/YAYTH/xsutil/cutil/ -I/YAYTH/xsutil/general/ -I/YAYTH/app/lib/alarm_YA/


LIBS    = -L/YAYTH/app/lib/lib_YA/ -lDB_xx -lrt -lc -lpthread -lCCSe_xx -lSePosix_xx -lAlarm

.cpp.o:
	$(CXX) $(CFLAGS) $(INCLUDE) -c $*.cpp
    
DEBUGFLAG = -D_DEBUG -DLINUX

ESRCS  = writeLogFile.cpp ReadCfgFile.cpp ASControl.cpp ASmain.cpp

EOBJS  = ${ESRCS:.cpp=.o}

all:$(APP)

$(APP): $(EOBJS)
	$(CXX) $(CFLAGS) $(INCLUDE) -o $(APP) $(EOBJS) $(LIBS)

clean:
	- rm -f $(APP) *.o 

pkg: all
	rm -f ./ASControlpkg*.tar.gz
	rm -rf ./ASControlpkg
	mkdir ./ASControlpkg
	cp ./adm/install.sh ./ASControlpkg/
	chmod +x ./ASControlpkg/install.sh
	cp ./adm/update.sh ./ASControlpkg/
	chmod +x ./ASControlpkg/update.sh
	cp -rf ./cfg ./ASControlpkg/
	rm -rf ./ASControlpkg/cfg/CVS
	rm -rf ./ASControlpkg/data/CVS
	mkdir ./ASControlpkg/bin
	cp -f ./ASControl ./ASControlpkg/bin/
	cp -f ./adm/ASControl.sh ./ASControlpkg/bin/
	cp -f ./adm/ASControl.init ./ASControlpkg/bin/
	cp -f ./adm/uninstall.sh ./ASControlpkg/bin/
	cp -f ./adm/ASControladm.sh ./ASControlpkg/bin/
	cp -f ./adm/ASControlbackup.sh ./ASControlpkg/bin/
	cp -f ./adm/ASControlrestore.sh ./ASControlpkg/bin/
	chmod +x ./ASControlpkg/bin/*
	tar zcvf ./ASControlpkg_ver`echo "$$Revision: 1.7 $$"| cut -d" " -f2`_osver`uname -r`.tar.gz ./ASControlpkg
																				
