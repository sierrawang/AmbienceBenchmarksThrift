#!/bin/bash

cd load_balancer

make

./tweet_db > fine_grain_results/linux_tweet_db.txt &
sleep 1
./user_db > fine_grain_results/linux_user_db.txt

