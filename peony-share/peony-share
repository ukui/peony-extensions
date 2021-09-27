#!/bin/bash

if [ "$#" -eq 1 ];
then
    # list samba shared directory
    if [ "$1" == "list" ];
    then
        net usershare list
    fi
else
    net "$@"
fi

