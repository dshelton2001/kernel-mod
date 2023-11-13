#!/bin/bash

###############
## Functions ##
###############
function build()
{
    make all

    [ -f lkmasg2.ko ]
    local fail=$?

    return $fail
}

function clean()
{
    make clean

    [ -f lkmasg2.ko ]
    local success=$?

    return $success
}

function insert()
{
    insmod lkmasg2.ko
}

function remove()
{
    rmmod lkmasg2.ko
}

function run()
{
    echo "== Test 1 - 1 write, 1 read =="
    ./test /dev/lkmasg2 message
    echo
    echo "== Test 2 - 2 write, 1 read =="
    ./test2 /dev/lkmasg2 testaroo wowie
    echo
    echo "== Test max - 2 writes at max length =="
    ./testmax /dev/lkmasg2
    echo
    echo "== Test buffer - 1 write to test buffers =="
    ./testbuffer /dev/lkmasg2
    echo
    echo "== Test read - 1 write, TWO reads =="
    ./testread /dev/lkmasg2 message
}

##########
## Main ##
##########
echo "####################################################"
echo "## Teehee!! Welcome to the Denoid's Test-all.sh!! ##"
echo "####################################################"
echo "Please make sure your Makefile is working before running this script."

echo ""
echo "####################"
echo "## Starting Build ##"
echo "####################"
build
fail=$?

if [ $fail = 0 ]
then
    echo "Build was successful"
else
    echo "Build was unsuccessful, exiting..."

    clean
    
    if [ $success = 0 ]
    then
        echo "Could not delete driver files. Check your makefile!"

        exit 130
    fi

    exit 129
fi

echo ""
echo "################"
echo "## Test Cases ##"
echo "################"
insert
run
remove

echo ""
echo "###################"
echo "## Final Cleanup ##"
echo "###################"
clean
success=$?

if [ $success = 1 ]
then
    echo "Cleanup successful.\n"
else
    echo "Could not delete driver files. Check your makefile!"

    echo "exiting..."

    exit 1
fi

echo "exiting..."