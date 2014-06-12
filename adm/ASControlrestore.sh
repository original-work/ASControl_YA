#!/bin/bash
#
# ASControlrestore.sh        ASControl configuration restore
#
#


if [ ! -r /etc/profile.d/ASControlenv.sh ] 
then
	echo "/etc/profile.d/ASControlenv.sh does not exist"
	exit 1
fi

. /etc/profile.d/ASControlenv.sh


SYSPATH="$RMLROOT"

[ -d "${SYSPATH}" ] || exit 1

while true; do
	echo -e "Input backup file name(with directory) or Q to quit: \c"
	read backup_file

	if [ -z "${backup_file:-}" ] ;then
		echo "Invalid input" ;continue
	fi

	if [ "${backup_file}" = "Q" -o "${backup_file}" = "q" ] ;then
		exit 1
	fi


	if [ ! -r "${backup_file}" ] ;then
		echo "${backup_file} does not exist"
		continue
	fi
	break
done

(cd "${SYSPATH}"; tar zxf ${backup_file})
echo "ASControl restore finished successfully"
