#!/bin/bash

qemu-system-x86_64 \
 -machine accel=kvm,type=q35 \
 -smp cpus=1,cores=1 \
 -m 4G \
 -nographic \
 -device virtio-net,netdev=network0,mac=66:66:66:66:66:67,host_mtu=65535 \
 -netdev tap,id=network0,ifname=tap0,script=no,downscript=no \
 -drive if=virtio,format=qcow2,file=$HOME/VM/fedora_36/hdd.qcow2 \
 -cdrom $HOME/VM/fedora_36/seedci.iso

# ssh into this vm with ssh fedora@10.0.1.72 -p22