#!/bin/bash

dnf -y upgrade

dnf install -y wget thrift-devel libtool m4 automake byacc flex bison gcc-c++ git libevent-devel openssl-devel make pkg-config boost-static boost-devel python3-devel

dnf install -y vim

git clone https://github.com/apache/thrift.git

cd thrift

./bootstrap.sh

./configure

make

make install

cd lib/py

python3 setup.py install
