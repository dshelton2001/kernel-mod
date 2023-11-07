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
    ./test /dev/lkmasg2
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
    echo "Cleanup successful."
else
    echo "Could not delete driver files. Check your makefile!"

    echo "exiting..."

    exit 1
fi

echo "exiting..."