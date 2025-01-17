/*
 * system.h - SOPC Builder system and BSP software package information
 *
 * Machine generated for CPU 'nios2_gen2_0' in SOPC Builder design 'SYSTEM'
 * SOPC Builder design path: ../../SYSTEM.sopcinfo
 *
 * Generated: Fri Jun 21 03:24:12 KST 2019
 */

/*
 * DO NOT MODIFY THIS FILE
 *
 * Changing this file will have subtle consequences
 * which will almost certainly lead to a nonfunctioning
 * system. If you do modify this file, be aware that your
 * changes will be overwritten and lost when this file
 * is generated again.
 *
 * DO NOT MODIFY THIS FILE
 */

/*
 * License Agreement
 *
 * Copyright (c) 2008
 * Altera Corporation, San Jose, California, USA.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * This agreement shall be governed in all respects by the laws of the State
 * of California and by the laws of the United States of America.
 */

#ifndef __SYSTEM_H_
#define __SYSTEM_H_

/* Include definitions from linker script generator */
#include "linker.h"


/*
 * CPU configuration
 *
 */

#define ALT_CPU_ARCHITECTURE "altera_nios2_gen2"
#define ALT_CPU_BIG_ENDIAN 0
#define ALT_CPU_BREAK_ADDR 0x10040820
#define ALT_CPU_CPU_ARCH_NIOS2_R1
#define ALT_CPU_CPU_FREQ 150000000u
#define ALT_CPU_CPU_ID_SIZE 1
#define ALT_CPU_CPU_ID_VALUE 0x00000000
#define ALT_CPU_CPU_IMPLEMENTATION "fast"
#define ALT_CPU_DATA_ADDR_WIDTH 0x1d
#define ALT_CPU_DCACHE_BYPASS_MASK 0x80000000
#define ALT_CPU_DCACHE_LINE_SIZE 32
#define ALT_CPU_DCACHE_LINE_SIZE_LOG2 5
#define ALT_CPU_DCACHE_SIZE 2048
#define ALT_CPU_EXCEPTION_ADDR 0x08000020
#define ALT_CPU_FLASH_ACCELERATOR_LINES 0
#define ALT_CPU_FLASH_ACCELERATOR_LINE_SIZE 0
#define ALT_CPU_FLUSHDA_SUPPORTED
#define ALT_CPU_FREQ 150000000
#define ALT_CPU_HARDWARE_DIVIDE_PRESENT 0
#define ALT_CPU_HARDWARE_MULTIPLY_PRESENT 1
#define ALT_CPU_HARDWARE_MULX_PRESENT 0
#define ALT_CPU_HAS_DEBUG_CORE 1
#define ALT_CPU_HAS_DEBUG_STUB
#define ALT_CPU_HAS_EXTRA_EXCEPTION_INFO
#define ALT_CPU_HAS_ILLEGAL_INSTRUCTION_EXCEPTION
#define ALT_CPU_HAS_JMPI_INSTRUCTION
#define ALT_CPU_ICACHE_LINE_SIZE 32
#define ALT_CPU_ICACHE_LINE_SIZE_LOG2 5
#define ALT_CPU_ICACHE_SIZE 4096
#define ALT_CPU_INITDA_SUPPORTED
#define ALT_CPU_INST_ADDR_WIDTH 0x1d
#define ALT_CPU_NAME "nios2_gen2_0"
#define ALT_CPU_NUM_OF_SHADOW_REG_SETS 0
#define ALT_CPU_OCI_VERSION 1
#define ALT_CPU_RESET_ADDR 0x08000000


/*
 * CPU configuration (with legacy prefix - don't use these anymore)
 *
 */

