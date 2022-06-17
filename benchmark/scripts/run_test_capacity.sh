#!/bin/bash

cd /home/ubuntu/AmbienceBenchmarksThrift/load_balancer
python3 test_capacity.py > /home/ubuntu/AmbienceBenchmarksThrift/benchmark/results/test_capacity_linux_output.txt

cd /home/ubuntu/AmbienceBenchmarksThrift/benchmark/scripts
python3 parse_test_capacity_output.py ../results/test_capacity_linux_output.txt > ../results/test_capacity_linux_parsed.txt
