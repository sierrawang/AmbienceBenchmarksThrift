#!/bin/bash

cd load_balancer

make

./read_endpoint > course_grain_results/linux_read_endpoint.txt
