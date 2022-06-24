#!/bin/bash

# Kill any running containers
docker rm -f client_con

# This container will exit after script finishes
docker run--net mynet --name client_con -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash load_balancer/run_load_balancer_client2.sh

# Parse the client output!
cd $HOME/AmbienceBenchmarksThrift/benchmark/fedora_scripts/

python3 parse_BEPS_output.py ../fedora_results/fedora_BEPS_client_client_in_container_output.txt > ../fedora_results/fedora_BEPS_client_client_in_container_parsed.txt

echo DONE!
