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

#include <grub/term.h>
#include <grub/err.h>
#include <grub/mm.h>
#include <grub/misc.h>

/* The list of terminals.  */
static grub_term_t grub_term_list;

/* The current terminal.  */
static grub_term_t grub_cur_term;

/* The amount of lines counted by the pager.  */
static int grub_more_lines;

/* If the more pager is active.  */
static int grub_more;

void
grub_term_register (grub_term_t term)
{
  term->next = grub_term_list;
  grub_term_list = term;
}

void
grub_term_unregister (grub_term_t term)
{
  grub_term_t *p, q;
  
  for (p = &grub_term_list, q = *p; q; p = &(q->next), q = q->next)
    if (q == term)
      {
        *p = q->next;
	break;
      }
}

void
grub_term_iterate (int (*hook) (grub_term_t term))
{
  grub_term_t p;
  
  for (p = grub_term_list; p; p = p->next)
    if (hook (p))
      break;
}

grub_err_t
grub_term_set_current (grub_term_t term)
{
  if (grub_cur_term && grub_cur_term->fini)
    if ((grub_cur_term->fini) () != GRUB_ERR_NONE)
      return grub_errno;

  if (term->init)
    if ((term->init) () != GRUB_ERR_NONE)
      return grub_errno;
  
  grub_cur_term = term;
  grub_cls ();
  return GRUB_ERR_NONE;
}

grub_term_t
grub_term_get_current (void)
{
  return grub_cur_term;
}

/* Put a Unicode character.  */
void
grub_putcode (grub_uint32_t code)
{
  if (code == '\t' && grub_cur_term->getxy)
    {
      int n;
      
      n = 8 - ((grub_getxy () >> 8) & 7);
      while (n--)
	grub_putcode (' ');

      return;
    }
  
  (grub_cur_term->putchar) (code);
  
  if (code == '\n')
    {
      grub_putcode ('\r');

      grub_more_lines++;
      /* XXX: Don't use a fixed height!  */
      if (grub_more && grub_more_lines == 24 - 1)
	{
	  char key;
	  int pos = grub_getxy ();

	  /* Show --MORE-- on the lower left side of the screen.  */
	  grub_gotoxy (1, 24 - 1);
	  grub_setcolorstate (GRUB_TERM_COLOR_HIGHLIGHT);
	  grub_printf ("--MORE--");
	  grub_setcolorstate (GRUB_TERM_COLOR_STANDARD);

	  key = grub_getkey ();
	  
	  /* Remove the message.  */
	  grub_gotoxy (1, 24 -1);
	  grub_printf ("        ");
	  grub_gotoxy (pos >> 8, pos & 0xFF);
	  
	  /* Scroll one lines or an entire page, depending on the key.  */
	  if (key == '\r' || key =='\n')
	    grub_more_lines--;
	  else
	    grub_more_lines = 0;
	}
    }
}

/* Put a character. C is one byte of a UTF-8 stream.
   This function gathers bytes until a valid Unicode character is found.  */
void
grub_putchar (int c)
{
  static grub_uint32_t code = 0;
  static int count = 0;

  if (count)
    {
      if ((c & 0xc0) != 0x80)
	{
	  /* invalid */
	  code = '@';
	  count = 0;
	}
      else
	{
	  code <<= 6;
	  code |= (c & 0x3f);
	  count--;
	}
    }
  else
    {
      if ((c & 0x80) == 0x00)
	code = c;
      else if ((c & 0xe0) == 0xc0)
	{
	  count = 1;
	  code = c & 0x1f;
	}
      else if ((c & 0xf0) == 0xe0)
	{
	  count = 2;
	  code = c & 0x0f;
	}
      else if ((c & 0xf8) == 0xf0)
	{
	  count = 3;
	  code = c & 0x07;
	}
      else if ((c & 0xfc) == 0xf8)
	{
	  count = 4;
	  code = c & 0x03;
	}
      else if ((c & 0xfe) == 0xfc)
	{
	  count = 5;
	  code = c & 0x01;
	}
      else
	/* invalid */
	code = '?';
    }	

  if (count)
    /* Not finished yet.  */
    return;

  grub_putcode (code);
}

int
grub_getkey (void)
{
  return (grub_cur_term->getkey) ();
}

int
grub_checkkey (void)
{
  return (grub_cur_term->checkkey) ();
}

grub_uint16_t
grub_getxy (void)
{
  return (grub_cur_term->getxy) ();
}

void
grub_gotoxy (grub_uint8_t x, grub_uint8_t y)
{
  (grub_cur_term->gotoxy) (x, y);
}

void
grub_cls (void)
{
  if (grub_cur_term->flags & GRUB_TERM_DUMB)
    {
      grub_putchar ('\n');
      grub_refresh ();
    }
  else
    (grub_cur_term->cls) ();
}

void
grub_setcolorstate (grub_term_color_state state)
{
  if (grub_cur_term->setcolorstate)
    (grub_cur_term->setcolorstate) (state);
}

void
grub_setcolor (grub_uint8_t normal_color, grub_uint8_t highlight_color)
{
  if (grub_cur_term->setcolor)
    (grub_cur_term->setcolor) (normal_color, highlight_color);
}

int
grub_setcursor (int on)
{
  static int prev = 1;
  int ret = prev;

  if (grub_cur_term->setcursor)
    {
      (grub_cur_term->setcursor) (on);
      prev = on;
    }
  
  return ret;
}

void
grub_refresh (void)
{
  if (grub_cur_term->refresh)
    (grub_cur_term->refresh) ();
}

void
grub_set_more (int onoff)
{
  if (onoff == 1)
    grub_more++;
  else
    grub_more--;

  grub_more_lines = 0;
}
