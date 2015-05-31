#!/bin/bash


if [ -z ${SEQHOUND_HOME} ]
then
    echo "SEQHOUND_HOME not set. Please set SEQHOUND_HOME and try again"
    exit 1
fi

( cd ${SEQHOUND_HOME}/install/example && make -f make.example )
