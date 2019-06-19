#include <system.h>
#include <stdio.h>
#include "gppcu.h"
#include <altera_avalon_pio_regs.h>
#include <nios2.h>

#define countof(v) (sizeof(v)/sizeof(*v))
#define BOOL int
#define TRUE 1
#define FALSE 0

void wait(int val)
{
	for(int i =0; i < val; ++i)
	{
		// nop;
	}
}

int main()
{ 
	printf("Hello from Nios II! ... Launching ... \n");
    
 
	printf("Init vector"); 
    uint32_t instrs[] = 
    {
	    GPPCU_ASSEMBLE_INSTRUCTION_C(COND_ALWAYS, OPR_MVI, FALSE, 0x1, 0),

	    GPPCU_ASSEMBLE_INSTRUCTION_C(COND_ALWAYS, OPR_MVI, FALSE, 0x2, 0),
	    GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_LDL, FALSE, 0x0, 0, 0, 0x2),
           
        GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_STL, FALSE, 0b0, 0x0, 1, 0x1), 
	    GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_STL, FALSE, 0b0, 0x0, 2, 0x1), 
	    GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_STL, FALSE, 0b0, 0x0, 3, 0x1), 
	    GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_STL, FALSE, 0b0, 0x0, 4, 0x1),
	    GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_STL, FALSE, 0b0, 0x0, 5, 0x1),
	    GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_LDL, FALSE, 0x2, 0b0, 5, 0x1),
	    GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_LSL, FALSE, 0x2, 0x2, 0, 0x2),
	    GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_STL, FALSE, 0b0, 0x2, 6, 0x1),
    };
	
	int cc = 0;
	while(1)
	{ 
		++cc;
		
		printf("--- STEP %d ---\n", cc);
		const int max_iter = 16;
		const int max_rot  = 4;
		
		for(int i = 0; i < max_rot; ++i)
		{
			for(int j = 0; j < max_iter; ++j)
			{
				gppcu_data_wr(i, j, j == 0 ? (i << 16) + cc : 0);
			}
		}
		
		for(int v = 0; v < 128; ++v)
		{
			gppcu_queue_instr(0);
		}
		for(int k = 0; k < countof(instrs); ++k)
		{
			gppcu_queue_instr(instrs[k]); 
		}
        {
            uint16_t head, tail;
            gppcu_stat_queue(&head, &tail);
            printf("stat queue ... h: %d, t: %d\n", head, tail);
            wait(5000000);
        }
        
        for(int rot = 0; rot < max_rot; ++rot)
        {
            printf("for thread %d\n", rot);
            for(int i=0; i<max_iter; ++i)
            {
                printf("%9x", i);
            }
            printf("\n");
            for(int i=0; i<max_iter; ++i)
            {
                gppcu_data_rd(rot, i);
                printf("%9x", gppcu_data_rd(rot, i));
            }
            printf("\n");
        } 
	} 

	return 0;
}
