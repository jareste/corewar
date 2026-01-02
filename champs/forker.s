.name "forker"
.comment "fork + live spam"

start:
    fork    %:loop
    fork    %:loop
    fork    %:loop

loop:
    live    %1
    zjmp    %:loop
