#!/usr/bin/env bash

program='./dungeon_fsanitize'

rm -rf ausgabe
rm -rf diff
mkdir -p diff
mkdir -p ausgabe

printf '\n'
printf 'Die Stackgröße wird jetzt beschränkt. Code, der zu große Arrays auf dem Stack anlegt,\n'
printf 'anstatt diese dynamisch mit malloc zu verwalten, sollte deshalb fehlschlagen.\n'
printf 'Aufgrund der Größe der Testlevel kann dieser Schritt bei ineffizientem Code sehr lange dauern.\n'
printf 'Zum Bestehen darf "make run" nicht länger als 5 Minuten benötigen.\n\n'

function check_stack() {
    level="$1"
    input="$2"
    output="$3"
    expected="$4"
    diff_file="diff/stack.txt"
    
    # Limit stack size
    ulimit -Ss 64

    (set -x ; "$program" -i "$input" -o "$output" "$level")

    return_value="$?"

    # Reset stack size limit to default (probably 8192k)
    ulimit -Ss unlimited

    if [ $return_value -eq 0 ]; then
        printf '\tOK: Rückgabewert ist 0.\n'
    else
        printf '\tFEHLER: Rückgabewert ist nicht 0, sondern %s.\n' "$return_value"
        exit 6
    fi

    if diff -yZ "$output" "$expected" > "$diff_file"
    then
        printf '\tOK: %s stimmt mit %s überein.\n' "$expected" "$output"
    else
        printf '\tFEHLER: %s stimmt nicht mit %s überein. Details in %s.\n' "$expected" "$output" "$diff_file"
        exit 7
    fi

    printf '\n'
}

check_stack 'level/1.txt' 'eingabe/level1_1.txt' 'ausgabe/level1_1.txt' 'erwartete_ausgabe/level1_1.txt'
check_stack 'level/10.dateiendungen_pruefen_gilt_nicht' 'eingabe/level10_1.foo' 'ausgabe/level10_1.asdf' 'erwartete_ausgabe/level10_1.txt'
check_stack 'level/11' 'eingabe/level11_1.bar' 'ausgabe/level10_1.123.456' 'erwartete_ausgabe/level11_1.foo'

printf 'Die Tests überprüfen nicht die Einhaltung der Code-Vorgaben aus dem Lernmodul,\n'
printf 'also zum Beispiel Verbot von globalen Variablen, höchstens 100 Zeichen pro Zeile,\n'
printf 'höchstens 60 Zeilen pro Funktion, Kommentare, usw. Darauf müssen Sie selber achten.\n\n'
