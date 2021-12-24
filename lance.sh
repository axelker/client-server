#/bin/bash
clear 
gcc client.c -o client
gcc serveurdaemon.c -o servdaemon
killall servdaemon
killall client
./servdaemon  

#On lance 3 clients
./client &
./client &
./client &


#affiche les processus
ps -jH
netstat -npt | grep 20 
echo "Affichage state terminé"


