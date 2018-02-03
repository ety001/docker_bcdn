#!/bin/sh
tar -C /usr/local -zxvf /data/go1.9.1.linux-amd64.tar.gz
mkdir -p ~/go/src
echo "export GOPATH=$HOME/go" >> ~/.bashrc
echo "export PATH=$PATH:$GOPATH/bin:/usr/local/go/bin" >> ~/.bashrc source ~/.bashrc
go version