#define NIOS2_BIG_ENDIAN 0
#define NIOS2_BREAK_ADDR 0x10040820
#define NIOS2_CPU_ARCH_NIOS2_R1
#define NIOS2_CPU_FREQ 150000000u
#define NIOS2_CPU_ID_SIZE 1
#define NIOS2_CPU_ID_VALUE 0x00000000
#define NIOS2_CPU_IMPLEMENTATION "fast"
#define NIOS2_DATA_ADDR_WIDTH 0x1d
#define NIOS2_DCACHE_BYPASS_MASK 0x80000000
#define NIOS2_DCACHE_LINE_SIZE 32
#define NIOS2_DCACHE_LINE_SIZE_LOG2 5
#define NIOS2_DCACHE_SIZE 2048
#define NIOS2_EXCEPTION_ADDR 0x08000020
#define NIOS2_FLASH_ACCELERATOR_LINES 0
#define NIOS2_FLASH_ACCELERATOR_LINE_SIZE 0
#define NIOS2_FLUSHDA_SUPPORTED
#define NIOS2_HARDWARE_DIVIDE_PRESENT 0
#define NIOS2_HARDWARE_MULTIPLY_PRESENT 1
#define NIOS2_HARDWARE_MULX_PRESENT 0
#define NIOS2_HAS_DEBUG_CORE 1
#define NIOS2_HAS_DEBUG_STUB
#define NIOS2_HAS_EXTRA_EXCEPTION_INFO
#define NIOS2_HAS_ILLEGAL_INSTRUCTION_EXCEPTION
#define NIOS2_HAS_JMPI_INSTRUCTION
#define NIOS2_ICACHE_LINE_SIZE 32
#define NIOS2_ICACHE_LINE_SIZE_LOG2 5
#define NIOS2_ICACHE_SIZE 4096
#define NIOS2_INITDA_SUPPORTED
#define NIOS2_INST_ADDR_WIDTH 0x1d
#define NIOS2_NUM_OF_SHADOW_REG_SETS 0
#define NIOS2_OCI_VERSION 1
#define NIOS2_RESET_ADDR 0x08000000


/*
 * Custom instruction macros
 *
 */

#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0(n,A,B) __builtin_custom_inii(ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_N+(n&ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_N_MASK),(A),(B))
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1(n,A,B) __builtin_custom_inii(ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_N+(n&ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_N_MASK),(A),(B))
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_FADDS_N ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_N+5
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_FDIVS_N ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_N+7
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_FIXSI_N ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_N+1
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_FLOATIS_N ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_N+2
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_FMULS_N ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_N+4
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_FSQRTS_N ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_N+3
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_FSUBS_N ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_N+6
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_N 0xf8
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_N_MASK ((1<<3)-1)
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_ROUND_N ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_N+0
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_FABSS_N ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_N+0
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_FCMPEQS_N ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_N+3
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_FCMPGES_N ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_N+4
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_FCMPGTS_N ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_N+5
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_FCMPLES_N ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_N+6
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_FCMPLTS_N ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_N+7
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_FCMPNES_N ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_N+2
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_FMAXS_N ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_N+8
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_FMINS_N ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_N+9
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_FNEGS_N ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_N+1
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_N 0xe0
#define ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_N_MASK ((1<<4)-1)
#define fmaxf(A,B) __builtin_custom_fnff(ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_FMAXS_N,(A),(B))
#define fminf(A,B) __builtin_custom_fnff(ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_FMINS_N,(A),(B))
#define lroundf(A) __builtin_custom_inf(ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_ROUND_N,(A))
#define sqrtf(A) __builtin_custom_fnf(ALT_CI_NIOS_CUSTOM_INSTR_FLOATING_POINT_2_0_1_FSQRTS_N,(A))


/*
 * Define for each module class mastered by the CPU
 *
 */

#define __ALTERA_AVALON_JTAG_UART
#define __ALTERA_AVALON_NEW_SDRAM_CONTROLLER
#define __ALTERA_AVALON_ONCHIP_MEMORY2
#define __ALTERA_AVALON_PIO
#define __ALTERA_AVALON_SYSID_QSYS
#define __ALTERA_AVALON_TIMER
#define __ALTERA_NIOS2_GEN2
#define __ALTERA_NIOS_CUSTOM_INSTR_FLOATING_POINT_2


