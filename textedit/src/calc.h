/************************************************************/
/* OSPlus Text Editor - Open Source                         */
/* Copyright (c) Owen Rudge 2000-2005. All Rights Reserved. */
/************************************************************/
/* OSPlus Calculator Accessory                              */
/* Header File                                              */
/************************************************************/

#if !defined( __CALC_H )
#define __CALC_H

#define Uses_string
#define Uses_stdlib
#define Uses_ctype
#define Uses_stdio // sprintf

#define Uses_TKeys
#define Uses_TRect
#define Uses_TEvent
#define Uses_TButton
#define Uses_TKeys
#define Uses_TView
#define Uses_TPalette
#define Uses_TDialog

#include "inc_tv.h"

// SET: The following should be enough to hold the output of the %f modifier
#define DISPLAYLEN  25      // Length (width) of calculator display

enum TCalcState { csFirst = 1, csValid, csError };

const int cmCalcButton = 200,
cmCalcClear = cmCalcButton,
cmCalcDelete = cmCalcButton + 1,
cmCalcPercent = cmCalcButton + 2,
cmCalcPlusMin = cmCalcButton + 3,
cmCalc7 = cmCalcButton + 4,
cmCalc8 = cmCalcButton + 5,
cmCalc9 = cmCalcButton + 6,
cmCalcDiv = cmCalcButton + 7,
cmCalc4 = cmCalcButton + 8,
cmCalc5 = cmCalcButton + 9,
cmCalc6 = cmCalcButton + 10,
cmCalcMul = cmCalcButton + 11,
cmCalc1 = cmCalcButton + 12,
cmCalc2 = cmCalcButton + 13,
cmCalc3 = cmCalcButton + 14,
cmCalcMinus = cmCalcButton + 15,
cmCalc0 = cmCalcButton + 16,
cmCalcDecimal = cmCalcButton + 17,
cmCalcEqual = cmCalcButton + 18,
cmCalcPlus = cmCalcButton + 19;

class TRect;
class TPalette;
struct TEvent;

class TCalcDisplay : public TView
{
public:
    TCalcDisplay(TRect& r);
    ~TCalcDisplay();
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    virtual void draw();

private:
    TCalcState status;
    char* number;
    char sign;
    char operate;           // since 'operator' is a reserved word.
    double operand;

    void calcKey(unsigned char key, unsigned code);
    void checkFirst();
    void setDisplay(double r);
    void clear();
    void error();
    inline double getDisplay() { return(atof(number)); };
};

class TCalculator : public TDialog
{
public:
    TCalculator();
    static const char* keyChar[20];

private:
    virtual const char* streamableName() const
    {
        return name;
    }

public:
    static const char* const name;
};

#endif      // __CALC_H