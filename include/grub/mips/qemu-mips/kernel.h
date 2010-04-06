/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2005,2006,2007,2008,2009  Free Software Foundation, Inc.
 *
 *  GRUB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  GRUB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GRUB.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GRUB_KERNEL_MACHINE_HEADER
#define GRUB_KERNEL_MACHINE_HEADER	1

#include <grub/symbol.h>
#include <grub/cpu/kernel.h>

#ifndef ASM_FILE

void EXPORT_FUNC (grub_reboot) (void);
void EXPORT_FUNC (grub_halt) (void);

/* The prefix which points to the directory where GRUB modules and its
   configuration file are located.  */
extern char grub_prefix[];

#define GRUB_PLATFORM_IMAGE_FORMATS     "raw, elf"
#define GRUB_PLATFORM_IMAGE_DEFAULT_FORMAT     "raw"

#define GRUB_PLATFORM_IMAGE_DEFAULT GRUB_PLATFORM_IMAGE_RAW

#ifndef ASM_FILE

typedef enum {
  GRUB_PLATFORM_IMAGE_RAW,
  GRUB_PLATFORM_IMAGE_ELF
}
  grub_platform_image_format_t;
#define GRUB_PLATFORM_IMAGE_RAW GRUB_PLATFORM_IMAGE_RAW
#define GRUB_PLATFORM_IMAGE_ELF GRUB_PLATFORM_IMAGE_ELF

#endif

#endif /* ! GRUB_KERNEL_MACHINE_HEADER */
