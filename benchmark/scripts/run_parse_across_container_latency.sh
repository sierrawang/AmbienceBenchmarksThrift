#!/bin/bash

python3 parse_poll_output.py ../results/latency_across_container_output.txt > ../results/latency_across_container_parsed.txt
python3 parse_poll_param_output.py ../results/param_latency_across_container_output.txt > ../results/param_latency_across_container_parsed.txt