#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "decode.h"
#include "corewar.h"
#include "log.h"
// #include "exec.h"
#include "process.h"

void init_vm(t_vm* vm)
{
    memset(vm->memory, 0, MEM_SIZE);
    vm->procs = NULL;
    vm->cycle = 0;
    vm->cycle_to_die = CYCLE_TO_DIE;
    vm->last_check_cycle = 0;
    vm->last_alive_player = -1;
}

void load_champ_into_vm(t_vm* vm, t_champ* champ, int player_id)
{
    t_proc* proc;
    int offset;
    
    offset = (MEM_SIZE / MAX_PLAYERS) * player_id;
    memcpy(&vm->memory[offset], champ->code, champ->size);

    proc = create_process(new_pid(), offset, player_id);
    
    log_msg(LOG_I, "Adding %p at offset %p\n", vm->procs, &vm->procs);
    FT_LIST_ADD_FIRST(&vm->procs, proc);

    vm->last_alive_player = player_id;

    log_msg(LOG_I, "Loaded champion '%s' into VM at offset %d\n", champ->name, offset);
}

void m_run(t_vm* vm)
{
    t_proc* proc;

    while (1)
    {
        vm->cycle++;
        
        proc = vm->procs;
        while (proc)
        {
            step_proc(vm, proc);

            proc = FT_LIST_GET_NEXT(&vm->procs, proc);
        }

        if (vm->cycle % vm->cycle_to_die == 0)
        {
            log_msg(LOG_I, "Cycle %d: Performing cycle to die check\n", vm->cycle);
            proc_check_deads(vm);
        }

        // log_msg(LOG_D, "Cycle %d completed\n", vm->cycle);
    }
}

int main(int argc, char **argv)
{
    t_champ champ;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
        return 1;
    }

    if (log_init() != 0)
    {
        fprintf(stderr, "Error: Could not initialize logging system\n");
        return 1;
    }

    if (decode_file(argv[1], &champ) != 0)
    {
        fprintf(stderr, "Error: Failed to decode file %s\n", argv[1]);
        return 1;
    }
    t_vm vm;
    init_vm(&vm);
    load_champ_into_vm(&vm, &champ, 0);

    m_run(&vm);

    return 0;
}
