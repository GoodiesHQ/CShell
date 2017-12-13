#!/bin/bash
if [[ -z $1 ]]; then
    echo "$0 <ELF file>"
    exit 1
fi

if [[ -z $2 ]]; then
    COLS=18
else
    COLS=$2
fi

FILE="$1"
SECTION=".text"

CMD=$(exec objdump -h $FILE | grep $SECTION | awk '{print "dd if='$FILE' bs=1 count=$[0x" $3 "] skip=$[0x" $6 "]"}')
echo "const char shellcode[] = {"
eval $CMD 2>/dev/null | xxd -i -c $COLS
echo "};"
