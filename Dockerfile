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
        libssl-dev
