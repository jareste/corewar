#!/usr/bin/env bash
set -u

if [ $# -eq 0 ]; then
    set -- ./*.s
fi

ret=0

for src in "$@"; do
    [ -f "$src" ] || { echo "Skipping missing: $src"; continue; }
    case "$src" in
        *.s) ;;
        *) echo "Skipping non-.s: $src"; continue;;
    esac

    base="${src%.*}"
    mine="${base}_mine.cor"
    out="${base}.cor"

    echo "Testing: $src"

    ./asm "$src" 2> /dev/null 1> /dev/null
    if [ ! -f "$out" ]; then
        echo "asm failed to produce ${out}"
        ret=1
        continue
    fi

    mv -f -- "$out" "$mine"

    ./asm_linux "$src"
    if [ ! -f "$out" ]; then
        echo "asm_linux failed to produce ${out}"
        ret=1
        rm -f -- "$mine"
        continue
    fi

    if cmp -s -- "$mine" "$out"; then
        echo "OK: $src"
    else
        echo "DIFF: $src (files ${mine} != ${out})"
        ret=1
    fi

    rm -f -- "$mine"
    rm -f -- "$out"
done

exit $ret
