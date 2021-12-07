#!usr/bin/bash

# This file will be used to make the benchmarks and print the result to stdout

TOEXEC=$1 # The command to execute
OUTFILE=$2 # The file in which we store the data
NARGS=$3 # Number of argument of the command
ALGO=$4 # Which lock algorithm to use
NOBS=5 # The number of observation to take
NPROC=$(grep -c ^processor /proc/cpuinfo) # Count the number of processor in the machine

# Make the computation of the parameters for each run easier
get_args() {
    local func_result=""
    if [ ${NARGS} -eq "1" ] 
    then 
        func_result="${TOEXEC} $1"
    else 
        func_result="${TOEXEC} $((($1 + 1)/2)) $(($1/2))" # Always favor the first parameter
    fi
    func_result+=" ${ALGO}"
    echo $func_result
}

# Contruct the headers : 1t,2t,...,2*NPROCt
OUT=""
for ((T=1; T<=$((2*${NPROC})); T++ ))
do 
    OUT+=",${T}t"
done
# ERASE the content and output the headers to the output file
echo "${OUT[@]:1}" > ${OUTFILE}

# Make the measurement
for ((i=1; i<=${NOBS}; i++))
do 
    OUT=""
    for ((N=1; N<=$((2*${NPROC})); N++)) 
    do
        OUT+=",$({ $(/usr/bin/time -f "%e" $(get_args ${N})); } 2>&1; )" # Redirect STDERR to ERR
    done
    echo "${OUT[@]:1}" >> ${OUTFILE}
done

# In case of an error, erase the output file
trap "rm ${OUTFILE}; echo 'An error has occured'; exit" SIGHUP SIGINT SIGTERM