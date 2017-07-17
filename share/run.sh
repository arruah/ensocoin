#!/bin/bash

BASENAME=$(dirname $(readlink -m $0))
BIN=$HOME/bin
ENSODIR=$HOME/.ensocoin

if [ ! -d $ENSODIR ]; then
    echo "Ensocoin home directory doen't exist. Please run install."
    exit 1
fi

ENSOCOIN=$(which ensocoin-qt)
if [ ! -z "$ENSOCOIN" ]; then
    $BIN/ensocoin-qt
else
    echo "Ensocoin program not installed. Please run install."
    exit 1
fi

