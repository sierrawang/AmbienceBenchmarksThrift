#!/bin/bash

apt -y update && apt -y upgrade

apt -y install automake bison flex g++ git libboost-all-dev libevent-dev libssl-dev libtool make pkg-config

apt -y install wget

wget http://sourceforge.net/projects/boost/files/boost/1.60.0/boost_1_60_0.tar.gz

tar xvf boost_1_60_0.tar.gz

cd boost_1_60_0

./bootstrap.sh

./b2 install

cd ..

wget https://dlcdn.apache.org/thrift/0.16.0/thrift-0.16.0.tar.gz

tar xvf thrift-0.16.0.tar.gz

cd thrift-0.16.0

./configure && make

make install

apt -y install cmake

cmake .

cd ..