/*
 * System configuration
 *
 */

#define ALT_DEVICE_FAMILY "Cyclone IV E"
#define ALT_ENHANCED_INTERRUPT_API_PRESENT
#define ALT_IRQ_BASE NULL
#define ALT_LOG_PORT "/dev/null"
#define ALT_LOG_PORT_BASE 0x0
#define ALT_LOG_PORT_DEV null
#define ALT_LOG_PORT_TYPE ""
#define ALT_NUM_EXTERNAL_INTERRUPT_CONTROLLERS 0
#define ALT_NUM_INTERNAL_INTERRUPT_CONTROLLERS 1
#define ALT_NUM_INTERRUPT_CONTROLLERS 1
#define ALT_STDERR "/dev/jtag_uart_0"
#define ALT_STDERR_BASE 0x10041080
#define ALT_STDERR_DEV jtag_uart_0
#define ALT_STDERR_IS_JTAG_UART
#define ALT_STDERR_PRESENT
#define ALT_STDERR_TYPE "altera_avalon_jtag_uart"
#define ALT_STDIN "/dev/jtag_uart_0"
#define ALT_STDIN_BASE 0x10041080
#define ALT_STDIN_DEV jtag_uart_0
#define ALT_STDIN_IS_JTAG_UART
#define ALT_STDIN_PRESENT
#define ALT_STDIN_TYPE "altera_avalon_jtag_uart"
#define ALT_STDOUT "/dev/jtag_uart_0"
#define ALT_STDOUT_BASE 0x10041080
#define ALT_STDOUT_DEV jtag_uart_0
#define ALT_STDOUT_IS_JTAG_UART
#define ALT_STDOUT_PRESENT
#define ALT_STDOUT_TYPE "altera_avalon_jtag_uart"
#define ALT_SYSTEM_NAME "SYSTEM"


/*
 * bufferram_out configuration
 *
 */

#define ALT_MODULE_CLASS_bufferram_out altera_avalon_onchip_memory2
#define BUFFERRAM_OUT_ALLOW_IN_SYSTEM_MEMORY_CONTENT_EDITOR 0
#define BUFFERRAM_OUT_ALLOW_MRAM_SIM_CONTENTS_ONLY_FILE 0
#define BUFFERRAM_OUT_BASE 0x10000000
#define BUFFERRAM_OUT_CONTENTS_INFO ""
#define BUFFERRAM_OUT_DUAL_PORT 1
#define BUFFERRAM_OUT_GUI_RAM_BLOCK_TYPE "AUTO"
#define BUFFERRAM_OUT_INIT_CONTENTS_FILE "SYSTEM_bufferram_out"
#define BUFFERRAM_OUT_INIT_MEM_CONTENT 0
#define BUFFERRAM_OUT_INSTANCE_ID "NONE"
#define BUFFERRAM_OUT_IRQ -1
#define BUFFERRAM_OUT_IRQ_INTERRUPT_CONTROLLER_ID -1
#define BUFFERRAM_OUT_NAME "/dev/bufferram_out"
#define BUFFERRAM_OUT_NON_DEFAULT_INIT_FILE_ENABLED 0
#define BUFFERRAM_OUT_RAM_BLOCK_TYPE "AUTO"
#define BUFFERRAM_OUT_READ_DURING_WRITE_MODE "DONT_CARE"
#define BUFFERRAM_OUT_SINGLE_CLOCK_OP 0
#define BUFFERRAM_OUT_SIZE_MULTIPLE 1
#define BUFFERRAM_OUT_SIZE_VALUE 192000
#define BUFFERRAM_OUT_SPAN 192000
#define BUFFERRAM_OUT_TYPE "altera_avalon_onchip_memory2"
#define BUFFERRAM_OUT_WRITABLE 1


/*
 * hal configuration
 *
 */

#define ALT_INCLUDE_INSTRUCTION_RELATED_EXCEPTION_API
#define ALT_MAX_FD 32
#define ALT_SYS_CLK TIMER_0
#define ALT_TIMESTAMP_CLK none


