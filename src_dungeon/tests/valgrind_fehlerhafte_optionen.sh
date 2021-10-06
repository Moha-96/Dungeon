#!/usr/bin/env bash

rm -rf valgrind
mkdir -p valgrind
mkdir -p ausgabe

input="eingabe/level1_1.txt"
output="ausgabe/test.txt"
level="level/1.txt"
invalid="Diese_Datei_sollte_nicht_lesbar_oder_schreibbar_sein.txt"
stderrfile="ausgabe/stderr.txt"
logfile="valgrind/log.txt"

rm -rf "$invalid"

touch "$invalid"
chmod -rw "$invalid"

function test_params() {
    expected_error_code="$1"

    (set -x; valgrind --leak-check=full --show-leak-kinds=all --log-file=$logfile ./dungeon "${@:2}" 2> "$stderrfile")
    return_value=$?

    printf '\tStandardfehlerausgabe:\n\t\t'
    cat "$stderrfile"

    if ! tests/valgrind_log_pruefen.sh "$logfile"
    then
        exit 1
    fi

    if [ $return_value -eq "$expected_error_code" ]
    then
        printf '\tOK: Rückgabewert ist %s wie erwartet.\n' "$expected_error_code"
    else
        printf '\tFEHLER: Rückgabewert ist nicht %s, sondern %s.\n\n' "$expected_error_code" "$return_value"
        exit 1
    fi

    if [ -s "$stderrfile" ] ; then
        printf '\tOK: Es wurde etwas auf der Standardfehlerausgabe ausgegeben.\n'
    else
        printf '\tFEHLER: Es wurde keine Fehlermeldung auf der Standardfehlerausgabe ausgegeben.\n'
        exit 1
    fi

    printf '\n'
}

printf 'Prüfe Behandlung von Level-Datei, die nicht zum Lesen geöffnet werden kann. Dieser Test setzt vorraus, dass Ihr Betriebssystem Dateizugriffsrechte respektiert.\n\n'
test_params '1' "$invalid"

printf 'Prüfe Behandlung von Eingabedatei, die nicht zum Lesen geöffnet werden kann.\n\n'
test_params '1' -i "$invalid"

printf "Prüfe Behandlung von nicht-schreibbarer Ausgabedatei $invalid.\n\n"
test_params '1' "$level" -i "$input" -o "$invalid"

printf 'Prüfe Behandlung von nicht-lesbarer Eingabedatei.\n\n'
test_params '2' -i 'eingabe'

printf 'Prüfe Behandlung von nicht-lesbarer Level-Datei.\n\n'
test_params '2' 'eingabe'

printf "Prüfe Behandlung von zu vielen Parametern.\n\n"
test_params '3' "$level" "$level"

printf "Prüfe Behandlung von ungültigen Optionen.\n\n"
test_params '3' -z "$output"

printf 'Prüfe Behandlung von doppelter Eingabedatei.\n'
test_params '3' -i "$input" -i "$input"

printf 'Prüfe Behandlung von doppelter Ausgabedatei.\n'
test_params '3' -o "$output" -o "$output"

printf 'Prüfe Behandlung von doppelter Eingabe- und Ausgabedatei.\n'
test_params '3' -i "$input" -i "$input" -o "$output" -o "$output"

rm -rf "$invalid"
