#!/bin/bash
ip=localhost
echo "Enter port number"
read port;
	cd web_server
	pwd
	if test -f "webserver_dynamic"; then
		cmd="./webserver_dynamic $port"
		$cmd &
		pid=$!   #pid of the running server which can be killed later
		cd ..
		start=$(date +%s.%N); \
		xargs -I % -P 10 wget -r -P dynamic $ip:$port/index.html \ < <(printf '%s\n' {1..20})
		runtime1=$(echo "$(date +%s.%N) - $start" | bc); \
		kill $pid
		rm dynamic
	else
		printf "webserver_dynamic is not found\n"
	fi
	 cd web_server 
	 if test -f "webserver_pool"; then
                cmd="./webserver_pool $port 5 FCFS"
                $cmd &
                pid=$!   #pid of the running server which can be killed later
                cd ..
		start=$(date +%s.%N); \
                xargs -I % -P 10 wget -r -P poolfc $ip:$port/index.html \ < <(printf '%s\n' {1..20})
                runtime2=$(echo "$(date +%s.%N) - $start" | bc); \
                kill $pid
cd web_server
cmd="./webserver_pool $port 5 SFF"
                $cmd &
                pid=$!   #pid of the running server which can be killed later
                cd ..
		start=$(date +%s.%N); \
                xargs -I % -P 10 wget -r -P poolsff $ip:$port/index.html \ < <(printf '%s\n' {1..20})
                runtime3=$(echo "$(date +%s.%N) - $start" | bc);
		kill $pid
		sleep 1s
		rm -r poolfc poolsff

        else
               printf "webserver_pool is not found\n"
        fi
	printf "\n"
	printf "Total runtime dynamic with 10 parallel requests is $runtime1 \n"
	printf "Total runtime pool with 10 parallel requests with FCFS and 5 pool threads is (1st test) $runtime2 \n"
printf "Total runtime pool with 10 parallel req with SFF and 5 pool threads is (2nd test) $runtime3 \n"


