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

void QueueInstr(uint32_t instr)
{
	uint32_t 
		cond = instr >> 28,
		opr  = (instr >> 23) & 0xf,
		s    = (instr & ( 1 << 22)) != 0,
		rd   = (instr >> 17) & 0x1f,
		ra   = (instr >> 12) & 0x1f,
		imm7 = (instr >> 5)  & 0x7f,
		rb 	 =  instr  		 & 0x1f,
		imm12=  instr 		 & 0xfff,
		imm17=  instr 		 & 0x1ffff;
	
	printf("Queueing command .. \n");
	printf("%6s %6s %6s %6s %6s %6s %6s\n", "cond", "opr", "s", "rd", "ra", "imm7", "rb");
	printf("%6x %6x %6x %6x %6x %6x %6x\n", cond, opr, s, rd, ra, imm7, rb);
	printf("%6s %6s %6s %6s %6s %6s\n", "cond", "opr", "s", "rd", "ra", "imm12");
	printf("%6x %6x %6x %6x %6x %6x\n", cond, opr, s, rd, ra, imm12 );
	printf("%6s %6s %6s %6s %6s\n", "cond", "opr", "s", "rd", "imm17");
	printf("%6x %6x %6x %6x %6x\n", cond, opr, s, rd, imm17);
	
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_DATAOUT_BASE, instr);
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, 0); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, 1u << 31); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, 0); 
} 

#define CMD_RD 1
#define CMD_WR 2
#define CMD_STAT 4
#define CMD_CLK (1 << 31)
uint32_t ReadData(int ThreadIdx, int WordIdx)
{
	uint32_t cmd = (CMD_RD << 24) | (ThreadIdx << 16) | (WordIdx);
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, cmd); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, CMD_CLK|cmd); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, ~CMD_CLK&cmd); 
	return IORD_ALTERA_AVALON_PIO_DATA(PIO_DATAIN_BASE);
}

void WriteData(int ThreadIdx, int WordIdx, uint32_t Data)
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
    
	const int max_iter = 16;
	const int max_rot  = 4;
	for(int i = 0; i < max_rot; ++i)
	{
		for(int j = 0; j < max_iter; ++j)
		{
			WriteData(i, j, j == 0 ? i + 0x1000 : 0);
		}
	}
 
	printf("Init vector"); 
    uint32_t instrs[] = 
    {
	    GPPCU_ASSEMBLE_INSTRUCTION_C(COND_ALWAYS, OPR_MVI, FALSE, 0x1, 1),

	    GPPCU_ASSEMBLE_INSTRUCTION_C(COND_ALWAYS, OPR_MVI, FALSE, 0x0, 0),
	    GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_LDL, FALSE, 0x0, 0b0, 0, 0x0),
          
	    GPPCU_ASSEMBLE_INSTRUCTION_C(COND_ALWAYS, OPR_MVI, FALSE, 0x1, 1),
        GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_STL, FALSE, 0b0, 0x1, 1, 0x1),
	    GPPCU_ASSEMBLE_INSTRUCTION_C(COND_ALWAYS, OPR_MVI, FALSE, 0x1, 2),
	    GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_STL, FALSE, 0b0, 0x1, 2, 0x1),
	    GPPCU_ASSEMBLE_INSTRUCTION_C(COND_ALWAYS, OPR_MVI, FALSE, 0x1, 3),
	    GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_STL, FALSE, 0b0, 0x1, 3, 0x1),
	    GPPCU_ASSEMBLE_INSTRUCTION_C(COND_ALWAYS, OPR_MVI, FALSE, 0x1, 4),
	    GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_STL, FALSE, 0b0, 0x1, 4, 0x1),
	    GPPCU_ASSEMBLE_INSTRUCTION_C(COND_ALWAYS, OPR_MVI, FALSE, 0x1, 5),
	    GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_STL, FALSE, 0b0, 0x1, 5, 0x1),
    };
	
	int cc = 0;
	while(1)
	{ 
		++cc;
		
		printf("--- STEP %d ---\n", cc);
		for(int k = 0; k < countof(instrs); ++k)
		{
			QueueInstr(instrs[k]);
			 
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
					ReadData(rot, i);
					printf("%9x", ReadData(rot, i));
				}
				printf("\n");
			} 
		}
	} 

	return 0;
}
