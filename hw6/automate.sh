#!/bin/bash
# A simple client doing 

router_port=$1
text_file=$2

while IFS= read -r line; do
	ip=$(echo $line | cut -d " " -f 1)
	port=$(echo $line | cut -d " " -f 2)
	# Run server with the ip and port specified
	python node.py $ip $port &	
done < "$2"
  
# ./router $router_port $text_file
go run router.go $router_port $text_file




