.name "jareste"
.comment "hola"

start:
    fork %:alive
    fork %:alive
    fork %:alive
    fork %:alive
    fork %:bomber
    fork %:bomber

    zjmp %:bomber

alive:
    live %1
    zjmp %:alive

bomber:
    ld %0, r2
    ld %1, r5
    ld %0, r3

bomb_loop:
    sti r2, r3, %0

    add r3, r5, r3

    and r1, %0, r1
    zjmp %:bomb_loop

