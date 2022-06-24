#!/bin/bash

# Kill any running containers
docker rm -f write_edpt

# Compile everything in a container then kill it
docker run --net mynet --name make_load_balancer_con -v $HOME/AmbienceBenchmarksThrift:/root/app -it thrift_img /bin/bash benchmark/fedora_scripts/make_load_balancer_logging.sh
docker rm -f make_load_balancer_con

docker run -p 9121:9121 --net mynet --name write_edpt -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash load_balancer/run_write_edpt.sh &
sleep 5

echo DONE!
