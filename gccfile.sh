#!/bin/bash

gcc master.c -o master
gcc drone.c -o drone
gcc input.c -lncurses -o input
gcc server.c -lncurses -o server
gcc wd.c -o wd