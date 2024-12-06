#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Usage: $0 <test_num>"
    exit 1
fi

make semaphores

ARG=$1

case "$ARG" in
    "1")
    echo "1 prod even"
    ./semaphores 1 0 0 0
    ;;
    "2")
    echo "1 prod odd"
    ./semaphores 0 1 0 0
    ;;
    "3")
    echo "1 cons even"
    ./semaphores 0 0 1 0
    ;;
    "4")
    echo "1 cons odd"
    ./semaphores 0 0 0 1
    ;;
    "5")
    echo "1 prod even + 1 prod odd"
    ./semaphores 1 1 0 0
    ;;
    "6")
    echo "1 cons even + 1 cons odd"
    ./semaphores 0 0 1 1
    ;;
    "7")
    echo "1 prod even + 1 cons even"
    ./semaphores 1 0 1 0
    ;;
    "8")
    echo "1 prod odd + 1 cons odd"
    ./semaphores 0 1 0 1
    ;;
    "9")
    echo "1 prod even + 1 cons even + 1 prod odd + 1 cons odd"
    ./semaphores 1 1 1 1
    ;;
    "10")
    echo "2 prod even + 2 cons even + 2 prod odd + 2 cons odd"
    ./semaphores 2 2 2 2
    ;;
    *)
    echo "Invalid argument: $ARG"
    echo "Valid arguments are: numbers from 1 to 10"
    exit 1
    ;;
esac