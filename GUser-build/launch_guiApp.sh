#!bin/bash
#SIGKILL cannot be caught, use as a last resort
#start with the least dangerous one, i.e., SIGTERM=9,  
function sig_int(){
echo 'SIGINT caught. Exiting the current process.'
cd $current_dir
exit 0
}

function sig_hup(){
echo 'SIGHUP caught. Exiting the current process.'
cd $current_dir
exit 0
}

function sig_term(){
echo 'SIGTERM caught. Exiting the current process.'
cd $current_dir
exit 0
}




current_dir=$PWD
cd /home/sirius/Chakma/Gru_dev/GRU_SIRIUS/GUser-build/

trap 'sig_int' INT
trap 'sig_hup' HUP 
trap 'sig_term' TERM 
./SiriusControlApp "$1" 
cd $current_dir
