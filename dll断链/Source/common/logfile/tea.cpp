//#include "precompile.h"
#include "tea.h"

void tea_decrypt(unsigned long *v, unsigned long *k) {
    unsigned long y=v[0], z=v[1], sum=0xC6EF3720, i; /* set up */
    unsigned long delta=0x9e3779b9;                  /* a key schedule constant */
    unsigned long a=k[0], b=k[1], c=k[2], d=k[3];    /* cache key */
    for(i=0; i<32; i++) {                            /* basic cycle start */
        z -= ((y<<4) + c) ^ (y + sum) ^ ((y>>5) + d);
        y -= ((z<<4) + a) ^ (z + sum) ^ ((z>>5) + b);
        sum -= delta;                                /* end cycle */
    }
    v[0]=y;
    v[1]=z;
}

void tea_encrypt(unsigned long *v, unsigned long *k) {
    unsigned long y=v[0], z=v[1], sum=0, i;         /* set up */
    unsigned long delta=0x9e3779b9;                 /* a key schedule constant */
    unsigned long a=k[0], b=k[1], c=k[2], d=k[3];   /* cache key */
    for (i=0; i < 32; i++) {                        /* basic cycle start */
        sum += delta;
        y += ((z<<4) + a) ^ (z + sum) ^ ((z>>5) + b);
        z += ((y<<4) + c) ^ (y + sum) ^ ((y>>5) + d);/* end cycle */
    }
    v[0]=y;
    v[1]=z;
}