#!/bin/bash

# Kill any running containers
docker rm -f $(docker ps -a)

# Start the containers one by one
docker run -p 3003:3000 --net mynet --name read_edpt -v /home/ubuntu/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash load_balancer/run_read_edpt.sh &
sleep 15

docker run -p 3000:3000 --net mynet --name db_con -v /home/ubuntu/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash load_balancer/run_db.sh &
sleep 5

docker run -p 3001:3000 --net mynet --name worker_con -v /home/ubuntu/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash load_balancer/run_worker.sh &
sleep 5

echo DONE!
