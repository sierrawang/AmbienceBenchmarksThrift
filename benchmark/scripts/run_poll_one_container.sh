#!/bin/bash

cd poll

# Start all services
./poll > ../benchmark/results/poll_output.txt &
sleep 5
./poll_bench_agent > ../benchmark/results/latency_within_group_output.txt &
./poll_param_bench_agent > ../benchmark/results/param_latency_within_group_output.txt