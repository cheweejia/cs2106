#!/bin/bash

####################
# Lab 1 Exercise 7
# Name: Chew Ee Jia
# Student No: A0217981W
# Lab Group: 13
####################

####################
# Strings that you may need for prompt (not in order)
####################
# Enter $N numbers:
# Enter NEW prefix (only alphabets):
# Enter prefix (only alphabets):
# INVALID
# Number of files to create:
# Please enter a valid number [0-9]:
# Please enter a valid prefix [a-z A-Z]:

####################
# Strings that you may need for prompt (not in order)
####################

#################### Steps #################### 

# Fill in the code to request user for the prefix

# Check the validity of the prefix #

# Enter numbers and create files #

# Fill in the code to request user for the new prefix

# Renaming to new prefix #


echo ""
prefix_correct=false
read -p 'Enter prefix (only alphabets):' prefix

while [ $prefix_correct != true ]
do
    if [ -z "$prefix" ] || ! [[ "$prefix" =~ ^[[:alpha:]]+$ ]]
    then
        echo "INVALID"
        read -p 'Please enter a valid prefix [a-z A-Z]:' prefix
    else
        prefix_correct=true
    fi
done

N_correct=false
read -p 'Number of files to create:' N

while [ $N_correct != true ]
do
    if [ -z "$N" ] || ! [[ "$N" =~ ^[[:digit:]]+$ ]]
    then
        echo "INVALID"
        read -p 'Please enter a valid number [0-9]:' N
    else
        N_correct=true
    fi
done

declare -a numbers
echo "Enter $N numbers:"
for (( c=0; c<$N; c++ ))
do 
    read number
    number_correct=false
    while [ $number_correct != true ]
    do
        if [ -z "$number" ] || ! [[ "$number" =~ ^[[:digit:]]+$ ]]
        then
            echo "INVALID"
            read -p 'Please enter a valid number [0-9]:' number
        else
            numbers[$c]=$number
            number_correct=true
        fi
    done
done

for number in ${numbers[@]}
do
    touch "$prefix"_"$number".txt
done

echo "Files Created"
ls *.txt
echo ""

new_prefix_correct=false
read -p "Enter NEW prefix (only alphabets):" new_prefix
while [ $new_prefix_correct != true ]
do
    if [ -z "$new_prefix" ] || ! [[ "$new_prefix" =~ ^[[:alpha:]]+$ ]]
    then
        echo "INVALID"
        read -p 'Please enter a valid prefix [a-z A-Z]:' new_prefix
    else
        new_prefix_correct=true
    fi
done

for old_file in *.txt
do
    mv -- "$old_file" "$new_prefix"_"${old_file#*_}"
done

echo ""
echo "Files Renamed"
ls *.txt
echo ""