#include "cachesim.h"

bool esPotenciaDeDos(int n){
    return pow(2, n);
}

int bitsEquivalent(int n){
    return ceil(log2(n));
}

int bitMask(int highbit, int lowbit) {
    int base = 1 << 31 >> 31;
    return (~(base << highbit << 1))&(base<< lowbit);
}
