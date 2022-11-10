#!/bin/bash
filename='gruScript_offline_pid.log'
n=1
while IFS= read -r line;
do
kill -15 $line
done < $filename
