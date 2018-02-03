#!/bin/sh
tar -C /usr/local -zxvf /data/src/go1.9.1.linux-amd64.tar.gz
mkdir -p /root/go/src
echo "export GOPATH=/root/go" >> ~/.bashrc
echo "export PATH=$PATH:$GOPATH/bin:/usr/local/go/bin" >> ~/.bashrc source ~/.bashrc
