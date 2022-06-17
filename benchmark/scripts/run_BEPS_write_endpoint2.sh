#!/bin/bash

# Kill any running containers
docker rm -f $(docker ps -a)

# Compile everything in a container then kill it
docker run -p 3003:3000 --net mynet --name make_load_balancer_con -v /home/ubuntu/AmbienceBenchmarksThrift:/root/app -it thrift_img /bin/bash benchmark/scripts/run_make_load_balancer.sh
docker rm -f make_load_balancer_con

# Start the containers one by one
docker run -p 3003:3000 --net mynet --name read_edpt -v /home/ubuntu/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash load_balancer/run_read_edpt.sh &
sleep 15

docker run -p 3000:3000 --net mynet --name db_con -v /home/ubuntu/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash load_balancer/run_db.sh &
sleep 5

docker run -p 3001:3000 -p --net mynet --name worker_con -v /home/ubuntu/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash load_balancer/run_worker.sh &
sleep 5

docker run -p 3002:3000 -p 9121:9121 --net mynet --name write_edpt -v /home/ubuntu/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash load_balancer/run_write_edpt.sh &
sleep 5

echo DONE!
