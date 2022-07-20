#!/bin/bash

docker rm -f $(docker ps -aq)

# Compile everything in a container then kill it
docker run --net mynet --name make_poll_con -v $HOME/AmbienceBenchmarksThrift:/root/app -it thrift_img /bin/bash benchmark/new_scripts/make_poll.sh
docker rm -f make_poll_con

# Start the containers one by one
docker run -p 9092:9092 --net mynet --name poll_con -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash benchmark/new_scripts/run_just_poll.sh $1 &

docker run -p 9093:9093 -p 9094:9094 --net mynet --name agent_con -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash benchmark/new_scripts/run_agents.sh $1 &
sleep 3

echo DONE!
