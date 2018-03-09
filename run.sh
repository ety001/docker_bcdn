#!/bin/bash
echo $CODE > /root/M_BerryMiner_Ubuntu_v4/conf/code.txt
sed -i 's/#\/bin\/bash/#!\/bin\/bash/g' /root/M_BerryMiner_Ubuntu_v4/script/monitor.sh
cd /root/M_BerryMiner_Ubuntu_v4 && ./script/monitor.sh
