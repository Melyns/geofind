#!/bin/bash

# Compile
gcc -o geofind geofind.c -lcurl -ljson-c

# check if the compilation was successful
if [ $? -eq 0 ]; then
    # move the binary to /usr/local/bin
    sudo mv geofind /usr/local/bin
    echo "geofind was installed successfully."
else
    echo "Compilation failed."
    exit 1
fi
