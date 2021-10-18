#!/bin/bash

#if [ "$#" -eq 1 ];
#then
#    # list samba shared directory
#    if [ "$1" == "list" ];
#    then
#        net usershare list
#    fi
#else
#    net "$@"
#fi

if [ "$#" -eq 1 ];
then
    if [[ $(groups $1 | grep "sambashare") != ""  ]];
    then
#        echo "have!!!"
        exit 0
    else
        usermod -a -G sambashare "$1"
    fi
fi
