#!/bin/bash

qemu-system-x86_64 \
 -machine accel=kvm,type=q35 \
 -smp cpus=1,cores=1 \
 -m 4G \
 -nographic \
 -device virtio-net-pci,netdev=net0 \
 -netdev user,id=net0,hostfwd=tcp::2222-:22 \
 -drive if=virtio,format=qcow2,file=$HOME/VM/fedora_36/hdd.qcow2 \
 -cdrom $HOME/VM/fedora_36/seedci.iso

# ssh into this vm with ssh fedora@localhost -p2222