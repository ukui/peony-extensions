#!/bin/bash

if [ "$#" -ge 1 ];
then
    if [[ $(groups $1 | grep "sambashare") == ""  ]];
    then
        usermod -a -G sambashare "$1"
    fi
fi

if [ "$#" -ge 2 ];
then
    if [[ $(echo "$2" | grep "/home/$1") != ""  ]];
    then
        umsk=$(umask)
        set umask 0
        chmod o+x "/home/$1"
        set umask umsk
    fi
fi
