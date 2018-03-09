FROM ubuntu:16.04
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
        libssl-dev \
        wget
WORKDIR /data
RUN wget https://dl.google.com/go/go1.9.1.linux-amd64.tar.gz && \
        wget https://github.com/ety001/docker_bcdn/raw/master/src/Libevent-release-2.1.8-stable.zip && \
        wget https://github.com/ety001/docker_bcdn/raw/master/src/protobuf-master.zip && \
        tar -C /usr/local -zxvf /data/go1.9.1.linux-amd64.tar.gz && \
        mkdir -p /root/go/src && \
        echo "export GOPATH=/root/go" >> ~/.bashrc && \
        echo "export PATH=$PATH:$GOPATH/bin:/usr/local/go/bin" >> ~/.bashrc source ~/.bashrc && \
        unzip Libevent-release-2.1.8-stable.zip && \
        cd Libevent-release-2.1.8-stable && \
        ./autogen.sh && \
        ./configure --prefix=/usr && \
        make && make install && \
        cd /data && \
        unzip protobuf-master.zip && \
        cd protobuf-master && \
        ./autogen.sh && \
        ./configure && \
        make && make check && make install && \
        ldconfig && \
        cd && \
        rm -rf /data/*
WORKDIR /root
CMD "pwd"
