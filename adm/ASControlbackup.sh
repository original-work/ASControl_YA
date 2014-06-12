#!/bin/bash
#
# ASControlbackup.sh        ASControl configuration backup
#
#

backup_file="ASControlconfig.tar.gz"

if [ ! -r /etc/profile.d/ASControlenv.sh ] 
then
	echo "/etc/profile.d/ASControlenv.sh does not exist"
	exit 1
fi

. /etc/profile.d/ASControlenv.sh


SYSPATH="$RMLROOT"

[ -d "${SYSPATH}" ] || exit 1

while true; do
	echo -e "Input directory where backup file is put or Q to quit: \c"
	read backup_dir

	if [ -z "${backup_dir:-}" ] ;then
		echo "Invalid input" ;continue
	fi

	if [ "${backup_dir}" = "Q" -o "${backup_dir}" = "q" ] ;then
		exit 1
	fi


	if [ ! -d "${backup_dir}" ] ;then
		echo "Directory \"${backup_dir}\" does not exist"
		continue
	fi
	break
done


tar_file="./cfg"

(cd "${SYSPATH}"; tar zcf "${backup_dir}"/"${backup_file}" $tar_file)
echo "ASControl backup finished successfully"
echo "you should put backup file ${backup_file} in a safe place"
