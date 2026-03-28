#!/usr/bin/env bash
set -u

if [ $# -eq 0 ]; then
    set -- ./*.s
fi

ret=0
asm_linux_err_count=0
asm_err_count=0
ok_count=0
ko_count=0

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
        asm_linux_err_count=$((asm_linux_err_count + 1))
        rm -f -- "$asm_linux_err" "$linux"
        continue
    fi
    rm -f -- "$asm_linux_err"

    if [ ! -f "$out" ]; then
        echo "asm_linux failed to produce ${out}"
        ret=1
        rm -f -- "$linux"
        asm_linux_err_count=$((asm_linux_err_count + 1))
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
        asm_err_count=$((asm_err_count + 1))
        rm -f -- "$asm_err"
        continue
    fi
    rm -f -- "$asm_err"

    if [ ! -f "$out" ]; then
        echo "asm failed to produce ${out}"
        ret=1
        asm_err_count=$((asm_err_count + 1))
        continue
    fi

    mv -f -- "$out" "$mine"

    if cmp -s -- "$linux" "$mine"; then
        printf '\033[1;32mOK: %s\033[0m\n' "$src"
        ok_count=$((ok_count + 1))
    else
        echo "DIFF: $src (files ${mine} != ${out})"
        ko_count=$((ko_count + 1))
        ret=1
    fi

    rm -f -- "$mine"
    rm -f -- "$out"
done

echo "Summary:"
echo "  asm_linux errors: $asm_linux_err_count"
echo "  asm errors:       $asm_err_count"
echo "  OK:               $ok_count"
echo "  KO:               $ko_count"

exit $ret
