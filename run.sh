#/bin/bash
clear 
gcc client.c -o client
gcc serveurdaemon.c -o servdaemon
killall servdaemon
killall client
./servdaemon  

#Run 3 clients
./client &
./client &
./client &


#Show process
ps -jH
netstat -npt | grep 20 
echo "Affichage state terminé"


