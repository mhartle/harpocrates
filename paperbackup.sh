#!/bin/bash

# Ensure availability of crcwrap
if ! [ -x "$(command -v ./crcwrap)" ]; then
  echo 'Error: crcwrap has not been build.' >&2
  exit 1
fi

# Ensure availability of hexdump
if ! [ -x "$(command -v hexdump)" ]; then
  echo 'Error: hexdump is not installed.' >&2
  exit 1
fi

# Ensure availability of enscript
if ! [ -x "$(command -v enscript)" ]; then
  echo 'Error: enscript is not installed.' >&2
  exit 1
fi

# Ensure availability of ps2pdf
if ! [ -x "$(command -v ps2pdf)" ]; then
  echo 'Error: ps2pdf is not installed.' >&2
  exit 1
fi

# Wrap data with CRC
./crcwrap -i $1 -o $1.wrapped

# Convert wrapped data into hexdump
hexdump -ve '"%04_ax [" 2/1 "%02x" "] " 1/1 "%02x" 1/1 "%02x " 1/1 "%02x" 1/1 "%02x " 1/1 "%02x" 1/1 "%02x " 1/1 "%02x" 1/1 "%02x " 1/1 "%02x" 1/1 "%02x " 1/1 "%02x" 1/1 "%02x " 1/1 "%02x" 1/1 "%02x " "\n"' $1.wrapped > $1.wrapped.hd

# Convert hexdump to postscript
enscript --header="Hexdump (2x address, 2x CRC-16, 14x raw data) of $1" --font=Courier9 -p$1.wrapped.ps -2 $1.wrapped.hd

# Convert postscript to pdf
ps2pdf $1.wrapped.ps $1.wrapped.pdf
