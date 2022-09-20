#!/bin/bash

####################
# Lab 1 Exercise 5
# Name: Chew Ee Jia
# Student No: A0217981W
# Lab Group: 13
####################

# Fill the below up
hostname=$(hostname)
machine_hardware=$(uname -m)
max_user_process_count=$(ulimit -u)
user_process_count=$(ps -e --no-headers | wc -l)
user_with_most_processes=$(ps -eo user|sort|uniq -c|sort -n|tail -1|awk '{print $2}')
mem_free_percentage=$(free | awk '/^Mem/ {a=($4/$2 * 100); print a}')

echo "Hostname: $hostname"
echo "Machine Hardware: $machine_hardware"
echo "Max User Processes: $max_user_process_count"
echo "User Processes: $user_process_count"
echo "User With Most Processes: $user_with_most_processes"
echo "Memory Free (%): $mem_free_percentage"