/*
 * jtag_uart_0 configuration
 *
 */

#define ALT_MODULE_CLASS_jtag_uart_0 altera_avalon_jtag_uart
#define JTAG_UART_0_BASE 0x10041080
#define JTAG_UART_0_IRQ 0
#define JTAG_UART_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define JTAG_UART_0_NAME "/dev/jtag_uart_0"
#define JTAG_UART_0_READ_DEPTH 64
#define JTAG_UART_0_READ_THRESHOLD 8
#define JTAG_UART_0_SPAN 8
#define JTAG_UART_0_TYPE "altera_avalon_jtag_uart"
#define JTAG_UART_0_WRITE_DEPTH 64
#define JTAG_UART_0_WRITE_THRESHOLD 8


/*
 * pio_cmd configuration
 *
 */

#define ALT_MODULE_CLASS_pio_cmd altera_avalon_pio
#define PIO_CMD_BASE 0x10041050
#define PIO_CMD_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_CMD_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_CMD_CAPTURE 0
#define PIO_CMD_DATA_WIDTH 32
#define PIO_CMD_DO_TEST_BENCH_WIRING 0
#define PIO_CMD_DRIVEN_SIM_VALUE 0
#define PIO_CMD_EDGE_TYPE "NONE"
#define PIO_CMD_FREQ 150000000
#define PIO_CMD_HAS_IN 0
#define PIO_CMD_HAS_OUT 1
#define PIO_CMD_HAS_TRI 0
#define PIO_CMD_IRQ -1
#define PIO_CMD_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO_CMD_IRQ_TYPE "NONE"
#define PIO_CMD_NAME "/dev/pio_cmd"
#define PIO_CMD_RESET_VALUE 0
#define PIO_CMD_SPAN 16
#define PIO_CMD_TYPE "altera_avalon_pio"


/*
 * pio_datain configuration
 *
 */

#define ALT_MODULE_CLASS_pio_datain altera_avalon_pio
#define PIO_DATAIN_BASE 0x10041060
#define PIO_DATAIN_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_DATAIN_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_DATAIN_CAPTURE 0
#define PIO_DATAIN_DATA_WIDTH 32
#define PIO_DATAIN_DO_TEST_BENCH_WIRING 0
#define PIO_DATAIN_DRIVEN_SIM_VALUE 0
#define PIO_DATAIN_EDGE_TYPE "NONE"
#define PIO_DATAIN_FREQ 150000000
#define PIO_DATAIN_HAS_IN 1
#define PIO_DATAIN_HAS_OUT 0
#define PIO_DATAIN_HAS_TRI 0
#define PIO_DATAIN_IRQ -1
#define PIO_DATAIN_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO_DATAIN_IRQ_TYPE "NONE"
#define PIO_DATAIN_NAME "/dev/pio_datain"
#define PIO_DATAIN_RESET_VALUE 0
#define PIO_DATAIN_SPAN 16
#define PIO_DATAIN_TYPE "altera_avalon_pio"


/*
 * pio_dataout configuration
 *
 */

#define ALT_MODULE_CLASS_pio_dataout altera_avalon_pio
#define PIO_DATAOUT_BASE 0x10041040
#define PIO_DATAOUT_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_DATAOUT_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_DATAOUT_CAPTURE 0
#define PIO_DATAOUT_DATA_WIDTH 32
#define PIO_DATAOUT_DO_TEST_BENCH_WIRING 0
#define PIO_DATAOUT_DRIVEN_SIM_VALUE 0
#define PIO_DATAOUT_EDGE_TYPE "NONE"
#define PIO_DATAOUT_FREQ 150000000
#define PIO_DATAOUT_HAS_IN 0
#define PIO_DATAOUT_HAS_OUT 1
#define PIO_DATAOUT_HAS_TRI 0
#define PIO_DATAOUT_IRQ -1
#define PIO_DATAOUT_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO_DATAOUT_IRQ_TYPE "NONE"
#define PIO_DATAOUT_NAME "/dev/pio_dataout"
#define PIO_DATAOUT_RESET_VALUE 0
#define PIO_DATAOUT_SPAN 16
#define PIO_DATAOUT_TYPE "altera_avalon_pio"


