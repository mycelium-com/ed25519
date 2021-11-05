#include "ge.h"
#include "precomp_data.h"


/*
r = p + q
*/

void myc_ge_add(myc_ge_p1p1 *r, const myc_ge_p3 *p, const myc_ge_cached *q) {
    myc_fe t0;
    myc_fe_add(r->X, p->Y, p->X);
    myc_fe_sub(r->Y, p->Y, p->X);
    myc_fe_mul(r->Z, r->X, q->YplusX);
    myc_fe_mul(r->Y, r->Y, q->YminusX);
    myc_fe_mul(r->T, q->T2d, p->T);
    myc_fe_mul(r->X, p->Z, q->Z);
    myc_fe_add(t0, r->X, r->X);
    myc_fe_sub(r->X, r->Z, r->Y);
    myc_fe_add(r->Y, r->Z, r->Y);
    myc_fe_add(r->Z, t0, r->T);
    myc_fe_sub(r->T, t0, r->T);
}


static void slide(signed char *r, const unsigned char *a) {
    int i;
    int b;
    int k;

    for (i = 0; i < 256; ++i) {
        r[i] = 1 & (a[i >> 3] >> (i & 7));
    }

    for (i = 0; i < 256; ++i)
        if (r[i]) {
            for (b = 1; b <= 6 && i + b < 256; ++b) {
                if (r[i + b]) {
                    if (r[i] + (r[i + b] << b) <= 15) {
                        r[i] += r[i + b] << b;
                        r[i + b] = 0;
                    } else if (r[i] - (r[i + b] << b) >= -15) {
                        r[i] -= r[i + b] << b;

                        for (k = i + b; k < 256; ++k) {
                            if (!r[k]) {
                                r[k] = 1;
                                break;
                            }

                            r[k] = 0;
                        }
                    } else {
                        break;
                    }
                }
            }
        }
}

/*
r = a * A + b * B
where a = a[0]+256*a[1]+...+256^31 a[31].
and b = b[0]+256*b[1]+...+256^31 b[31].
B is the Ed25519 base point (x,4/5) with x positive.
*/

void myc_ge_double_scalarmult_vartime(myc_ge_p2 *r, const unsigned char *a, const myc_ge_p3 *A, const unsigned char *b) {
    signed char aslide[256];
    signed char bslide[256];
    myc_ge_cached Ai[8]; /* A,3A,5A,7A,9A,11A,13A,15A */
    myc_ge_p1p1 t;
    myc_ge_p3 u;
    myc_ge_p3 A2;
    int i;
    slide(aslide, a);
    slide(bslide, b);
    myc_ge_p3_to_cached(&Ai[0], A);
    myc_ge_p3_dbl(&t, A);
    myc_ge_p1p1_to_p3(&A2, &t);
    myc_ge_add(&t, &A2, &Ai[0]);
    myc_ge_p1p1_to_p3(&u, &t);
    myc_ge_p3_to_cached(&Ai[1], &u);
    myc_ge_add(&t, &A2, &Ai[1]);
    myc_ge_p1p1_to_p3(&u, &t);
    myc_ge_p3_to_cached(&Ai[2], &u);
    myc_ge_add(&t, &A2, &Ai[2]);
    myc_ge_p1p1_to_p3(&u, &t);
    myc_ge_p3_to_cached(&Ai[3], &u);
    myc_ge_add(&t, &A2, &Ai[3]);
    myc_ge_p1p1_to_p3(&u, &t);
    myc_ge_p3_to_cached(&Ai[4], &u);
    myc_ge_add(&t, &A2, &Ai[4]);
    myc_ge_p1p1_to_p3(&u, &t);
    myc_ge_p3_to_cached(&Ai[5], &u);
    myc_ge_add(&t, &A2, &Ai[5]);
    myc_ge_p1p1_to_p3(&u, &t);
    myc_ge_p3_to_cached(&Ai[6], &u);
    myc_ge_add(&t, &A2, &Ai[6]);
    myc_ge_p1p1_to_p3(&u, &t);
    myc_ge_p3_to_cached(&Ai[7], &u);
    myc_ge_p2_0(r);

    for (i = 255; i >= 0; --i) {
        if (aslide[i] || bslide[i]) {
            break;
        }
    }

    for (; i >= 0; --i) {
        myc_ge_p2_dbl(&t, r);

        if (aslide[i] > 0) {
            myc_ge_p1p1_to_p3(&u, &t);
            myc_ge_add(&t, &u, &Ai[aslide[i] / 2]);
        } else if (aslide[i] < 0) {
            myc_ge_p1p1_to_p3(&u, &t);
            myc_ge_sub(&t, &u, &Ai[(-aslide[i]) / 2]);
        }

        if (bslide[i] > 0) {
            myc_ge_p1p1_to_p3(&u, &t);
            myc_ge_madd(&t, &u, &Bi[bslide[i] / 2]);
        } else if (bslide[i] < 0) {
            myc_ge_p1p1_to_p3(&u, &t);
            myc_ge_msub(&t, &u, &Bi[(-bslide[i]) / 2]);
        }

        myc_ge_p1p1_to_p2(r, &t);
    }
}


