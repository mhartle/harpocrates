# Harpocrates

## Overview

Harpocrates is a pair of scripts for backup and restore of (small,
critically important) files on paper (yes, paper), such as
cryptographic keys:

* You can run paperbackup.sh for a file to be backed up to
  create a PDF document containing a two-column hexdump with many rows
  of numbers and characters. Each row consists of an address (2 bytes),
  the CRC16 checksum (2 bytes) covering all previous data bytes and the
  following, remaining data bytes of this row, and the data bytes (up
  to 14 bytes) themselves, grouped into 2-byte blocks for easier reading.

  Print the created PDF document and store the printout in a safe
  location to protect against the original files becoming corrupted
  or lost.

* If you ever need to restore the file from paper, retrieve its printout
  and manually enter the CRC16 checksum (without the brackets) and the 
  following data bytes of each row into a text file using a text editor.
  This may take a while, and may introduce one or more errors (we are
  human, after all).

  Once you have entered all rows as described above, you can run
  paperrestore.sh to check for typing errors in your text file using
  the CRC16 checksums, and restore the original file from the data.
  In case of a typing error or an omitted line, the computed CRC16
  checksum does not match the entered CRC16 checksum in the vast
  majority of cases. If a CRC error is observed, it is reported with
  a row and byte range, so you can compare the printout to the text
  file, fix the error and continue.

For that purpose, Harpocrates depend on the availability of hexdump,
enscript, ps2pdf and xxd as well as on its own crcwrap and crcunwrap,
two commandline tools that come as part of Harpocrates:

* crcwrap reads blocks of up to 14 data bytes as they exist,
  computes a continuous 2-byte CRC16 checksum, and writes the checksum
  followed by the data bytes.

* crcunwrap reads pairs of a 2-byte CRC16 checksum and a block of up to
  14 data bytes as they exist, computes its own CRC16 checksum over all
  data bytes up to and including this block, and compares the checksums.
  If the checksums match, crcunwrap writes just the data bytes,
  otherwise crcunwrap aborts.

## Build

Run `make` to build crcwrap and crcunwrap as a prerequisite for
using paperbackup.sh and paperrestore.sh.

## Usage

Run `./paperbackup.sh <data-file>` in the same directory to produce a
<data-file>.wrapped.pdf containing the CRC-protected hexdump.

Run `./paperrestore.sh <text-file>` in the same directory to produce a
<text-file>.restored containing the restored file.

## Hints

* If you want to back up cryptographic keys and certificates, consider
  backing up DER-encoded representations rather than their larger
  base64-wrapped PEM-encoded alternatives.

## Recommendation

Before using these scripts in production, do actually test whether
these work as intended. Although this is used by the author, no guarantee
is given that this software serves any purpose at all; and you would
not put blind faith into the tools of a total stranger safeguarding
your most important files, would you?

You can test it by backing up a test-file and entering only 1-2 rows
of data into a text file; running paperrestore.sh on that partial file
will convert these rows to binary data, which can be checked by
comparing the first lines of their corresponding hexdumps, e.g. by

```
./paperrestore <test-file.txt>
hd <test-file.txt>.restored | head -4
hd <test-file.original> | head -4
```

## Limitations

* The CRC16 checksum used to protect every 14-byte block may detect
  certain types of errors, but it cannot detect each and every possible
  alteration to the data bytes.

* The usage of a continuous CRC16 checksum provides some protection
  against omitted and duplicated rows, but there is no protection
  against missing tailing rows, as no metadata is stored regarding the
  total row count.

