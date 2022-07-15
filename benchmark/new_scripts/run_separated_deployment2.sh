#!/bin/bash

docker rm -f $(docker ps -aq)

# Compile everything in a container then kill it
docker run --net mynet --name make_load_balancer_con -v $HOME/AmbienceBenchmarksThrift:/root/app -it thrift_img /bin/bash benchmark/new_scripts/make_lb_logging.sh
docker rm -f make_load_balancer_con

# Start the containers one by one
docker run --net mynet --name read_edpt -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash benchmark/new_scripts/run_individual_services/run_redpt.sh &
sleep 1

docker run --net mynet --name t_db -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash benchmark/new_scripts/run_individual_services/run_tweetdb.sh &
sleep 1

docker run --net mynet --name u_db -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash benchmark/new_scripts/run_individual_services/run_userdb.sh t_db &
sleep 1

for (( i = 0; i < 5; i++ ))
do
	docker run --net mynet --name worker_con$i -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash benchmark/new_scripts/run_individual_services/run_worker.sh $i t_db u_db read_edpt &
done

sleep 1

docker run --net mynet --name lb -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash benchmark/new_scripts/run_individual_services/run_lb.sh $1 worker_con &
sleep 1

docker run -p 9121:9121 --net mynet --name w_edpt -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash benchmark/new_scripts/run_individual_services/run_wedpt.sh $1 lb &
sleep 1

echo DONE!
