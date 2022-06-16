#!/bin/bash

python3 parse_poll_output.py ../benchmark/results/latency_within_group_output.txt > ../benchmark/results/latency_within_group_parsed.txt
python3 parse_poll_param_output.py ../benchmark/results/param_latency_within_group_output.txtt > ../benchmark/results/param_latency_within_group_parsed.txt