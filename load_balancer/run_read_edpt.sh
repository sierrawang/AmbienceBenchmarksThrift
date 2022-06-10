#!/bin/bash

cd load_balancer

make

./read_endpoint > fine_grain_results/linux_read_endpoint.txt
