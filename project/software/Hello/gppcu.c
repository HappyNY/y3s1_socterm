#include "gppcu.h"
#include "utils.h"
#include <stdlib.h>

void gppcu_stat_queue(uint16_t* phead, uint16_t* ptail)
{
    enum { GPPCU_CMD_RD =  1 };
    enum { GPPCU_CMD_WR =  2 };
    enum { GPPCU_CMD_STAT =  4 };
    enum { GPPCU_CMD_CLK =  (1 << 31) };
    
	uint32_t cmd = (GPPCU_CMD_STAT << 24);
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, cmd); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, GPPCU_CMD_CLK | cmd); 
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_CMD_BASE, ~GPPCU_CMD_CLK | cmd); 
	uint32_t ret = IORD_ALTERA_AVALON_PIO_DATA(PIO_DATAIN_BASE);
    *phead = ((uint16_t*)&ret)[1];
    *ptail = ((uint16_t*)&ret)[0];
} 
 
void gppcu_init(swk_gppcu_stat* const pp, int32_t num_threads, int32_t Capacity, int32_t MaxWordPerThread)
{
    pp->mcap = Capacity;
    pp->marr = malloc(Capacity * sizeof(swk_gppcu_data_t));
    pp->mnum = 0;
    pp->mnumthr = num_threads;
    pp->mtaskcycle = 0;
    pp->mtaskmem = 0;
	pp->ro_numtask = 0;
	pp->ro_max_word_per_thread = MaxWordPerThread;
}

void gppcu_init_task(swk_gppcu_stat *const pp, uint8_t WordsPerTask, uint16_t NumTasks)
{
	pp->ro_numtask = NumTasks;
	pp->mtaskcycle = NumTasks / pp->mnumthr;
	pp->mtaskmem = WordsPerTask;
	pp->ro_taskmaxcycle = pp->ro_max_word_per_thread / WordsPerTask;
	passert(NumTasks < pp->ro_taskmaxcycle * pp->mnumthr, "Task number has set too many.");
}

void gppcu_destroy(swk_gppcu_stat* const pp)
{
	free(pp->marr);
}

void gppcu_clear_instr(swk_gppcu_stat* const pp);

void gppcu_write(
    swk_gppcu_stat* pp, 
    swk_gppcu_data_t const* const data, 
    uint8_t ElementSizeInWords,  
    uint32_t ofst // Means local space offset on task domain. Units in word
)
{
	int idx_cycle = 0;
    // Write data per thread
    for(int idx_task = 0, cycle_counter = 0; idx_task < pp->ro_numtask; ++idx_task)
    {
        // @todo.
		
		++cycle_counter;
		if(cycle_counter == 24)
		{
			cycle_counter = 0;
			++idx_cycle;
		}
    }
}

void gppcu_read(
    swk_gppcu_stat* pp, 
    swk_gppcu_data_t * const dst, 
    uint32_t Capacity, 
    uint8_t ElementSizeInWords, 
    uint32_t ofst
);
