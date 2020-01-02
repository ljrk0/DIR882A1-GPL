if [ "$1" == "" ]; then
  echo Usage:./apcli_connect_trial.sh "<InfName> <SSID> <Authmode> <Encryp> <PSK> <Trial_channel> <bssid>"
  echo "<InfName> apcli0/apcli1"
  echo "<SSID> MT7621_AP"
  echo "<Authmode> OPEN/WPA2PSK"
  echo "<Encryp> NONE/TKIP/AES"
  echo "<PSK> 12345678"
  echo "<Trial_channel> 1-14,36-165"
  echo "<bssid> xx:xx:xx:xx:xx:xx"
  exit 0
fi

echo "Input:"
echo InfName=$1
echo RootAP=$2
echo Security=$3/$4
echo Passphrases=$5
echo Trial_channel=$6
echo bssid=$7

ifconfig $1 up
iwpriv $1 set ApCliTrialCh=$6 
iwpriv $1 set ApCliEnable=0
iwpriv ra0 set MACRepeaterEn=0
iwpriv $1 set ApCliSsid=$2
iwpriv $1 set ApCliBssid=$7
iwpriv $1 set ApCliAuthMode=$3
iwpriv $1 set ApCliEncrypType=$4
iwpriv $1 set ApCliWPAPSK=$5
iwpriv $1 set ApCliEnable=1