static const myc_fe d = {
    -10913610, 13857413, -15372611, 6949391, 114729, -8787816, -6275908, -3247719, -18696448, -12055116
};

static const myc_fe sqrtm1 = {
    -32595792, -7943725, 9377950, 3500415, 12389472, -272473, -25146209, -2005654, 326686, 11406482
};

int myc_ge_frombytes_negate_vartime(myc_ge_p3 *h, const unsigned char *s) {
    myc_fe u;
    myc_fe v;
    myc_fe v3;
    myc_fe vxx;
    myc_fe check;
    myc_fe_frombytes(h->Y, s);
    myc_fe_1(h->Z);
    myc_fe_sq(u, h->Y);
    myc_fe_mul(v, u, d);
    myc_fe_sub(u, u, h->Z);     /* u = y^2-1 */
    myc_fe_add(v, v, h->Z);     /* v = dy^2+1 */
    myc_fe_sq(v3, v);
    myc_fe_mul(v3, v3, v);      /* v3 = v^3 */
    myc_fe_sq(h->X, v3);
    myc_fe_mul(h->X, h->X, v);
    myc_fe_mul(h->X, h->X, u);  /* x = uv^7 */
    myc_fe_pow22523(h->X, h->X); /* x = (uv^7)^((q-5)/8) */
    myc_fe_mul(h->X, h->X, v3);
    myc_fe_mul(h->X, h->X, u);  /* x = uv^3(uv^7)^((q-5)/8) */
    myc_fe_sq(vxx, h->X);
    myc_fe_mul(vxx, vxx, v);
    myc_fe_sub(check, vxx, u);  /* vx^2-u */

    if (myc_fe_isnonzero(check)) {
        myc_fe_add(check, vxx, u); /* vx^2+u */

        if (myc_fe_isnonzero(check)) {
            return -1;
        }

        myc_fe_mul(h->X, h->X, sqrtm1);
    }

    if (myc_fe_isnegative(h->X) == (s[31] >> 7)) {
        myc_fe_neg(h->X, h->X);
    }

    myc_fe_mul(h->T, h->X, h->Y);
    return 0;
}


/*
r = p + q
*/

void myc_ge_madd(myc_ge_p1p1 *r, const myc_ge_p3 *p, const myc_ge_precomp *q) {
    myc_fe t0;
    myc_fe_add(r->X, p->Y, p->X);
    myc_fe_sub(r->Y, p->Y, p->X);
    myc_fe_mul(r->Z, r->X, q->yplusx);
    myc_fe_mul(r->Y, r->Y, q->yminusx);
    myc_fe_mul(r->T, q->xy2d, p->T);
    myc_fe_add(t0, p->Z, p->Z);
    myc_fe_sub(r->X, r->Z, r->Y);
    myc_fe_add(r->Y, r->Z, r->Y);
    myc_fe_add(r->Z, t0, r->T);
    myc_fe_sub(r->T, t0, r->T);
}


/*
r = p - q
*/

void myc_ge_msub(myc_ge_p1p1 *r, const myc_ge_p3 *p, const myc_ge_precomp *q) {
    myc_fe t0;

    myc_fe_add(r->X, p->Y, p->X);
    myc_fe_sub(r->Y, p->Y, p->X);
    myc_fe_mul(r->Z, r->X, q->yminusx);
    myc_fe_mul(r->Y, r->Y, q->yplusx);
    myc_fe_mul(r->T, q->xy2d, p->T);
    myc_fe_add(t0, p->Z, p->Z);
    myc_fe_sub(r->X, r->Z, r->Y);
    myc_fe_add(r->Y, r->Z, r->Y);
    myc_fe_sub(r->Z, t0, r->T);
    myc_fe_add(r->T, t0, r->T);
}


/*
r = p
*/

void myc_ge_p1p1_to_p2(myc_ge_p2 *r, const myc_ge_p1p1 *p) {
    myc_fe_mul(r->X, p->X, p->T);
    myc_fe_mul(r->Y, p->Y, p->Z);
    myc_fe_mul(r->Z, p->Z, p->T);
}



