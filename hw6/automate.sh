#!/bin/bash
# A simple client doing 

router_ip=$1
router_port=$2
text_file=$3

while IFS= read -r line; do
	ip=$(echo $line | cut -d " " -f 1)
	port=$(echo $line | cut -d " " -f 2)
	# Run server with the ip and port specified
	python node.py $ip $port &	
done < "$3"
  
# ./router $router_port $text_file
go run router.go $router_ip $router_port $text_file




