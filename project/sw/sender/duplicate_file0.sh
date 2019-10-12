#!/bin/bash

for i in {1..9}
do  
cp file0.txt file${i}.txt
echo "$i"; 
sed -i -e "s/^/file${i}:/" file${i}.txt; 
done