/*
r = p
*/

void myc_ge_p1p1_to_p3(myc_ge_p3 *r, const myc_ge_p1p1 *p) {
    myc_fe_mul(r->X, p->X, p->T);
    myc_fe_mul(r->Y, p->Y, p->Z);
    myc_fe_mul(r->Z, p->Z, p->T);
    myc_fe_mul(r->T, p->X, p->Y);
}


void myc_ge_p2_0(myc_ge_p2 *h) {
    myc_fe_0(h->X);
    myc_fe_1(h->Y);
    myc_fe_1(h->Z);
}



/*
r = 2 * p
*/

void myc_ge_p2_dbl(myc_ge_p1p1 *r, const myc_ge_p2 *p) {
    myc_fe t0;

    myc_fe_sq(r->X, p->X);
    myc_fe_sq(r->Z, p->Y);
    myc_fe_sq2(r->T, p->Z);
    myc_fe_add(r->Y, p->X, p->Y);
    myc_fe_sq(t0, r->Y);
    myc_fe_add(r->Y, r->Z, r->X);
    myc_fe_sub(r->Z, r->Z, r->X);
    myc_fe_sub(r->X, t0, r->Y);
    myc_fe_sub(r->T, r->T, r->Z);
}


void myc_ge_p3_0(myc_ge_p3 *h) {
    myc_fe_0(h->X);
    myc_fe_1(h->Y);
    myc_fe_1(h->Z);
    myc_fe_0(h->T);
}


/*
r = 2 * p
*/

void myc_ge_p3_dbl(myc_ge_p1p1 *r, const myc_ge_p3 *p) {
    myc_ge_p2 q;
    myc_ge_p3_to_p2(&q, p);
    myc_ge_p2_dbl(r, &q);
}



/*
r = p
*/

static const myc_fe d2 = {
    -21827239, -5839606, -30745221, 13898782, 229458, 15978800, -12551817, -6495438, 29715968, 9444199
};

void myc_ge_p3_to_cached(myc_ge_cached *r, const myc_ge_p3 *p) {
    myc_fe_add(r->YplusX, p->Y, p->X);
    myc_fe_sub(r->YminusX, p->Y, p->X);
    myc_fe_copy(r->Z, p->Z);
    myc_fe_mul(r->T2d, p->T, d2);
}


/*
r = p
*/

void myc_ge_p3_to_p2(myc_ge_p2 *r, const myc_ge_p3 *p) {
    myc_fe_copy(r->X, p->X);
    myc_fe_copy(r->Y, p->Y);
    myc_fe_copy(r->Z, p->Z);
}


void myc_ge_p3_tobytes(unsigned char *s, const myc_ge_p3 *h) {
    myc_fe recip;
    myc_fe x;
    myc_fe y;
    myc_fe_invert(recip, h->Z);
    myc_fe_mul(x, h->X, recip);
    myc_fe_mul(y, h->Y, recip);
    myc_fe_tobytes(s, y);
    s[31] ^= myc_fe_isnegative(x) << 7;
}


static unsigned char equal(signed char b, signed char c) {
    unsigned char ub = b;
    unsigned char uc = c;
    unsigned char x = ub ^ uc; /* 0: yes; 1..255: no */
    uint64_t y = x; /* 0: yes; 1..255: no */
    y -= 1; /* large: yes; 0..254: no */
    y >>= 63; /* 1: yes; 0: no */
    return (unsigned char) y;
}

static unsigned char negative(signed char b) {
    uint64_t x = b; /* 18446744073709551361..18446744073709551615: yes; 0..255: no */
    x >>= 63; /* 1: yes; 0: no */
    return (unsigned char) x;
}

static void cmov(myc_ge_precomp *t, const myc_ge_precomp *u, unsigned char b) {
    myc_fe_cmov(t->yplusx, u->yplusx, b);
    myc_fe_cmov(t->yminusx, u->yminusx, b);
    myc_fe_cmov(t->xy2d, u->xy2d, b);
}


static void select(myc_ge_precomp *t, int pos, signed char b) {
    myc_ge_precomp minust;
    unsigned char bnegative = negative(b);
    unsigned char babs = b - (((-bnegative) & b) << 1);
    myc_fe_1(t->yplusx);
    myc_fe_1(t->yminusx);
    myc_fe_0(t->xy2d);
    cmov(t, &base[pos][0], equal(babs, 1));
    cmov(t, &base[pos][1], equal(babs, 2));
    cmov(t, &base[pos][2], equal(babs, 3));
    cmov(t, &base[pos][3], equal(babs, 4));
    cmov(t, &base[pos][4], equal(babs, 5));
    cmov(t, &base[pos][5], equal(babs, 6));
    cmov(t, &base[pos][6], equal(babs, 7));
    cmov(t, &base[pos][7], equal(babs, 8));
    myc_fe_copy(minust.yplusx, t->yminusx);
    myc_fe_copy(minust.yminusx, t->yplusx);
    myc_fe_neg(minust.xy2d, t->xy2d);
    cmov(t, &minust, bnegative);
}

