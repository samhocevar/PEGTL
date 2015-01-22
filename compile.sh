#!/bin/bash

echo
echo "Please customise $0 with your favourite editor [in case of errors]."
echo

set -ex

for i in *.cc; do
    g++-mp-4.3 -g -std=gnu++0x -pipe -O3 -Wall -Wextra -Wimplicit -Wconversion -Wcast-align -Woverloaded-virtual -Wold-style-cast -Wformat=2 -Wswitch-enum -Wswitch-default -Wredundant-decls -fno-enforce-eh-specs -fno-strict-overflow -D_REENTRANT -I. -I/opt/macports-1.6.0/include $i -o `basename $i .cc` -lpthread
done
