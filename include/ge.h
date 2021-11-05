#ifndef MYC_GE_H
#define MYC_GE_H

#include "fe.h"


/*
ge means group element.

Here the group is the set of pairs (x,y) of field elements (see fe.h)
satisfying -x^2 + y^2 = 1 + d x^2y^2
where d = -121665/121666.

Representations:
  ge_p2 (projective): (X:Y:Z) satisfying x=X/Z, y=Y/Z
  ge_p3 (extended): (X:Y:Z:T) satisfying x=X/Z, y=Y/Z, XY=ZT
  ge_p1p1 (completed): ((X:Z),(Y:T)) satisfying x=X/Z, y=Y/T
  ge_precomp (Duif): (y+x,y-x,2dxy)
*/

typedef struct {
  myc_fe X;
  myc_fe Y;
  myc_fe Z;
} myc_ge_p2;

typedef struct {
  myc_fe X;
  myc_fe Y;
  myc_fe Z;
  myc_fe T;
} myc_ge_p3;

typedef struct {
  myc_fe X;
  myc_fe Y;
  myc_fe Z;
  myc_fe T;
} myc_ge_p1p1;

typedef struct {
  myc_fe yplusx;
  myc_fe yminusx;
  myc_fe xy2d;
} myc_ge_precomp;

typedef struct {
  myc_fe YplusX;
  myc_fe YminusX;
  myc_fe Z;
  myc_fe T2d;
} myc_ge_cached;

void myc_ge_p3_tobytes(unsigned char *s, const myc_ge_p3 *h);
void myc_ge_tobytes(unsigned char *s, const myc_ge_p2 *h);
int myc_ge_frombytes_negate_vartime(myc_ge_p3 *h, const unsigned char *s);

void myc_ge_add(myc_ge_p1p1 *r, const myc_ge_p3 *p, const myc_ge_cached *q);
void myc_ge_sub(myc_ge_p1p1 *r, const myc_ge_p3 *p, const myc_ge_cached *q);
void myc_ge_double_scalarmult_vartime(myc_ge_p2 *r, const unsigned char *a, const myc_ge_p3 *A, const unsigned char *b);
void myc_ge_madd(myc_ge_p1p1 *r, const myc_ge_p3 *p, const myc_ge_precomp *q);
void myc_ge_msub(myc_ge_p1p1 *r, const myc_ge_p3 *p, const myc_ge_precomp *q);
void myc_ge_scalarmult_base(myc_ge_p3 *h, const unsigned char *a);

void myc_ge_p1p1_to_p2(myc_ge_p2 *r, const myc_ge_p1p1 *p);
void myc_ge_p1p1_to_p3(myc_ge_p3 *r, const myc_ge_p1p1 *p);
void myc_ge_p2_0(myc_ge_p2 *h);
void myc_ge_p2_dbl(myc_ge_p1p1 *r, const myc_ge_p2 *p);
void myc_ge_p3_0(myc_ge_p3 *h);
void myc_ge_p3_dbl(myc_ge_p1p1 *r, const myc_ge_p3 *p);
void myc_ge_p3_to_cached(myc_ge_cached *r, const myc_ge_p3 *p);
void myc_ge_p3_to_p2(myc_ge_p2 *r, const myc_ge_p3 *p);

void myc_ge_point_add(uint8_t *public_key, uint8_t *ZL8, uint8_t *child_public_key);

#endif
