/* Copyright (c) 2004 Robert Nordier.  All rights reserved. */

/* $Id: blib.c,v 1.5 2004/12/11 11:55:14 rn Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include "blib.h"

#define FTSZ 20

extern uint16_t *M;

static int ft[FTSZ];
static int fi, fo;

uint16_t getbyte(uint16_t s, uint16_t i)
{
    uint16_t n = M[s + i / 2];
    if (!(i & 1))
        n >>= 8;
    return n & 0xFF;
}

void putbyte(uint16_t s, uint16_t i, uint16_t ch)
{
    uint16_t *p = M + s + i/2;
    if (i & 1) {
        *p &= 0xFF00;
        *p |= (uint8_t)ch;
    } else {
        *p &= 0x00FF;
        *p |= ch << 8;
    }
}

static char *cstr(uint16_t s)
{
    char *st;
    int n, i;

    n = getbyte(s, 0);
    st = sbrk(n + 1);
    if (st == (char *)-1) {
        write(2, "OOM\n", 4);
        exit(1);
    }
    for (i = 1; i <= n; i++)
        st[i - 1] = getbyte(s, i);
    st[n] = 0;
    return st;
}

static int ftslot(void)
{
    int i;

    for (i = 3; i < FTSZ; i++)
        if (ft[i] == -1)
            return i;
    return -1;
}

void initio(void)
{
    int i;
    ft[0] = 0;
    ft[1] = 1;
    ft[2] = 2;
    for (i = 3; i < FTSZ; i++)
        ft[i] = -1;
    fi = 0;
    fo = 1;
}

int16_t findinput(uint16_t s)
{
    char *st = cstr(s);
    int x;

    x = ftslot();
    if (x != -1) {
        ft[x] = open(st, O_RDONLY);
        if (ft[x] == -1)
            x = -1;
    }
    brk(st);
    return x + 1;
}

int16_t findoutput(uint16_t s)
{
    char *st = cstr(s);
    int x;

    x = ftslot();
    if (x != -1) {
        ft[x] = open(st, O_WRONLY|O_CREAT|O_TRUNC, 0666);
        if (ft[x] == -1)
            x = -1;
    }
    brk(st);
    return x + 1;
}

void selectinput(int16_t x)
{
    fi = x - 1;
}

void selectoutput(int16_t x)
{
    fo = x - 1;
}

int16_t input(void)
{
    return fi + 1;
}

int16_t output(void)
{
    return fo + 1;
}

int16_t rdch(void)
{
    char c;
    if (read(ft[fi], &c, 1))
        return c;
    return -1;
}

void wrch(int16_t c)
{
    char cv = c;
    write(ft[fo], &cv, 1);
}

void endread(void)
{
    if (fi > 2) {
        close(ft[fi]);
        ft[fi] = -1;
    }
    fi = 0;
}

void endwrite(void)
{
    if (fo > 2) {
        close(ft[fo]);
        ft[fo] = -1;
    }
    fo = 1;
}

void mapstore(void)
{
    fprintf(stderr, "\nMAPSTORE NOT IMPLEMENTED\n");
}
