#!/bin/bash

cd load_balancer

./tweet_db > course_grain_results/linux_tweet_db.txt &
sleep 1
./user_db > course_grain_results/linux_user_db.txt

