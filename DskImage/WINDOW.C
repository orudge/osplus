/*************************************************/
/* OSPlus Disk Imager                            */
/* Version 1.2                                   */
/*                                               */
/* Copyright (c) Owen Rudge 2000-2001            */
/* Disk Writing Routines: Written by Mark Becker */
/* Disk Reading Routines: (c) Hans Lermen 1995   */
/*************************************************/

#include <string.h>
#include <dos.h>

#ifdef __MSDOS__
	#define CHANGE_WINDOW_TITLE_DOS   1 // change the Windows VM title?
	#define APPLICATION_TITLE         "OSPlus Disk Imager\0"
#endif

#ifdef CHANGE_WINDOW_TITLE_DOS
void OS_ChangeWindowTitle()
{
  char AppTitle[20];
  struct SREGS sregs;
  union REGS regs;

  strcpy(AppTitle, APPLICATION_TITLE);

  regs.h.ah = 0x16;
  regs.h.al = 0x8E;
  sregs.es = FP_SEG(AppTitle);
  regs.w.di = FP_OFF(AppTitle);
  regs.w.dx = 0; // set app title

  int86x(0x2F, &regs, &regs, &sregs);
}

void OS_RemoveWindowTitle()
{
  char BlankTitle = '\0';
  struct SREGS sregs;
  union REGS regs;

  regs.h.ah = 0x16;
  regs.h.al = 0x8E;
  sregs.es = FP_SEG(BlankTitle);
  regs.w.di = FP_OFF(BlankTitle);
  regs.w.dx = 0; // set app title

  int86x(0x2F, &regs, &regs, &sregs);
}
#endif
