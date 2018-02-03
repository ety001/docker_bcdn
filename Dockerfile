FROM ubuntu:16.04
VOLUME /data
WORKDIR /data
COPY . /data
RUN apt-get update && \
        apt-get install -y \
        unzip \
        build-essential \
        autoconf \
        automake \
        libtool \
        curl \
        make \
        g++ \
        libssl-dev
RUN /data/go_env.sh && \
        /data/libevent.sh && \
        /data/protobuf.sh && \
        mv /data/M_BerryMiner_ubuntu_v1_0 /root && \
        echo $CODE > /root/M_BerryMiner_ubuntu_v1_0/server/conf/code.txt
CMD "/data/run.sh"
