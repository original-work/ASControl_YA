#!/bin/bash
#
# uninstall.sh        this will uninstall ASControl
#
#

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

######################################################################
# Prompt and error check a yes/no question
######################################################################
query_yes_no()
{
	_ANSWER=

	if [ $# -ne 2 ]
	then
		echo "Usage: query_yes_no message default" 1>&2
		exit 1
	fi

	while true
	do
		echo -e "$1 (y/n, default=$2)?\c "
		read _ANSWER

		# Return default if string length is zero
		if [ -z $_ANSWER ]
		then
			if [ $2 = "y" ]
			then
				return 0;
			else
				return 1;
			fi
		fi

		case "$_ANSWER" in
			y | Y | [Yy][eE][Ss] ) return 0 ;;
			n | N | [Nn][Oo] ) return 1 ;;

			* ) echo "Please answer yes or no (y/n)" ;;
		esac
	done
}

query_yes_no "This will uninstall ASControl, all ASControl data will be deleted, continue" "n"
[ $? -eq 0 ] || exit 1
[ -f /etc/init.d/ASControl ] && rm -f /etc/init.d/ASControl
[ -f /etc/profile.d/ASControlenv.sh ] && rm -f /etc/profile.d/ASControlenv.sh
[ -f /etc/profile.d/ASControlenv.csh ] && rm -f /etc/profile.d/ASControlenv.csh

rm -rf ${SYSPATH}
echo "Uninstall ASControl successfully"
