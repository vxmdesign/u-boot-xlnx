/*
 * (C) Copyright 2012 Xilinx
 *
 * Configuration settings for the Xilinx Zynq AFX board.
 * See zynq_common.h for Zynq common configs
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_ZYNQ_AFX_H
#define __CONFIG_ZYNQ_AFX_H

#define PHYS_SDRAM_1_SIZE (128 * 1024 * 1024)

#define CONFIG_ZYNQ_SERIAL_UART1

#define CONFIG_SYS_NO_FLASH
#if defined(CONFIG_AFX_NOR)
# undef CONFIG_SYS_NO_FLASH
#endif


#include <configs/zynq_common.h>

#if defined(CONFIG_AFX_NOR) || defined(CONFIG_AFX_QSPI)
/* Place a Xilinx Boot ROM header in u-boot image? */
#define CONFIG_ZYNQ_XILINX_FLASH_HEADER
#endif



/*
 * SPI Settings
 */
#if defined(CONFIG_AFX_QSPI)
#define CONFIG_ZYNQ_SPI
#define CONFIG_CMD_SPI
#define CONFIG_SF_DEFAULT_SPEED 30000000
#define CONFIG_SPI_FLASH
#define CONFIG_CMD_SF
#define CONFIG_SPI_FLASH_STMICRO
#define CONFIG_SPI_FLASH_WINBOND
#define CONFIG_SPI_FLASH_SPANSION

#ifdef CONFIG_ZYNQ_XILINX_FLASH_HEADER
/* Address Xilinx boot rom should use to launch u-boot */
#define CONFIG_ZYNQ_XIP_START XPSS_QSPI_LIN_BASEADDR
#endif
#endif

/*
 * NAND Flash settings
 */
#if defined(CONFIG_AFX_NAND)
#define CONFIG_NAND_ZYNQ
#define CONFIG_CMD_NAND
#define CONFIG_CMD_NAND_LOCK_UNLOCK
#define CONFIG_SYS_MAX_NAND_DEVICE 1
#define CONFIG_SYS_NAND_BASE XPSS_NAND_BASEADDR
#define CONFIG_SYS_NAND_ONFI_DETECTION
#define CONFIG_MTD_DEVICE
#endif

#endif /* __CONFIG_ZYNQ_AFX_H */