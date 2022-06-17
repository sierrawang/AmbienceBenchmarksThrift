#!/bin/bash

cd /home/sierra/AmbienceBenchmarksThrift/load_balancer

python3 test_capacity.py > ../results/test_capacity_linux_output.txt

python3 parse_test_capacity_output.py ../results/test_capacity_linux_output.txt > ../results/test_capacity_linux_parsed.txt
