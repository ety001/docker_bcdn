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
COPY . /data
WORKDIR /data
RUN tar zxvf src.tar.gz
RUN tar -C /usr/local -zxvf /data/src/go1.9.1.linux-amd64.tar.gz && \
        mkdir -p /root/go/src && \
        echo "export GOPATH=/root/go" >> ~/.bashrc && \
        echo "export PATH=$PATH:$GOPATH/bin:/usr/local/go/bin" >> ~/.bashrc source ~/.bashrc && \
        cd src/ && \
        unzip Libevent-release-2.1.8-stable.zip && \
        cd Libevent-release-2.1.8-stable && \
        ./autogen.sh && \
        ./configure --prefix=/usr && \
        make && make install
RUN cd src/ && \
        unzip protobuf-master.zip && \
        cd protobuf-master && \
        ./autogen.sh && \
        ./configure && \
        make && make check && make install && \
        ldconfig
RUN mv /data/src/M_BerryMiner_ubuntu_v1_0 /root && \
        cd /root && rm -rf src/ src.tar.gz
CMD "/data/run.sh"
