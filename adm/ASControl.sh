#!/bin/bash
#
# ASControl.sh        This starts and stops ASControl.
#
#

[ -r /etc/profile.d/ASControlenv.sh ] || exit 1
. /etc/profile.d/ASControlenv.sh

SYSPATH=${RMLROOT}

allbase=`basename $0`

prog="ASControl"

. /etc/init.d/functions


PATH=${PATH}:${SYSPATH}/bin


[ -x ${SYSPATH}/bin/${prog} ] || exit 1

RETVAL=0

# A function to start a program.
daemon() {
	# Test syntax.
	local gotbase= force=
	local base= user= nice= bg= pid
	nicelevel=0
	while [ "$1" != "${1##[-+]}" ]; do
	  case $1 in
	    '')    echo $"$0: Usage: daemon [+/-nicelevel] {program}"
	           return 1;;
	    --check)
		   base=$2
		   gotbase="yes"
		   shift 2
		   ;;
	    --check=?*)
	    	   base=${1#--check=}
		   gotbase="yes"
		   shift
		   ;;
	    --user)
		   user=$2
		   shift 2
		   ;;
	    --user=?*)
	           user=${1#--user=}
		   shift
		   ;;
	    --force)
	    	   force="force"
		   shift
		   ;;
	    [-+][0-9]*)
	    	   nice="nice -n $1"
	           shift
		   ;;
	    *)     echo $"$0: Usage: daemon [+/-nicelevel] {program}"
	           return 1;;
	  esac
	done

        # Save basename.
        [ -z "$gotbase" ] && base=${1##*/}

        # See if it's already running. Look *only* at the pid file.
        pid=`pidfileofproc $base`
	
	[ -n "${pid:-}" -a -z "${force:-}" ] && return

	ulimit -S -c 1000000 > /dev/null 2>&1
	
	# Echo daemon
        [ "${BOOTUP:-}" = "verbose" -a -z "$LSB" ] && echo -n " $base"

	# And start it up.
	if [ -z "$user" ]; then
	   $nice initlog $INITLOG_ARGS -c "$*"
	else
	   $nice initlog $INITLOG_ARGS -c "su -s /bin/bash - $user -c \"$*\""
	fi
	[ "$?" -eq 0 ] && success $"$base startup" || failure $"$base startup"
}

# A function to stop a program.
killproc() {
	RC=0
	# Test syntax.
	if [ "$#" -eq 0 ]; then
		echo $"Usage: killproc {program} [signal]"
		return 1
	fi

	notset=0
	# check for second arg to be kill level
	if [ "$2" != "" ] ; then
		killlevel=$2
	else
		notset=1
		killlevel="-9"
	fi

        # Save basename.
        base=${1##*/}

        # Find pid.
	pid=`pidofproc $1`
	if [ -z "${pid:-}" ] ; then
		pid=`pidofproc $base`
	fi

        # Kill it.
        if [ -n "${pid:-}" ] ; then
                [ "$BOOTUP" = "verbose" -a -z "$LSB" ] && echo -n "$base "
		if [ "$notset" -eq "1" ] ; then
		       if checkpid $pid 2>&1; then
			   # TERM first, then KILL if not dead
			   kill -TERM $pid
			   usleep 100000
			   if checkpid $pid && sleep 1 &&
			      checkpid $pid && sleep 3 &&
			      checkpid $pid && sleep 3 &&
			      checkpid $pid && sleep 3 &&
			      checkpid $pid && sleep 3 &&
			      checkpid $pid && sleep 3 &&
			      checkpid $pid ; then
                                kill -KILL $pid
				usleep 100000
			   fi
		        fi
			checkpid $pid
			RC=$?
			[ "$RC" -eq 0 ] && failure $"$base shutdown" || success $"$base shutdown"
			RC=$((! $RC))
		# use specified level only
		else
		        if checkpid $pid >/dev/null 2>&1; then
	                	kill $killlevel $pid
				RC=$?
				[ "$RC" -eq 0 ] && success $"$base $killlevel" || failure $"$base $killlevel"
			fi
		fi
	else
	    failure $"$base shutdown"
	    RC=1
	fi

        # Remove pid file if any.
	if [ "$notset" = "1" ]; then
            rm -f /var/run/$base.pid
	fi
	return $RC
}

start(){
	# check whether needed directory exist
	if [ ! -d ${SYSPATH}/cfg ] ; then
		echo $"cfg directory does not exist, start failed"
		/sbin/initlog -n "$allbase" -s "cfg directory does not exist, start failed"
		RETVAL=1
		return $RETVAL
	fi 
	if [ ! -d ${SYSPATH}/log ] ; then
		echo $"log directory does not exist, please create log directory first, start failed"
		/sbin/initlog -n "$allbase" -s $"log directory does not exist, please create log directory first, start failed"
		RETVAL=1
		return $RETVAL
	fi 

    echo -n $"Starting $prog... "
	/sbin/initlog -n "$allbase" -s "Starting $prog... "

    unset HOME MAIL USER USERNAME
    daemon $prog -d$SYSPATH
    RETVAL=$?
    echo
    touch /var/lock/subsys/$prog
    return $RETVAL
}

stop(){
    echo -n $"Stopping $prog... "
	/sbin/initlog -n "$allbase" -s "Stopping $prog... "
    killproc $prog
    RETVAL=$?
    echo
    rm -f /var/lock/subsys/$prog
    return $RETVAL

}

restart(){
    stop
    start
}

condrestart(){
    [ -e /var/lock/subsys/$prog ] && restart
    return 0
}

pidcount_normal() {
	case `uname -r` in 
		2.4.18*)
			pidcount=`ps -ef | grep " $prog -d" | wc -l`
			if [ $pidcount -lt 5 ]  ;then
				return 1
			else return 0
			fi
			;;
		*)
			pidcount=`ps -ef | grep " $prog -d" | grep -v grep | wc -l`
			if [ $pidcount -lt 1 ]  ;then
				return 1
			else return 0
			fi
			;;
	esac
}

# See how we were called.
case "$1" in
    start)
	start
	RETVAL=$?
	;;
    stop)
	stop
	RETVAL=$?
	;;
    status)
	if status $prog  ;then
		if pidcount_normal ;then
			RETVAL=0
		else
			RETVAL=1
		fi
	else	
		RETVAL=1
	fi
	;;
    restart)
	restart
	RETVAL=$?
	;;
    condrestart)
	condrestart
	RETVAL=$?
	;;
    *)
	echo $"Usage: $0 {start|stop|status|restart|condrestart}"
	RETVAL=1
esac

exit $RETVAL
