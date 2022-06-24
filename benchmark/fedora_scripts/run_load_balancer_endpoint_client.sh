# Once the container is up an running, run the client to trigger the endpoint
cd ../../load_balancer/

# Run the client
python3 test_write_endpoint2.py

cp ../results/BEPS_write_endpoint_linux_output.txt > ../fedora_results/fedora_BEPS_client_write_endpoint_output.txt

python3 parse_BEPS_output.py ../fedora_results/fedora_BEPS_client_write_endpoint_output.txt > ../fedora_results/fedora_BEPS_client_write_endpoint_parsed.txt