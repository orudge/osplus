/*************************************************************/
/* OSPlus - Open Source version                              */
/* Copyright (c) Owen Rudge 2000-2005. All Rights Reserved.  */
/*************************************************************/
/* OSPlus Calculator Accessory                               */
/*************************************************************/

/* This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */

/* Revision History:
 *
 * 19/05/2001: Updated copyright information to reflect Open Source version (orudge)
 * 05/01/2002: Cleaned up indentation (orudge)
 * 19/03/2025: Replaced with version from SET's Turbo Vision (orudge)
*/

/*------------------------------------------------------------*/
/*                                                            */
/*   Turbo Vision 1.0                                         */
/*   Copyright (c) 1991 by Borland International              */
/*                                                            */
/*   Calc.cpp:  TCalcDisplay member functions                 */
/*                                                            */
/*------------------------------------------------------------*/

/*
  Modified by Salvador Eduardo Tropea <salvador@inti.gov.ar>
  <set@ieee.org> <set@computer.org>
  I ported it from TV 1.03.
  SET: Added locale support for the decimal point.
 */

#include "calc.h"

#define cpCalcPalette   "\x13"

TCalcDisplay::TCalcDisplay(TRect& r) : TView(r)
{
   options |= ofSelectable;
   eventMask = (evKeyboard | evBroadcast);
   number = new char[DISPLAYLEN];
   clear();
}

TCalcDisplay::~TCalcDisplay()
{
   delete[] number;
}

TPalette& TCalcDisplay::getPalette() const
{
   static TPalette palette(cpCalcPalette, sizeof(cpCalcPalette) - 1);
   return palette;
}

void TCalcDisplay::handleEvent(TEvent& event)
{
   static char keys[] = { 'C','\x8','%','_','7','8','9','/','4','5','6','*','1','2','3','-','0','.','=','+' };
   TView::handleEvent(event);

   switch (event.what)
   {
      case evKeyboard:
         calcKey(event.keyDown.charScan.charCode, event.keyDown.keyCode);
         clearEvent(event);
         break;
      case evBroadcast:
         if (event.message.command >= cmCalcButton &&
             event.message.command <= cmCalcButton + 19)
         {
            calcKey(keys[event.message.command - cmCalcButton], 0);
            clearEvent(event);
         }
         break;
   }
}

void TCalcDisplay::draw()
{
   TColorAttr color = getColor(1);
   int i;
   TDrawBuffer buf;

   i = size.x - strlen(number) - 2;
   buf.moveChar(0, ' ', color, size.x);
   buf.moveChar(i, sign, color, 1);
   buf.moveStr(i + 1, number, color);
   writeLine(0, 0, size.x, 1, buf);
}

void TCalcDisplay::error()
{
   status = csError;
   strcpy(number, "Error");
   sign = ' ';
}

void TCalcDisplay::clear()
{
   status = csFirst;
   strcpy(number, "0");
   sign = ' ';
   operate = '=';
}

void TCalcDisplay::setDisplay(double r)
{
   size_t len;
   char str[64];

   if (r < 0.0)
   {
      sign = '-';
      sprintf(str, "%f", -r);
   }
   else
   {
      sprintf(str, "%f", r);
      sign = ' ';
   }

   len = strlen(str) - 1; // Minus one so we can use as an index.

   if (len > DISPLAYLEN)
      error();
   else
      strcpy(number, str);
}

void TCalcDisplay::checkFirst()
{
   if (status == csFirst)
   {
      status = csValid;
      strcpy(number, "0");
      sign = ' ';
   }
}

void TCalcDisplay::calcKey(unsigned char key, unsigned code)
{
   char stub[2] = " ";
   double r;
   char* decPoint = "."; // TODO: localisation support

   switch (code)
   {
#ifdef _SET_TVISION
      case kbBackSpace:
#else
      case kbBack:
#endif
         key = 8;
         break;
      case kbEsc:
         key = 27;
         break;
      case kbEnter: // Added by Mike
         key = 13;
         break;
   }

   key = (unsigned char)toupper(key);

   if (status == csError && key != 'C')
      key = ' ';

   switch (key)
   {
      case '0':   case '1':   case '2':   case '3':   case '4':
      case '5':   case '6':   case '7':   case '8':   case '9':
         checkFirst();
         if (strlen(number) < 15)
         {                       // 15 is max visible display length
            if (!strcmp(number, "0"))
               number[0] = '\0';
            stub[0] = key;
            strcat(number, stub);
         }
         break;

      case '.':
         checkFirst();
         if (strstr(number, decPoint) == NULL)
             strcat(number, decPoint);
         break;

      case 8:
      case 27:
         int len;

         checkFirst();
         if ((len = strlen(number)) == 1)
            strcpy(number, "0");
         else
            number[len - 1] = '\0';
         break;

      case '_': // +-
         sign = (sign == ' ') ? '-' : ' ';
         break;

      case '+':   case '-':   case '*':   case '/':
      case '=':   case '%':   case 13:
         if (status == csValid)
         {
            status = csFirst;
            r = getDisplay() * ((sign == '-') ? -1.0 : 1.0);

            if (key == '%')
            {
               if (operate == '+' || operate == '-')
                  r = (operand * r) / 100;
               else
                  r /= 100;
            }

            switch (operate)
            {
               case '+':
                  setDisplay(operand + r);
                  break;

               case '-':
                  setDisplay(operand - r);
                  break;

               case '*':
                  setDisplay(operand * r);
                  break;

               case '/':
                  if (r == 0)
                     error();
                  else
                     setDisplay(operand / r);
                  break;
            }
         }

         operate = key;
         operand = getDisplay() * ((sign == '-') ? -1.0 : 1.0);
         break;

      case 'C':
         clear();
         break;
   }

   drawView();
}

const char* TCalculator::keyChar[20] =
{
   "C",   "<-",    "%",   "+-",
   "7",    "8",    "9",    "/",
   "4",    "5",    "6",    "*",
   "1",    "2",    "3",    "-",
   "0",    ".",    "=",    "+"
};

const char* const TCalculator::name = "TCalculator";

TCalculator::TCalculator() :
    TDialog(TRect(5, 3, 5 + 6 + DISPLAYLEN, 18), "Calculator"),
    TWindowInit(&TCalculator::initFrame)
{
    TView* tv;
    TRect r;

    options |= ofFirstClick;

    for (int i = 0; i <= 19; i++)
    {
        int x = (i % 4) * 6 + 3;
        int y = (i / 4) * 2 + 4;
        r = TRect(x, y, x + 6, y + 2);

        tv = new TButton(r, keyChar[i], cmCalcButton + i, bfNormal | bfBroadcast);
        tv->options &= ~ofSelectable;
        insert(tv);
    }

    r = TRect(3, 2, 3 + DISPLAYLEN, 3);
    insert(new TCalcDisplay(r));
}
