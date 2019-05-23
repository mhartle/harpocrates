#!/bin/bash

# Ensure availability of xxd
if ! [ -x "$(command -v xxd)" ]; then
  echo 'Error: xxd is not installed.' >&2
  exit 1
fi

# Ensure availability of crcunwrap
if ! [ -x "$(command -v ./crcunwrap)" ]; then
  echo 'Error: crcunwrap has not been built.' >&2
  exit 1
fi

xxd -r -p $1 > $1.wrapped
./crcunwrap -i $1.wrapped -o $1.restored
