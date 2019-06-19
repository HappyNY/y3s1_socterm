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

#define CMD_RD 1
#define CMD_WR 2
#define CMD_STAT 4
#define CMD_CLK (1 << 31)

static inline void gppcu_queue_instr(uint32_t instr)
{  
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_DATAOUT_BASE, instr);
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, 0); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, CMD_CLK); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, 0);  
} 

static inline uint32_t gppcu_data_rd(int ThreadIdx, int WordIdx)
{
	uint32_t cmd = (CMD_RD << 24) | (ThreadIdx << 16) | (WordIdx);
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, cmd); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, CMD_CLK|cmd); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, ~CMD_CLK&cmd); 
	return IORD_ALTERA_AVALON_PIO_DATA(PIO_DATAIN_BASE);
}

static inline void gppcu_data_wr(int ThreadIdx, int WordIdx, uint32_t Data)
{
	uint32_t cmd = (CMD_WR << 24) | (ThreadIdx << 16) | (WordIdx);
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_DATAOUT_BASE, Data); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, cmd); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, CMD_CLK|cmd); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, ~CMD_CLK&cmd);  
}

void QueueStat(uint16_t* phead, uint16_t* ptail)
{
	uint32_t cmd = (CMD_STAT << 24);
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, cmd); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, CMD_CLK | cmd); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, ~CMD_CLK | cmd); 
	uint32_t ret = IORD_ALTERA_AVALON_PIO_DATA(PIO_DATAIN_BASE);
    *phead = ((uint16_t*)&ret)[1];
    *ptail = ((uint16_t*)&ret)[0];
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
            QueueStat(&head, &tail);
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
