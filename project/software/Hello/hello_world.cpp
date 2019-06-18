/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

#include <stdio.h>
#include <alt_types.h>
#include <system.h>
#include <altera_avalon_pio_regs.h>
#include <stdlib.h>
#include <cstring> 
#include "src/chout.hpp"
#include "src/gppcu.hpp" 
#include "nios2.h"

#define LPDISPLAY ((volatile uint32_t*)BUFFERRAM_OUT_BASE)
#define LPCMD ((volatile uint32_t*)PIO_CMD_BASE)
#define LPDATI ((volatile uint32_t*)PIO_DATAIN_BASE)
#define LPDATO ((volatile uint32_t*)PIO_DATAOUT_BASE)

void wait(int val)
{
	for(int i =0; i < val; ++i)
	{
		// nop;
	}
}
 
void QueueInstr(skGppcu::instr_t instr)
{  
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, 0); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_DATAOUT_BASE, instr);
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, 1u << 31); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, 0); 
} 

#define CMD_RD 2
#define CMD_WR 1
uint32_t ReadData(int ThreadIdx, int WordIdx)
{
	uint32_t cmd = (CMD_RD << 24) | (ThreadIdx << 16) | (WordIdx);
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, cmd); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, (1u<<31)|cmd); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, ~(1u<<31)&cmd); 
	return IORD_ALTERA_AVALON_PIO_DATA(PIO_DATAIN_BASE);
}

void WriteData(int ThreadIdx, int WordIdx, uint32_t Data)
{
	uint32_t cmd = (CMD_WR << 24) | (ThreadIdx << 16) | (WordIdx);
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_DATAOUT_BASE, Data); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, (1u<<31)|cmd); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, ~(1u<<31)&cmd);  
}

int main()
{ 
	printf("Hello from Nios II! ... Launching ... \n");

	memset((void*)LPDISPLAY, 0, 96000*sizeof(*LPDISPLAY));
	skConsole Console(400, 240); 

	const int max_iter = 15;
	const int max_rot  = 8;
	for(int i = 0; i < max_rot; ++i)
	{
		WriteData(i, 0, 0x1000 + i);
	}
	
	using namespace GPPCU; 
	
	wait(1000); 
	
	int cc = 0;
#define R(v) v
	while(1)
	{
		QueueInstr(GPPCU_ASSEMBLE_INSTRUCTION_C(COND_ALWAYS, OPR_MVI, 0, R(0), ++cc));
		QueueInstr(GPPCU_ASSEMBLE_INSTRUCTION_C(COND_ALWAYS, OPR_MVI, 0, R(1), cc));
		QueueInstr(GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_ITOF, 0, R(2), R(0), 0, R(0)));
		QueueInstr(GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_ADC, 0, R(3), R(1), 0, R(0)));
		QueueInstr(GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_ADC, 0, R(3), R(3), 0, R(0)));
		QueueInstr(GPPCU_ASSEMBLE_INSTRUCTION_C(COND_ALWAYS, OPR_MVI, 0, R(4), 8));
		QueueInstr(GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_STL, 0, R(0), R(3), 0, R(4)));
		QueueInstr(GPPCU_ASSEMBLE_INSTRUCTION_C(COND_ALWAYS, OPR_MVI, 0, R(4), 7));
		QueueInstr(GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_STL, 0, R(0), R(2), 0, R(4)));
		QueueInstr(GPPCU_ASSEMBLE_INSTRUCTION_C(COND_ALWAYS, OPR_MVI, 0, R(4), 6));
		QueueInstr(GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_STL, 0, R(0), R(1), 0, R(4)));
		QueueInstr(GPPCU_ASSEMBLE_INSTRUCTION_C(COND_ALWAYS, OPR_MVI, 0, R(0), 0));
		QueueInstr(GPPCU_ASSEMBLE_INSTRUCTION_C(COND_ALWAYS, OPR_LDL, 0, R(11), R(0)));
		QueueInstr(GPPCU_ASSEMBLE_INSTRUCTION_C(COND_ALWAYS, OPR_MVI, 0, R(1), 1));
		QueueInstr(GPPCU_ASSEMBLE_INSTRUCTION_A(COND_ALWAYS, OPR_STL, 0, R(0), R(11), 0, R(1)));
		
		wait(10000000);
		
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
	
	while(1)
	{
		// printf("Enter any.\n");
		// char buff[1024];
		// gets(buff);
		wait(1000);
		Console.Write("a");
		Console.Print(LPDISPLAY);
		Console.ForeColor--;
		Console.BackColor = 0xffffu - Console.ForeColor + 1;  
	}
	printf("done.\n");
	while(1);

	return 0;
}
