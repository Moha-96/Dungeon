#!/usr/bin/env bash

rm -rf ausgabe diff
mkdir -p ausgabe diff

./dungeon < eingabe/level1_1.txt > ausgabe/level1_1.txt
./dungeon level/1.txt -i eingabe/level1_2.txt -o ausgabe/level1_2.txt
./dungeon -o ausgabe/level1_3.txt -i eingabe/level1_3.txt level/1.txt
./dungeon level/2.txt -i eingabe/level2_1.txt > ausgabe/level2_1.txt
./dungeon -i eingabe/level2_2.txt level/2.txt -o ausgabe/level2_2.txt
./dungeon level/2.txt -o ausgabe/level2_3.txt < eingabe/level2_3.txt
./dungeon level/2.txt -i eingabe/level2_4.txt -o ausgabe/level2_4.txt

for name in 1_1 1_2 1_3 2_1 2_2 2_3 2_4
do
    input="eingabe/level$name.txt"
    output="ausgabe/level$name.txt"
    expected="erwartete_ausgabe/level$name.txt"
    diff_file="diff/level$name.txt"

    if diff -yZ "$output" "$expected" > "$diff_file"
    then
        printf '\tOK: %s stimmt mit %s überein.\n' "$expected" "$output"
    else
        printf '\tFEHLER: %s stimmt nicht mit %s überein. Details in %s.\n' "$expected" "$output" "$diff_file"
        exit 1
    fi
done
