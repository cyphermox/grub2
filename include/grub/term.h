/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2002,2003  Free Software Foundation, Inc.
 *
 *  GRUB is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GRUB; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef GRUB_TERM_HEADER
#define GRUB_TERM_HEADER	1

#include <grub/err.h>
#include <grub/symbol.h>
#include <grub/types.h>

/* These are used to represent the various color states we use.  */
typedef enum
  {
    /* The color used to display all text that does not use the
       user defined colors below.  */
    GRUB_TERM_COLOR_STANDARD,
    /* The user defined colors for normal text.  */
    GRUB_TERM_COLOR_NORMAL,
    /* The user defined colors for highlighted text.  */
    GRUB_TERM_COLOR_HIGHLIGHT
  }
grub_term_color_state;

/* Flags for representing the capabilities of a terminal.  */
/* Some notes about the flags:
   - These flags are used by higher-level functions but not terminals
   themselves.
   - If a terminal is dumb, you may assume that only putchar, getkey and
   checkkey are called.
   - Some fancy features (setcolorstate, setcolor and setcursor) can be set
   to NULL.  */

/* Set when input characters shouldn't be echoed back.  */
#define GRUB_TERM_NO_ECHO	(1 << 0)
/* Set when the editing feature should be disabled.  */
#define GRUB_TERM_NO_EDIT	(1 << 1)
/* Set when the terminal cannot do fancy things.  */
#define GRUB_TERM_DUMB		(1 << 2)
/* Set when the terminal needs to be initialized.  */
#define GRUB_TERM_NEED_INIT	(1 << 16)

struct grub_term
{
  /* The terminal name.  */
  const char *name;

  /* Initialize the terminal.  */
  grub_err_t (*init) (void);

  /* Clean up the terminal.  */
  grub_err_t (*fini) (void);
  
  /* Put a character. C is encoded in Unicode.  */
  void (*putchar) (grub_uint32_t c);
  
  /* Check if any input character is available.  */
  int (*checkkey) (void);
  
  /* Get a character.  */
  int (*getkey) (void);
  
  /* Get the cursor position. The return value is ((X << 8) | Y).  */
  grub_uint16_t (*getxy) (void);
  
  /* Go to the position (X, Y).  */
  void (*gotoxy) (grub_uint8_t x, grub_uint8_t y);
  
  /* Clear the screen.  */
  void (*cls) (void);
  
  /* Set the current color to be used */
  void (*setcolorstate) (grub_term_color_state state);
  
  /* Set the normal color and the highlight color. The format of each
     color is VGA's.  */
  void (*setcolor) (grub_uint8_t normal_color, grub_uint8_t highlight_color);
  
  /* Turn on/off the cursor.  */
  void (*setcursor) (int on);

  /* Update the screen.  */
  void (*refresh) (void);

  /* The feature flags defined above.  */
  grub_uint32_t flags;
  
  /* The next terminal.  */
  struct grub_term *next;
};
typedef struct grub_term *grub_term_t;

void EXPORT_FUNC(grub_term_register) (grub_term_t term);
void EXPORT_FUNC(grub_term_unregister) (grub_term_t term);
void EXPORT_FUNC(grub_term_iterate) (int (*hook) (grub_term_t term));

grub_err_t EXPORT_FUNC(grub_term_set_current) (grub_term_t term);
grub_term_t EXPORT_FUNC(grub_term_get_current) (void);

void EXPORT_FUNC(grub_putchar) (int c);
void EXPORT_FUNC(grub_putcode) (grub_uint32_t code);
int EXPORT_FUNC(grub_getkey) (void);
int EXPORT_FUNC(grub_checkkey) (void);
grub_uint16_t EXPORT_FUNC(grub_getxy) (void);
void EXPORT_FUNC(grub_gotoxy) (grub_uint8_t x, grub_uint8_t y);
void EXPORT_FUNC(grub_cls) (void);
void EXPORT_FUNC(grub_setcolorstate) (grub_term_color_state state);
void EXPORT_FUNC(grub_setcolor) (grub_uint8_t normal_color,
				 grub_uint8_t highlight_color);
int EXPORT_FUNC(grub_setcursor) (int on);
void EXPORT_FUNC(grub_refresh) (void);
void EXPORT_FUNC(grub_set_more) (int onoff);

/* For convenience.  */
#define GRUB_TERM_ASCII_CHAR(c)	((c) & 0xff)

#endif /* ! GRUB_TERM_HEADER */
