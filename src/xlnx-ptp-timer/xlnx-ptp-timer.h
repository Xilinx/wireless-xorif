/*
 * Xilinx PTP : Linux driver for 1588 timer
 *
 * Author: Xilinx, Inc.
 *
 * 2014 (c) Xilinx, Inc. This file is licensed uner the terms of the GNU
 * General Public License version 2. This program is licensed "as is"
 * without any warranty of any kind, whether express or implied.
 *
 */

#ifndef PTP_XILINX_1588_H
#define PTP_XILINX_1588_H

/* Read/Write access to the registers */
#ifndef out_be32
#if defined(CONFIG_ARCH_ZYNQ) || defined(CONFIG_ARCH_ZYNQMP)
#define in_be32(offset)		__raw_readl(offset)
#define out_be32(offset, val)	__raw_writel(val, offset)
#endif
#endif

#endif /* PTP_XILINX_1588_H */