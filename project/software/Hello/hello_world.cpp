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
#include <stdlib.h>
#include <cstring>
#include "src/chout.hpp"

#define LPDISPLAY ((volatile uint32_t*)BUFFERRAM_OUT_BASE)

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

	memset((void*)LPDISPLAY, 0, 48000*sizeof(alt_u32));
	skConsole Console(400, 240);
	while(1)
	{
		printf("Enter any.\n");
		char buff[1024];
		gets(buff);
		Console.Write(buff);
		Console.Print(LPDISPLAY);
		
		// col = 0;
		// while(col < 48000)
		// {
		// 	LPDISPLAY[col] = ((col << 16) | col);
		// 	++col;
		// }
		/*
		for(int i = 0; i < 240; ++i)
		{
			int addr = i * 200;
			for(int j = 0; j < 200; ++j, ++addr)
			{
				if(i < 40)
				{

				}
			}
		}
		*/
	}
	printf("done.\n");
	while(1);

	return 0;
}
