#!/bin/bash

this_path="$(pwd)"
path_to_script="$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )"
server_path="${path_to_script}/sw/receiver"
client_path="${path_to_script}/sw/sender"

#cd $server_path; 
#gnome-terminal --title="1" -e "./receiver.run --source-ip 127.0.0.1 --source-port=7777 --output-file dump.txt -v -x &" &
#cd $client_path; 
#gnome-terminal --title="2" -e "./sender.run --source-ip 127.0.0.1 --source-port 7778 --dest-ip 127.0.0.1 --dest-port 7777 --output-file file.txt -v -x file0.txt file1.txt file2.txt file3.txt file4.txt file5.txt file6.txt file7.txt file8.txt file9.txt;" &

cd $server_path; 
gnome-terminal --title="server:" -e "./receiver.run --source-ip 127.0.0.1 --source-port=7777 --output-file dump.txt -v -x &;" &
cd $client_path; 
#gnome-terminal --title="2" -e "./sender.run --source-ip 127.0.0.1 --source-port 7778 --dest-ip 127.0.0.1 --dest-port 7777 --output-file file.txt -v -x file0.txt file1.txt file2.txt file3.txt file4.txt file5.txt file6.txt file7.txt file8.txt file9.txt; " & 

#gnome-terminal --geometry=345x5+0+0 --title="PROC IRQS" -e "pwd;" &
#gnome-terminal --geometry=345x5+0+0 --title="$1 PROC IRQS" -e "bash -c pwd &" &
#gnome-terminal -x bash -c "$(cat README.txt); exec bash" &
#gnome-terminal --geometry=345x5+0+0 --title="$1 PROC IRQS" --window-with-profile="minimized" -e "$(cat README.txt) &" &
#xterm -title server -geometry 102x39+0+0   -e '$(cat README.txt) &' &
#gnome-terminal -e "bash -c pwd; bash -c $(cat README.txt) &;bash" &

#$(cat README.txt) 
#gnome-terminal -x bash -c "$(cat README.txt); exec bash" &
#gnome-terminal --geometry=345x5+0+0 --title="$1 PROC IRQS" --window-with-profile="minimized" -e "$(cat README.txt) &" &
#xterm -title client -geometry 102x39+0+0   -e '$(cat README.txt) &' &
#gnome-terminal -e "bash -c pwd; bash -c $(cat README.txt) &;bash" &


