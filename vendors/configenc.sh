#!/bin/sh

#echo $0
echo $1
echo $2
case "$1" in
        enc) 
		enc=1
		if [ -f $2/RT2860_default_novlan ]; then \
			openssl enc -e -aes-256-ecb -k 3E076A574946E033B765C121DEEB8A70 -in $2/RT2860_default_novlan -out $2/RT2860_default_novlan.enc; \
		fi
		if [ -f $2/RT2860_default_vlan ]; then \
			openssl enc -e -aes-256-ecb -k 3E076A574946E033B765C121DEEB8A70 -in $2/RT2860_default_vlan -out $2/RT2860_default_vlan.enc; \
		fi
		if [ -f $2/RT2860_default_vlan-factory ]; then \
			openssl enc -e -aes-256-ecb -k 3E076A574946E033B765C121DEEB8A70 -in $2/RT2860_default_vlan-factory -out $2/RT2860_default_vlan-factory.enc; \
		fi
		echo "----enc-:$enc"
		;;
		
        dec)
		dec=1
			if [ -f $2/RT2860_default_novlan.enc ]; then \
			openssl enc -d -aes-256-ecb -k 3E076A574946E033B765C121DEEB8A70 -in $2/RT2860_default_novlan.enc -out $2/RT2860_default_novlan; \
			fi

			if [ -f $2/RT2860_default_vlan.enc ]; then \
			openssl enc -d -aes-256-ecb -k 3E076A574946E033B765C121DEEB8A70 -in $2/RT2860_default_vlan.enc -out $2/RT2860_default_vlan; \
			fi

			if [ -f $2/RT2860_default_vlan-factory.enc ]; then \
			openssl enc -d -aes-256-ecb -k 3E076A574946E033B765C121DEEB8A70 -in $2/RT2860_default_vlan-factory.enc -out $2/RT2860_default_vlan-factory; \
			fi
		echo "----dec-:$dec"
		;;
		
        clean) 
		clean=1
			if [ -f $2/RT2860_default_novlan.enc ]; then \
			rm -f $2/RT2860_default_novlan; \
			fi

			if [ -f $2/RT2860_default_vlan.enc ]; then \
			rm -f $2/RT2860_default_vlan; \
			fi

			if [ -f $2/RT2860_default_vlan-factory.enc ]; then \
			rm -f $2/RT2860_default_vlan-factory; \
			fi
		echo "----clean-:$clean"
		;;
		
esac
