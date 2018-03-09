FROM ety001/bcdn:base_image
RUN wget https://dl.blockcdn.org/dev/2018-03-09-v0.1.1-100.amd64.tar.gz && \
        mkdir M_BerryMiner_Ubuntu_v4 && \
        tar -xxvf 2018-03-09-v0.1.1-100.amd64.tar.gz -C M_BerryMiner_Ubuntu_v4 && \
        rm -rf  2018-03-09-v0.1.1-100.amd64.tar.gz && \
        echo $CODE > conf/code.txt
WORKDIR /root/M_BerryMiner_Ubuntu_v4
CMD "./script/monitor.sh"
