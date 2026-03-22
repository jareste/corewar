.name "forker"
.comment "fork + live spam"

start:
    fork    %:loop
    fork    %:loop
    fork    %:loop

loop:
    ld      %0, r2
    live    %1
    zjmp    %:loop
