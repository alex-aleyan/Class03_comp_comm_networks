#!/bin/bash
which wireshark sed awk make; if [[ $? -ne 0 ]]; then echo "install: wireshark, sed, awk, make"; fi
if [ "$?" -ne 0 ]; then cd $this_path; exit ; fi


this_path="$(pwd)"
echo $this_path
path_to_script="$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )"
echo $path_to_script
server_path="${path_to_script}/sw/server"
client_path="${path_to_script}/sw/client"

data_file0="file0.txt"
dest_file="file.txt"

cd $server_path
if [ "$?" -ne 0 ]; then cd $this_path; exit ; fi
rm $dest_file
make all
if [ "$?" -ne 0 ]; then cd $this_path; exit ; fi

cd $this_path

cd $client_path
if [ "$?" -ne 0 ]; then cd $this_path; exit ; fi
rm $dest_file
make all
if [ "$?" -ne 0 ]; then cd $this_path; exit ; fi

# create 10 files from "data_file0"
if test -f "$data_file0"; then echo "$data_file0 exist"; fi

./duplicate_file0.sh


cd $server_path; 
$(cat README.txt) &

cd $client_path; 
$(cat README.txt) 

echo -e "\nTo manually run the client and server applications:"
cd $server_path; 
echo -e "\n\tFirst please launch the server application like this:"
echo -e "\t$(cat README.txt)"

cd $client_path; 
echo -e "\n\tNext please launch the client application like this:"
echo -e "\t$(cat README.txt)"

cd $this_path

echo -e "\n\nSee:\n\t$server_path/file.txt \
\n\t$server_path/README.txt \
\n\t$client_path/README.txt"

