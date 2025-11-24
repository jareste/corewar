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
    linux="${base}_linux.cor"
    out="${base}.cor"
    printf '\033[1;36mTesting: %s\033[0m\n' "$src"


    # printf '\033[1;36mTesting asm_linux: %s -> %s\033[0m\n' "$src" "$out"
    asm_linux_err=$(mktemp)
    ./asm_linux "$src" > /dev/null 2> "$asm_linux_err"
    asm_linux_rc=$?
    if [ $asm_linux_rc -ne 0 ]; then
        echo "asm_linux exited with code ${asm_linux_rc} for ${src}"
        [ -s "$asm_linux_err" ] && sed -n '1,200p' "$asm_linux_err"
        ret=1
        rm -f -- "$asm_linux_err" "$linux"
        continue
    fi
    rm -f -- "$asm_linux_err"

    if [ ! -f "$out" ]; then
        echo "asm_linux failed to produce ${out}"
        ret=1
        rm -f -- "$linux"
        continue
    fi

    mv -f -- "$out" "$linux"


    asm_err=$(mktemp)
    ./asm "$src" > /dev/null 2> "$asm_err"
    asm_rc=$?
    if [ $asm_rc -ne 0 ]; then
        echo "asm exited with code ${asm_rc} for ${src}"
        [ -s "$asm_err" ] && sed -n '1,200p' "$asm_err"
        ret=1
        rm -f -- "$asm_err"
        continue
    fi
    rm -f -- "$asm_err"

    if [ ! -f "$out" ]; then
        echo "asm failed to produce ${out}"
        ret=1
        continue
    fi

    mv -f -- "$out" "$mine"

    # printf '\033[1;36mTesting asm_linux: %s -> %s\033[0m\n' "$src" "$out"
    # asm_linux_err=$(mktemp)
    # ./asm_linux "$src" > /dev/null 2> "$asm_linux_err"
    # asm_linux_rc=$?
    # if [ $asm_linux_rc -ne 0 ]; then
    #     echo "asm_linux exited with code ${asm_linux_rc} for ${src}"
    #     [ -s "$asm_linux_err" ] && sed -n '1,200p' "$asm_linux_err"
    #     ret=1
    #     rm -f -- "$asm_linux_err" "$mine"
    #     continue
    # fi
    # rm -f -- "$asm_linux_err"

    

    if cmp -s -- "$linux" "$mine"; then
        printf '\033[1;32mOK: %s\033[0m\n' "$src"
    else
        echo "DIFF: $src (files ${mine} != ${out})"
        ret=1
    fi

    rm -f -- "$mine"
    rm -f -- "$out"
done

exit $ret