/*
h = a * B
where a = a[0]+256*a[1]+...+256^31 a[31]
B is the Ed25519 base point (x,4/5) with x positive.

Preconditions:
  a[31] <= 127
*/

void myc_ge_scalarmult_base(myc_ge_p3 *h, const unsigned char *a) {
    signed char e[64];
    signed char carry;
    myc_ge_p1p1 r;
    myc_ge_p2 s;
    myc_ge_precomp t;
    int i;

    for (i = 0; i < 32; ++i) {
        e[2 * i + 0] = (a[i] >> 0) & 15;
        e[2 * i + 1] = (a[i] >> 4) & 15;
    }

    /* each e[i] is between 0 and 15 */
    /* e[63] is between 0 and 7 */
    carry = 0;

    for (i = 0; i < 63; ++i) {
        e[i] += carry;
        carry = e[i] + 8;
        carry >>= 4;
        e[i] -= carry << 4;
    }

    e[63] += carry;
    /* each e[i] is between -8 and 8 */
    myc_ge_p3_0(h);

    for (i = 1; i < 64; i += 2) {
        select(&t, i / 2, e[i]);
        myc_ge_madd(&r, h, &t);
        myc_ge_p1p1_to_p3(h, &r);
    }

    myc_ge_p3_dbl(&r, h);
    myc_ge_p1p1_to_p2(&s, &r);
    myc_ge_p2_dbl(&r, &s);
    myc_ge_p1p1_to_p2(&s, &r);
    myc_ge_p2_dbl(&r, &s);
    myc_ge_p1p1_to_p2(&s, &r);
    myc_ge_p2_dbl(&r, &s);
    myc_ge_p1p1_to_p3(h, &r);

    for (i = 0; i < 64; i += 2) {
        select(&t, i / 2, e[i]);
        myc_ge_madd(&r, h, &t);
        myc_ge_p1p1_to_p3(h, &r);
    }
}


/*
r = p - q
*/

void myc_ge_sub(myc_ge_p1p1 *r, const myc_ge_p3 *p, const myc_ge_cached *q) {
    myc_fe t0;
    
    myc_fe_add(r->X, p->Y, p->X);
    myc_fe_sub(r->Y, p->Y, p->X);
    myc_fe_mul(r->Z, r->X, q->YminusX);
    myc_fe_mul(r->Y, r->Y, q->YplusX);
    myc_fe_mul(r->T, q->T2d, p->T);
    myc_fe_mul(r->X, p->Z, q->Z);
    myc_fe_add(t0, r->X, r->X);
    myc_fe_sub(r->X, r->Z, r->Y);
    myc_fe_add(r->Y, r->Z, r->Y);
    myc_fe_sub(r->Z, t0, r->T);
    myc_fe_add(r->T, t0, r->T);
}


void myc_ge_tobytes(unsigned char *s, const myc_ge_p2 *h) {
    myc_fe recip;
    myc_fe x;
    myc_fe y;
    myc_fe_invert(recip, h->Z);
    myc_fe_mul(x, h->X, recip);
    myc_fe_mul(y, h->Y, recip);
    myc_fe_tobytes(s, y);
    s[31] ^= myc_fe_isnegative(x) << 7;
}

void myc_ge_point_add(uint8_t *public_key, uint8_t *ZL8, uint8_t *child_public_key) {
    myc_ge_p3 nB;
    myc_ge_p1p1 A_p1p1;
    myc_ge_p3 A;
    myc_ge_p3 public_key_unpacked;
    myc_ge_cached T;
    myc_ge_scalarmult_base(&nB, ZL8);
    myc_ge_frombytes_negate_vartime(&public_key_unpacked, public_key);
    myc_fe_neg(public_key_unpacked.X, public_key_unpacked.X); /* undo negate */
    myc_fe_neg(public_key_unpacked.T, public_key_unpacked.T); /* undo negate */
    myc_ge_p3_to_cached(&T, &public_key_unpacked);
    myc_ge_add(&A_p1p1, &nB, &T);
    myc_ge_p1p1_to_p3(&A, &A_p1p1);
    myc_ge_p3_tobytes(child_public_key, &A);
}

