echo "top -d 1 -n $1 -b > $2.txt"
top -d 1 -n $1 -b > $2.txt
read -rp "Press any key to download report..." key
echo "tftp -p -l $2.txt $3"
tftp -p -l $2.txt $3
