#!/bin/bash

cp ../results/BEPS_write_endpoint_linux_output.txt ../fedora_results/fedora_BEPS_client_write_endpoint_output.txt

python3 parse_BEPS_output.py ../fedora_results/fedora_BEPS_client_write_endpoint_output.txt > ../fedora_results/fedora_BEPS_client_write_endpoint_parsed.txt
