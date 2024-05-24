#!/bin/bash


cd bin

for ((i = 1; i <= $1; i++))
do
    TIME=$(((RANDOM % 20000) + 5000))
    SEGUNDOS=$(( (TIME / 1000) + 1))

    # Verifica se o número de segundos é par ou ímpar
    if [ $((SEGUNDOS % 2)) -eq 0 ]; then
        echo "./client execute $TIME -u \"./hello $SEGUNDOS\""
        ./client execute $TIME -u "./hello $SEGUNDOS" 
    else
        echo "./client execute $TIME -p \"./hello $SEGUNDOS | tail -n 100 | head -n 10 | grep "paths" | wc -l \""  
        ./client execute $TIME -p "./hello $SEGUNDOS | tail -n 100 | head -n 10 | grep "paths" | wc -w"         
    fi
done

