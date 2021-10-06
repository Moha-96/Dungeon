#!/usr/bin/env bash

function check_disallowed() {
    disallowed="$1"
    if grep -rF --include='*.c' --include='*.h' "$disallowed" .
    then
        printf '\n'
        printf 'FEHLER: Die Verwendung von %s ist nicht erlaubt, da nicht Teil des C99-Standards.\n' "$disallowed"
        printf 'Falls Sie durch das Kapitel "16.26 Low-Level-Datei-I/O-Funktionen ---> (nicht ANSI C) <---"\n'
        printf 'aus dem Buch "C von A bis Z" auf diese Idee gekommen sind, dann haben Sie möglicherweise\n'
        printf 'eine interessante Funktion, die vorher in Kapitel 16 genannt wurde, verpasst.\n\n'
        exit 9
    fi
}

check_disallowed 'fcntl.h'
check_disallowed '<io.h>'
check_disallowed 'sys/types.h'
check_disallowed 'sys/stat.h'
check_disallowed 'sys/unistd.h'
check_disallowed 'S_ISREG'
check_disallowed 'S_ISDIR'
check_disallowed 'F_OK'
check_disallowed 'R_OK'
check_disallowed 'W_OK'
check_disallowed 'X_OK'
check_disallowed 'O_CREAT'
check_disallowed 'O_WRONLY'
check_disallowed 'POSIX_SOURCE'
check_disallowed 'GNU_SOURCE'
