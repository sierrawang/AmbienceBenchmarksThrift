#!/bin/bash

# Kill any running containers
docker rm -f $(docker ps -a)

docker run -p 3003:3000 --net mynet --name make_poll_con -v /home/ubuntu/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash run_make_poll

echo yay

# Start the containers one by one
docker run -p 3003:3000 --net mynet --name poll_con -v /home/ubuntu/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash poll/run_poll.sh &
sleep 5

docker run -p 3000:3000 --net mynet --name agent_con -v /home/ubuntu/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash poll/run_agent.sh &
sleep 5

echo DONE!
