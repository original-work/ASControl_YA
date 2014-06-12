#!/bin/bash
#
# install.sh        this will install ASControl 
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

SYSPATH=/usr/ASControl

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

get_syspath()
{
	while true; do
		echo -e "Input the path where ASControl to be installed or updated(default ${SYSPATH}):\c"
		read input 

		if [ -n "${input:-}" ] ;then
			if [ "${input:0:1}" != "/" ] ;then
				echo "Invalid Input"
				continue
			else
				SYSPATH="${input}"
			fi
		fi
		break
	done
}

space="\t"
mainecho()
{
	echo -e $"${space}$1"
}

exit_uncompleted()
{
	echo "Installation abandoned"
	exit 1
}


#This is where the story starts...
update="no"
if [ $# -eq 1 ] ;then
	[ $1 = "--update" ] && update="yes"
fi

if [ $update = "no" -a -r /etc/profile.d/ASControlenv.sh ] ;then
	query_yes_no "A previous ASControl is detected, this installation will make previous ASControl unusable, continue" "n"
	[ $? -eq 0 ] || exit_uncompleted
fi

get_syspath

if [ -d ${SYSPATH} ] ;then
	if [ $update = "no" ] ;then
		query_yes_no "All the contents in ${SYSPATH} will be deleted, continue" "y"
		[ $? -eq 0 ] || exit_uncompleted
	fi
	bak_suffix=`date +%s`
	cp -rf ${SYSPATH}/bin ${SYSPATH}/bin.${bak_suffix}
	cp -rf ${SYSPATH}/cfg ${SYSPATH}/cfg.${bak_suffix}
	rm -rf ${SYSPATH}/bin
	[ $update = "yes" ] || rm -rf ${SYSPATH}/cfg
	[ $update = "yes" ] || rm -rf ${SYSPATH}/log
else
	query_yes_no "Path ${SYSPATH} does not exist, do you wish installation create it for you" "y"
	[ $? -eq 0 ] || exit_uncompleted
	if ! mkdir -p ${SYSPATH} ;then
		echo "Create path ${SYSPATH} failed"
		exit_uncompleted
	fi
fi

PKG_DIR=`dirname $0`

[ -f /etc/init.d/ASControl ] && rm -f /etc/init.d/ASControl
cp -v ${PKG_DIR}/bin/ASControl.init /etc/init.d/ASControl
[ -f /etc/profile.d/ASControlenv.sh ] && rm -f /etc/profile.d/ASControlenv.sh
[ -f /etc/profile.d/ASControlenv.csh ] && rm -f /etc/profile.d/ASControlenv.csh
echo "Making ASControlenv.sh..."
echo '# ASControl initialization script (sh)'                  > /etc/profile.d/ASControlenv.sh
echo 'if [ -z "$RMLROOT" ] ; then'                      >> /etc/profile.d/ASControlenv.sh
echo "    RMLROOT=${SYSPATH}"                           >> /etc/profile.d/ASControlenv.sh
echo 'fi'                                               >> /etc/profile.d/ASControlenv.sh
echo 'ASControlpath="${RMLROOT}/bin"'           >> /etc/profile.d/ASControlenv.sh
echo 'if ! printenv PATH | grep -q $ASControlpath ; then'     >> /etc/profile.d/ASControlenv.sh
echo '    PATH="$ASControlpath:$PATH"'                        >> /etc/profile.d/ASControlenv.sh
echo 'fi'                                               >> /etc/profile.d/ASControlenv.sh
echo 'export RMLROOT PATH'                              >> /etc/profile.d/ASControlenv.sh
chmod +x /etc/profile.d/ASControlenv.sh

echo "Making ASControlenv.csh..."
echo '# ASControl initialization script (csh)'                 > /etc/profile.d/ASControlenv.csh
echo 'if ( $?RMLROOT ) then'                            >> /etc/profile.d/ASControlenv.csh
echo '    exit'                                         >> /etc/profile.d/ASControlenv.csh
echo 'endif'                                            >> /etc/profile.d/ASControlenv.csh
echo "setenv RMLROOT ${SYSPATH}"                        >> /etc/profile.d/ASControlenv.csh
echo 'set ASControlpath="${RMLROOT}/bin"'       >> /etc/profile.d/ASControlenv.csh
echo 'printenv PATH | grep -q ${ASControlpath}'               >> /etc/profile.d/ASControlenv.csh
echo 'if ( $status == 0 ) then'                         >> /etc/profile.d/ASControlenv.csh
echo '    exit'                                         >> /etc/profile.d/ASControlenv.csh
echo 'endif'                                            >> /etc/profile.d/ASControlenv.csh
echo 'setenv PATH "${ASControlpath}:${PATH}"'                 >> /etc/profile.d/ASControlenv.csh
chmod +x /etc/profile.d/ASControlenv.csh

cp -rfv ${PKG_DIR}/bin ${SYSPATH}/
[ $update = "yes" ] || cp -rfv ${PKG_DIR}/cfg ${SYSPATH}/
[ $update = "yes" ] || mkdir ${SYSPATH}/log


echo
echo "Installation finished successfully, you should do ASControl configuration next."
