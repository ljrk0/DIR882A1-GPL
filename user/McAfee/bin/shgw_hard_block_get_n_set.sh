#!/bin/sh
. /etc/shgw/shgw.constants

	_temp_file="${TMPFS_PATH}/temp_hard_blk"
case $1 in

        get)

                $ECHO "object:Device.WiFi.AccessPoint.1.X_LANTIQ_COM_Vendor: :GET" > ${_temp_file}
                $ECHO "param:MACAddressControlList: :" >> ${_temp_file}

                $ECHO "object:Device.WiFi.AccessPoint.2.X_LANTIQ_COM_Vendor: :GET" >> ${_temp_file}
                $ECHO "param:MACAddressControlList: :" >> ${_temp_file}

                ${CALTEST} -g ${_temp_file} 2>&1 | ${GREP} ${PARAMVALUE} | ${AWK} 'FNR == 3 {print $3}' | ${TR} -d '""' > /var/.shgw/temp_caltest
                ${CALTEST} -g ${_temp_file} 2>&1 | ${GREP} ${PARAMVALUE} | ${AWK} 'FNR == 4 {print $3}' | ${TR} -d '""' >> /var/.shgw/temp_caltest
        ;;

        set)

		case $2 in

			block) 

				$ECHO "object:Device.WiFi.AccessPoint.1.X_LANTIQ_COM_Vendor: :MODIFY" > ${_temp_file}
				$ECHO "param:MACAddressControlList: :$3" >> ${_temp_file}
				$ECHO "param:MACAddressControlMode: :Deny" >> ${_temp_file}

				$ECHO "object:Device.WiFi.AccessPoint.2.X_LANTIQ_COM_Vendor: :MODIFY" >> ${_temp_file}
				$ECHO "param:MACAddressControlList: :$4" >> ${_temp_file}
				$ECHO "param:MACAddressControlMode: :Deny" >> ${_temp_file}
			        ${CALTEST} -s ${_temp_file}
        
			;;
	
			unblock)

				$ECHO "object:Device.WiFi.AccessPoint.1.X_LANTIQ_COM_Vendor: :MODIFY" > ${_temp_file}
				if [ ${#3} -le 1  ]
				then
				        $ECHO "param:MACAddressControlList: :\"" >> ${_temp_file}
					$ECHO "param:MACAddressControlMode: :Disabled" >> ${_temp_file}
				else
				        $ECHO "param:MACAddressControlList: :${3}" >> ${_temp_file}
					$ECHO "param:MACAddressControlMode: :Deny" >> ${_temp_file}
				fi

			        $ECHO "object:Device.WiFi.AccessPoint.2.X_LANTIQ_COM_Vendor: :MODIFY" >> ${_temp_file}
				if [ ${#4} -le 1 ]
				then
			        	$ECHO "param:MACAddressControlList: :\"" >> ${_temp_file}
					$ECHO "param:MACAddressControlMode: :Disabled" >> ${_temp_file}
				else
			        	$ECHO "param:MACAddressControlList: :${4}" >> ${_temp_file}
					$ECHO "param:MACAddressControlMode: :Deny" >> ${_temp_file}
				fi
			        ${CALTEST} -s ${_temp_file}

			;;
		esac
	;;
esac

