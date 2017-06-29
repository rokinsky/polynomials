#!/bin/bash

# Skrypt bashowy automatyzujący wykonywanie zapisanych w plikach sekwencji operacji na wielomianach.

RED="\033[1;31m"
DEFAULT="\033[0m"

if [ "$#" -eq 2 ]
then
    PROG=$1
    DIR=$2
    if ! [ -f "$PROG" -o -x "$PROG" ]
    then
        echo -e "${RED}ERROR: podana nazwa programu nie wskazuje na plik wykonywalny${DEFAULT}"
        exit 1
    elif ! [ -f "$DIR" -o -d "$DIR" ]
    then
        echo -e "${RED}ERROR: podana nazwa katalogu nie wskazuje na katalog${DEFAULT}"
        exit 1
    fi
else
    echo -e "${RED}ERROR: zła liczba parametrów${DEFAULT}"
    exit 1
fi

s="$(find "$DIR" -exec grep "^START" -l \{\} \; 2>/dev/null)"
cat <(grep . <(sed "$s" -e "s/START//")) >_tmp.in
touch _tmp.out
while true
do
    l="$(tail -n 1 _tmp.in)"
    cat <_tmp.out >_tmp2.in
    cat <(grep . <(sed _tmp.in -e "s/$l//")) >>_tmp2.in
    ./$PROG <_tmp2.in > _tmp.out
    if [[ "$l" == "STOP" ]]
    then
        cat _tmp.out
        rm _tmp*
        exit 0
    else
        f="$(sed <(echo "$l") -e "s/FILE //")"
        cat <"$DIR/$f" >_tmp.in
    fi
done
