#!/bin/bash
#
# ASControladm.sh        ASControl maintenance 
#
#

# TODO add function to let edit configuration file

check_root()
{
    if [ `id -u` != 0 ]
    then
        echo This program requires you to be a Superuser.
        echo Please login as root and re-run $0.
        exit 1
    fi
}

check_root

if [ ! -r /etc/profile.d/ASControlenv.sh ] 
then
	echo "/etc/profile.d/ASControlenv.sh does not exist"
	exit 1
fi

. /etc/profile.d/ASControlenv.sh


SYSPATH=$RMLROOT

if [ ! -r /etc/init.d/ASControl ] ;then
	echo "/etc/init.d/ASControl does not exist"
	exit 1
fi

. /etc/init.d/ASControl function

[ -d ${SYSPATH} ] || exit 1

space="\t\t\t"
mainecho()
{
	echo -e $"${space}$1"
}

pause()
{
	echo
	echo -e "Press ENTER to continue\c"
	read TMPFLG
}

mainmenu()
{
	clear
	mainecho "        RML Administration Main Menu"
	mainecho
	mainecho "1,  Start ASControl"
	mainecho "2,  Stop ASControl"
	mainecho "3,  Restart ASControl"
	mainecho "4,  Display ASControl status"
	mainecho
	mainecho "5,  Start cron"
	mainecho "6,  Stop cron"
#	mainecho "7,  Enable automatic ASControl startup when poweron"
#	mainecho "8,  Disable automatic ASControl startup when poweron"
	mainecho "9,  Backup configuration"
	mainecho "0,  Restore configuration"
	mainecho "c,  clearlog"


	mainecho "q,  quit"
	mainecho
	mainecho "Your selection: \c"
}


while true; do
	mainmenu
	read input

	if [ `echo "${input}" | wc -w` != 1 ] ;then
		echo "Invalid input" ;pause; continue
	fi

	len=`expr length "${input}"`
	if [ $len -eq 1 ] ;then
		if [ 	"$input" != "1" -a "$input" != "2" -a \
			"$input" != "3" -a "$input" != "4" -a \
			"$input" != "5" -a "$input" != "6" -a \
			"$input" != "9" -a "$input" != "0" -a \
			"$input" != "c" -a "$input" != "C" -a \
			"$input" != "q" -a "$input" != "Q" ] ;then
			echo "Invalid input" ;pause; continue
		fi 	
	else
		echo "Invalid input" ;pause; continue
	fi

	case $input in
		1)
			${SYSPATH}/bin/ASControl.sh start
			pause
			;;	
		2)
			stop_verify_cron_only 
			echo "ASControl cron is also stopped, make sure restart cron later."
			${SYSPATH}/bin/ASControl.sh stop
			pause
			;;	
		3)
			${SYSPATH}/bin/ASControl.sh restart
			pause
			;;	
		4)
			${SYSPATH}/bin/ASControl.sh status
			pause
			;;	
		5)
			start_cron
			echo "ASControl cron started successfully"
			pause
			;;
		6)
			stop_cron
			echo "ASControl cron stopped successfully"
			pause
			;;
#		7)
#			[ -f /etc/rc3.d/S97ASControl ] && rm -f /etc/rc3.d/S97ASControl 
#			[ -f /etc/rc5.d/S97ASControl ] && rm -f /etc/rc5.d/S97ASControl
#			[ -f /etc/rc0.d/K02ASControl ] && rm -f /etc/rc0.d/K02ASControl
#			[ -f /etc/rc6.d/K02ASControl ] && rm -f /etc/rc6.d/K02ASControl
#			(cd /etc/rc3.d ; ln -s ../init.d/ASControl S97ASControl)
#			(cd /etc/rc5.d ; ln -s ../init.d/ASControl S97ASControl)
#			(cd /etc/rc0.d ; ln -s ../init.d/ASControl K02ASControl)
#			(cd /etc/rc6.d ; ln -s ../init.d/ASControl K02ASControl)
#			echo "Enable automatic ASControl startup successfully"
#			pause
#			;;
#		8)
#			[ -f /etc/rc3.d/S97ASControl ] && rm -f /etc/rc3.d/S97ASControl
#			[ -f /etc/rc5.d/S97ASControl ] && rm -f /etc/rc5.d/S97ASControl
#			[ -f /etc/rc0.d/K02ASControl ] && rm -f /etc/rc0.d/K02ASControl
#			[ -f /etc/rc6.d/K02ASControl ] && rm -f /etc/rc6.d/K02ASControl
#			echo "Disable automatic ASControl startup successfully"
#			pause
#			;;
		9)
			${SYSPATH}/bin/ASControlbackup.sh 
			pause
			;;	
		0)
			${SYSPATH}/bin/ASControlrestore.sh 
			pause
			;;	
		c|C)
			clearlog 
			clearcdr
			clearvox
			pause
			;;
		q|Q) 
			exit 0 
			;;
		*) 
			echo "Invalid input" 
			pause
			;;
	esac
done
