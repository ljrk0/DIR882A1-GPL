#########################################################################
# File Name: decompress.sh
# Author: linhongqing
# mail: linhongqing@twsz.com
# Created Time: 2017年11月17日 星期五 22时12分18秒
#########################################################################
#!/bin/bash
path="$1"
[ "$#" = "0" ] && path="./"
files=$(find ${path} -name "*.tar" -o -name "*.gz" -o -name "*.bz2" -o -name "*.zip" -o -name "*.tgz" -o -name "*.xz" 2</dev/null)
while [ "${files}x" != "x" ]; do
	#echo ${files}
	for file in ${files}; do
		echo "file=${file}"
		filepath=$(echo ${file} | awk -F'/' '{for (i=1;i!=NF;++i)printf $i"/"}')
		#echo "path=${filepath}"
		case ${file} in
			*.tar.gz | *.tgz)
				tar zxf ${file} -C ${filepath} > /dev/null 2>&1
				rm -rf ${file}
				;;
			*.tar.bz2)
				tar jxf ${file} -C ${filepath} > /dev/null 2>&1
				rm -rf ${file}
				;;
			*.xz)
				xz -d ${file} > /dev/null 2>&1
				rm -rf ${file} > /dev/null 2>&1
				;;
			*.tar)
				tar xf ${file} -C ${filepath} > /dev/null 2>&1
				rm -rf ${file}
				;;
			*.gz)
				gunzip -f ${file} > /dev/null 2>&1
				;;
			*.bz2)
				bunzip2 -f ${file} > /dev/null 2>&1
				;;
			*.zip)
				unzip -f ${file} > /dev/null 2>&1
				rm -rf ${file}
				;;
		esac
	done
	files=$(find ${path} -name "*.tar" -o -name "*.gz" -o -name "*.bz2" -o -name "*.zip" -o -name "*.tgz" -o -name "*.xz" 2</dev/null)
done
