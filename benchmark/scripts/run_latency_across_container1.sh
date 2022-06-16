#!/bin/bash

<<<<<<< HEAD
# This bash script starts the ubuntu vm

killall qemu-system-x86_64

qemu-system-x86_64 \
 -machine accel=kvm,type=q35 \
 -smp cpus=1,cores=1 \
 -m 4G \
 -nographic \
 -device virtio-net-pci,netdev=net0 \
 -netdev user,id=net0,hostfwd=tcp::2222-:22,hostfwd=tcp::9093-:9093,hostfwd=tcp::9094-:9094 \
 -drive if=virtio,format=qcow2,file=/home/sierra/ubuntu-20.04-server-cloudimg-amd64.img \
 -drive if=virtio,format=raw,file=/home/sierra/seed.img \
  > ../results/latency_across_container_log.txt &

# For now, manually log into the ubuntu machine
# TO DO - fix this hack to actually wait until ready to log in
#sleep 20
#ssh ubuntu@build.a6e.org -p2222 bash run_latency_across_container2b.sh
