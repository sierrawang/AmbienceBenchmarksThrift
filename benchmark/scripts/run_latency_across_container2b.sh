#!/bin/bash

# Kill any running containers
docker rm -f $(docker ps -a)

# Compile everything in a container then kill it
docker run -p 3003:3000 --net mynet --name make_poll_con -v /home/ubuntu/AmbienceBenchmarksThrift:/root/app -it thrift_img /bin/bash benchmark/scripts/run_make_poll.sh
docker rm -f make_poll_con

# Start the containers one by one
docker run -p 3003:3000 --net mynet --name poll_con -v /home/ubuntu/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash benchmark/scripts/run_poll.sh &
sleep 5

docker run -p 3000:3000 --net mynet --name agent_con -v /home/ubuntu/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash benchmark/scripts/run_agent.sh &
sleep 5

echo DONE!