/*
 * sdram configuration
 *
 */

#define ALT_MODULE_CLASS_sdram altera_avalon_new_sdram_controller
#define SDRAM_BASE 0x8000000
#define SDRAM_CAS_LATENCY 3
#define SDRAM_CONTENTS_INFO
#define SDRAM_INIT_NOP_DELAY 0.0
#define SDRAM_INIT_REFRESH_COMMANDS 2
#define SDRAM_IRQ -1
#define SDRAM_IRQ_INTERRUPT_CONTROLLER_ID -1
#define SDRAM_IS_INITIALIZED 1
#define SDRAM_NAME "/dev/sdram"
#define SDRAM_POWERUP_DELAY 200.0
#define SDRAM_REFRESH_PERIOD 7.8125
#define SDRAM_REGISTER_DATA_IN 1
#define SDRAM_SDRAM_ADDR_WIDTH 0x19
#define SDRAM_SDRAM_BANK_WIDTH 2
#define SDRAM_SDRAM_COL_WIDTH 10
#define SDRAM_SDRAM_DATA_WIDTH 32
#define SDRAM_SDRAM_NUM_BANKS 4
#define SDRAM_SDRAM_NUM_CHIPSELECTS 1
#define SDRAM_SDRAM_ROW_WIDTH 13
#define SDRAM_SHARED_DATA 0
#define SDRAM_SIM_MODEL_BASE 0
#define SDRAM_SPAN 134217728
#define SDRAM_STARVATION_INDICATOR 0
#define SDRAM_TRISTATE_BRIDGE_SLAVE ""
#define SDRAM_TYPE "altera_avalon_new_sdram_controller"
#define SDRAM_T_AC 5.5
#define SDRAM_T_MRD 3
#define SDRAM_T_RCD 20.0
#define SDRAM_T_RFC 70.0
#define SDRAM_T_RP 20.0
#define SDRAM_T_WR 14.0


/*
 * sysid_qsys_0 configuration
 *
 */

#define ALT_MODULE_CLASS_sysid_qsys_0 altera_avalon_sysid_qsys
#define SYSID_QSYS_0_BASE 0x10041078
#define SYSID_QSYS_0_ID 303182900
#define SYSID_QSYS_0_IRQ -1
#define SYSID_QSYS_0_IRQ_INTERRUPT_CONTROLLER_ID -1
#define SYSID_QSYS_0_NAME "/dev/sysid_qsys_0"
#define SYSID_QSYS_0_SPAN 8
#define SYSID_QSYS_0_TIMESTAMP 1561054015
#define SYSID_QSYS_0_TYPE "altera_avalon_sysid_qsys"


/*
 * timer_0 configuration
 *
 */

#define ALT_MODULE_CLASS_timer_0 altera_avalon_timer
#define TIMER_0_ALWAYS_RUN 0
#define TIMER_0_BASE 0x10041000
#define TIMER_0_COUNTER_SIZE 64
#define TIMER_0_FIXED_PERIOD 0
#define TIMER_0_FREQ 150000000
#define TIMER_0_IRQ 1
#define TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define TIMER_0_LOAD_VALUE 149999
#define TIMER_0_MULT 0.001
#define TIMER_0_NAME "/dev/timer_0"
#define TIMER_0_PERIOD 1
#define TIMER_0_PERIOD_UNITS "ms"
#define TIMER_0_RESET_OUTPUT 0
#define TIMER_0_SNAPSHOT 1
#define TIMER_0_SPAN 64
#define TIMER_0_TICKS_PER_SEC 1000
#define TIMER_0_TIMEOUT_PULSE_OUTPUT 0
#define TIMER_0_TYPE "altera_avalon_timer"

#endif /* __SYSTEM_H_ */
