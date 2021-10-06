#!/usr/bin/env bash

program='./dungeon_fsanitize'

rm -rf ausgabe
rm -rf diff
mkdir -p diff
mkdir -p ausgabe

for name in 1_1 1_2 1_3 2_1 2_2 2_3 2_4 3_1 3_2 4_1 4_2 5_1 6_1 7_1 7_2 7_3 8_1 9_1 9_2
do
    level="level/$(echo $name | cut -d_ -f1).txt"
    input="eingabe/level$name.txt"
    output="ausgabe/level$name.txt"
    expected="erwartete_ausgabe/level$name.txt"
    diff_file="diff/level$name.txt"

    (set -x ; "$program" -i "$input" -o "$output" "$level")

    return_value="$?"

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
done
