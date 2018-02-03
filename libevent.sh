#!/bin/sh
unzip Libevent-release-2.1.8-stable.zip && \
cd Libevent-release-2.1.8-stable && \
./autogen.sh && \
./configure --prefix=/usr && \
make && make install && \
ls -al /usr/lib | grep libevent && \
cd .. && rm -rf Libevent-release-2.1.8-stable.zip Libevent-release-2.1.8-stable
