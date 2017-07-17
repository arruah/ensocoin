#!/bin/bash

BASENAME=$(dirname $(readlink -m $0))
BIN=$HOME/bin
ENSODIR=$HOME/.ensocoin

if [ -d $ENSODIR ]; then
    echo -n "Ensocoin home directory already exists. Do you want to remove it? (y/n)"
    read -r ANSWER

    if [[ "$ANSWER" == "y" ]]; then
        rm -rf $ENSODIR
    fi
fi

if [ ! -d $ENSODIR ]; then
    mkdir $ENSODIR
    cp -f ensocoin/* $ENSODIR
fi

mkdir -p $BIN

ENSOCOIN=$(which ensocoin-qt)
if [ -z "$ENSOCOIN" ]; then
    cp -f $BASENAME/bin/ensocoin-qt $BIN/
fi

ENSOCOIN=$(which ensocoind)
if [ -z "$ENSOCOIN" ]; then
    cp -f $BASENAME/bin/ensocoind $BIN/
fi

ENSOCOIN=$(which ensocoin-cli)
if [ -z "$ENSOCOIN" ]; then
    cp -f $BASENAME/bin/ensocoin-cli $BIN/
fi

ENSOCOIN=$(which ensocoin-tx)
if [ -z "$ENSOCOIN" ]; then
    cp -f $BASENAME/bin/ensocoin-tx $BIN/
fi

$BIN/ensocoin-qt
