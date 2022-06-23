#!/bin/bash

dnf -y upgrade

dnf install -y wget thrift-devel libtool m4 automake byacc flex bison gcc-c++ git libevent-devel openssl-devel make pkg-config

dnf install -y vim
