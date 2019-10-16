#!/bin/bash
this_path="$(pwd)"
echo $this_path
path_to_script="$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )"
echo $path_to_script
server_path="${path_to_script}/sw/receiver/"
client_path="${path_to_script}/sw/sender/"

data_file0="file0.txt"

cd $server_path
make all
if [ "$?" -ne 0 ]; then 
    cd $this_path
    exit ; 
fi
cd $this_path


cd $client_path
make all
if [ "$?" -ne 0 ]; then 
    cd $this_path
    exit ; 
fi

# create 10 files from "data_file0"
if test -f "$data_file0"; then
    echo "$data_file0 exist"
fi
./duplicate_file0.sh

wireshark -i lo -k & 

sleep 2

cd $server_path; $(cat README.txt) &
cd $client_path; $(cat README.txt) 

