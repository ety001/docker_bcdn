#!/bin/sh
unzip protobuf-master.zip && \
cd protobuf-master && \
./autogen.sh && \
./configure && \
make && make check && make install && \
ldconfig && \
protoc --version && \
cd .. && rm -rf /data/protobuf-master.zip /data/protobuf-master
