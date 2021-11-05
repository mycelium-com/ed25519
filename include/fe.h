#ifndef MYC_FE_H
#define MYC_FE_H

#include "fixedint.h"


/*
    fe means field element.
    Here the field is \Z/(2^255-19).
    An element t, entries t[0]...t[9], represents the integer
    t[0]+2^26 t[1]+2^51 t[2]+2^77 t[3]+2^102 t[4]+...+2^230 t[9].
    Bounds on each t[i] vary depending on context.
*/


typedef int32_t myc_fe[10];


void myc_fe_0(myc_fe h);
void myc_fe_1(myc_fe h);

void myc_fe_frombytes(myc_fe h, const unsigned char *s);
void myc_fe_tobytes(unsigned char *s, const myc_fe h);

void myc_fe_copy(myc_fe h, const myc_fe f);
int myc_fe_isnegative(const myc_fe f);
int myc_fe_isnonzero(const myc_fe f);
void myc_fe_cmov(myc_fe f, const myc_fe g, unsigned int b);
void myc_fe_cswap(myc_fe f, myc_fe g, unsigned int b);

void myc_fe_neg(myc_fe h, const myc_fe f);
void myc_fe_add(myc_fe h, const myc_fe f, const myc_fe g);
void myc_fe_invert(myc_fe out, const myc_fe z);
void myc_fe_sq(myc_fe h, const myc_fe f);
void myc_fe_sq2(myc_fe h, const myc_fe f);
void myc_fe_mul(myc_fe h, const myc_fe f, const myc_fe g);
void myc_fe_mul121666(myc_fe h, myc_fe f);
void myc_fe_pow22523(myc_fe out, const myc_fe z);
void myc_fe_sub(myc_fe h, const myc_fe f, const myc_fe g);

#endif
