FROM ubuntu:16.04
VOLUME /data
WORKDIR /data
RUN /data/go_env.sh && \
        apt-get update && \
        apt-get install -y \
        unzip \
        build-essential \
        autoconf \
        automake \
        libtool \
        curl \
        make \
        g++ \
        libssl-dev && \
        unzip Libevent-release-2.1.8-stable.zip && \
        cd Libevent-release-2.1.8-stable && \
        ./autogen.sh && \
        ./configure --prefix=/usr && \
        make && make install && \
        ls -al /usr/lib | grep libevent && \
        cd .. && \
        unzip protobuf-master.zip && \
        cd protobuf-master && \
        ./autogen.sh && \
        ./configure && \
        make && make check && make install && \
        ldconfig && \
        protoc --version &&
        cd && cp -r /data/M_BerryMiner_ubuntu_v1_0 . && \
        echo $CODE > /root/M_BerryMiner_ubuntu_v1_0/server/conf/code.txt
CMD "/root/M_BerryMiner_ubuntu_v1_0/server/bcdn"
