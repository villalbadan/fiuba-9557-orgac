/*
 * CS:APP Data Lab
 *
 * <Please put your name and userid here>
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:

  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code
  must conform to the following style:

  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>

  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.


  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 *
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce
 *      the correct answers.
 */


#endif
//1
/*
 * bitAnd - x&y using only ~ and |
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {
  return ~(~x | ~y);
}
/*
 * bitMatch - Create mask indicating which bits in x match those in y
 *            using only ~ and &
 *   Example: bitMatch(0x7, 0xE) = 0x6
 *   Legal ops: ~ & |
 *   Max ops: 14
 *   Rating: 1
 */
int bitMatch(int x, int y) {
  return (x&y)|~(x|y);
}
/*
 * bitNor - ~(x|y) using only ~ and &
 *   Example: bitNor(0x6, 0x5) = 0xFFFFFFF8
 *   Legal ops: ~ &
 *   Max ops: 8
 *   Rating: 1
 */
int bitNor(int x, int y) {
  return ~x & ~y;
}
/*
 * bitXor - x^y using only ~ and &
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  int complXandY = ~x&y; // 0 0 1 -> 1 1 0
  int complYandX = x&~y; // 0 0 0 -> 1 1 1

  return ~(~(complXandY)&~(complYandX));
}
//2
/*
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */

int allOddBits(int x) {
  int base=0xAA + (0xAA << 8) + (0xAA << 16) + (0xAA << 24);  // 10101010 10101010 10101010 10101010

  return !((x & base) ^ base);
}
/*
 * anyEvenBit - return 1 if any even-numbered bit in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples anyEvenBit(0xA) = 0, anyEvenBit(0xE) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int anyEvenBit(int x) {
  int base=0x55 + (0x55 << 8) + (0x55 << 16) + (0x55 << 24);
  // 1010101010101010101010101010101 but only allowed constants from 0x0 to 0xff)
  return !!(base & x);
}
/*
 * byteSwap - swaps the nth byte and the mth byte
 *  Examples: byteSwap(0x12345678, 1, 3) = 0x56341278
 *            byteSwap(0xDEADBEEF, 0, 2) = 0xDEEFBEAD
 *  You may assume that 0 <= n <= 3, 0 <= m <= 3
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 2
 */
int byteSwap(int x, int n, int m) {
  // 0 - 3
  //255=1byte
  int swap;
  int byteNaByteM;
  int byteMaByteN;
  //Create mask with 1 in bytes we want to change
  int baseM= 255 <<(m<<3); 
  int baseN= 255 <<(n<<3); 

  int cambioN= baseN&x;
  int cambioM= baseM&x;
  
  int baseBytesSinCambios= ~(baseM|baseN);

  //Keep the bytes without changes
  x=x&baseBytesSinCambios;

  byteNaByteM = ((cambioN>>(n<<3))&255)<<(m<<3);
  byteMaByteN = ((cambioM>>(m<<3))&255)<<(n<<3);

  //Mix the bytes already swap but with 0 in bytes without changes
  swap = byteNaByteM | byteMaByteN;

  return x|swap; 

}

/*
 * fitsBits - return 1 if x can be represented as an
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
  //x must be lower than 2^n-1. It must not have any 1 on the firts 32-n+1 bits

  //Bits before the n bit
  int shiftN= x>>~(~n+1);

  //Case: !(~shift_n)) -> n=32 bits
  return (!(~shiftN)) | !shiftN;
}

/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return (~x+1);
}
/*
 * sign - return 1 if positive, 0 if zero, and -1 if negative
 *  Examples: sign(130) = 1
 *            sign(-23) = -1
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 10
 *  Rating: 2
 */
int sign(int x) {
  //Keep de bit sign
  int sign = (x >> 31);

  return (sign | (!!x)); 
  // !! expression to bool, if the expression is !0 => returns 1, oppsite case returns 0
  
  // sign = 10    -> positive  => (0 | 1)   = 0
  // sign = 0     -> cero      => (0 | 0)   = 1
  // sign = -10   -> negative  => (-1 | 1) = -1

}
//3
/*
 * addOK - Determine if can compute x+y without overflow
 *   Example: addOK(0x80000000,0x80000000) = 0,
 *            addOK(0x80000000,0x70000000) = 1,
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int addOK(int x, int y) {
  // case 1: different sign cant overflow
  int signX = x >> 31;
  int signY = y >> 31;
  int cond1= (signX ^ signY); // 0000 dif sign, 1111 same sign
  // case 2: same sign cant result in x+y w different sign
  int sumSign = (x+y) >> 31;

  return !(~cond1 & (signY ^ sumSign));
}
/*
 * bitMask - Generate a mask consisting of all 1's
 *   lowbit and highbit
 *   Examples: bitMask(5,3) = 0x38
 *   Assume 0 <= lowbit <= 31, and 0 <= highbit <= 31
 *   If lowbit > highbit, then mask should be all 0's
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int bitMask(int highbit, int lowbit) {
    int base = 1 << 31 >> 31;
  return (~(base << highbit << 1))&(base<< lowbit);
}
/*
 * conditional - same as x ? y : z
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) { // x then y, not x then z
  int boolX= !!x;
  int condition = boolX << 31 >> 31;

  return ((~condition|y)&(condition|z));
}
/*
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  int limiteMenor = 0x30;
  int limiteMayor = 0x39;

  int dif1 = x +(~limiteMenor + 1);
  int dif2 = limiteMayor + (~x+1);

  return !(dif1>>31)&!(dif2>>31);
}
/*
 * isGreater - if x > y  then return 1, else return 0
 *   Example: isGreater(4,5) = 0, isGreater(5,4) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isGreater(int x, int y) {
  //Result of calculate "y-x"
  int result = y+(~x+1); 

  int caso1 = y&(~x);
  int caso2 = (~(x^y))&result; 

  return (((caso1 | caso2) >> 31) & 1);
}
/*
 * replaceByte(x,n,c) - Replace byte n in x with c
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: replaceByte(0x12345678,1,0xab) = 0x1234ab78
 *   You can assume 0 <= n <= 3 and 0 <= c <= 255
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 3
 */
int replaceByte(int x, int n, int c) {
  int mask=255;

  //Shift to the position we want to replace. All 1 in the position we don´t want to replace 
  int mask2= ~(mask <<(n<<3));
  
  int res= x&mask2;
  //Move c to the position it has to be. All else with 0
  int cShift=c <<(n<<3);

  return res|cShift;
}
//4
/*
 * absVal - absolute value of x
 *   Example: absVal(-1) = 1.
 *   You may assume -TMax <= x <= TMax
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 4
 */
int absVal(int x) {
  int sign= x >> 31; // MSB determines 0 if positive, else all 1 (-1) negative
  x = x + sign; 

  return x ^ sign; // xor:: if positive x stays the same; if negative, == complement
}
/*
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int bang(int x) {
  int negate_x = (~x)+1;
  int igual=(x|negate_x)>>31;

  return igual + 1;
}

/*
 * isNonZero - Check whether x is nonzero using
 *              the legal operators except !
 *   Examples: isNonZero(3) = 1, isNonZero(0) = 0
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 4
 */
int isNonZero(int x) {
  int base=0x00;
  return (((base|x)|(~x + 1))>>31)&1;

}
/*
 * logicalNeg - implement the ! operator, using all of
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int logicalNeg(int x) {
  int negateX = (~x)+1;
  int igual=(x|negateX)>>31;

  return igual + 1;
}
//float
/*
 * floatAbsVal - Return bit-level equivalent of absolute value of f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument..
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */

// FLOATING-POINT IEEE
// SINGLE PRECISION-> 32bits representation
// 1bit => sign bit + 8 bits => exponent + 23bits => fraction/mantissa
// Exponent range from -126 to 127, with actual numeric value of 255 (-127 as bias for signed representation)

unsigned floatAbsVal(unsigned uf) {
    // remove sign bit of floating point with mask 0x7fffffff
    // (every bit is 1 but the sign bit)
    unsigned absUf = uf & 0x7fffffff; // absolute value of uf

    // Check if NaN, as in if it's larger than 0x7f800000
    // 127[10] = 7f[16] = 1111111[2] max value for exponent
    // 128[10] = 80[16] == 10000000[2]
    // fraction (any value !0)

    if (absUf > 0x7f800000) {
        return uf;
    }
    return absUf;
}
/*
 * floatIsEqual - Compute f == g for floating point arguments f and g.
 *   Both the arguments are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   If either argument is NaN, return 0.
 *   +0 and -0 are considered equal.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 25
 *   Rating: 2
 */
int floatIsEqual(unsigned int x, unsigned int y) {
    // case NaN (**see floatAbsVal for doc**)
    if ((x & 0x7FFFFFFF) > 0x7F800000 || (y & 0x7FFFFFFF) > 0x7F800000) {
        return 0;
    }

    // case 0
    if ((x & 0x7FFFFFFF) == 0 && (y & 0x7FFFFFFF) == 0) {
        return 1;
    }

    return x == y;
}
/*
 * floatNegate - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned floatNegate(unsigned uf) {
    // case NaN (**see floatAbsVal for doc**)
    if ((uf & 0x7FFFFFFF) > 0x7F800000) {
        return uf;
    }
    return uf+0x80000000;
}
/*
 * floatIsLess - Compute f < g for floating point arguments f and g.
 *   Both the arguments are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   If either argument is NaN, return 0.
 *   +0 and -0 are considered equal.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 3
 */
int floatIsLess(unsigned uf, unsigned ug) {
    unsigned signF = uf >> 31;
    unsigned signG = ug >> 31;
    unsigned fracF = uf << 9;
    unsigned fracG = ug << 9;
    unsigned expF = (uf >> 23) & 0xFF;
    unsigned expG = (ug >> 23) & 0xFF;

    // case NaN
    if ((expF  == 0xff && fracF) || (expG == 0xff && fracG))
        return 0;

    // case +0/-0
    if ((expF == 0 && fracF == 0) && (expG == 0 && fracG == 0))
        return 0;

    // case != sign
    // return 1 if f is negative and g positive
    if (signF != signG)
        return signF && !signG;


    // case equal signs but different exponent
    // return depends on exponent
    if (expF != expG)
        return signF ? expF > expG : expF < expG;

    // case same exponent
    // check if fractions are equivalent
    return signF ? fracF > fracG : fracF < fracG;
}
/*
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
    unsigned sign = uf >> 31;
    unsigned exp = (uf >> 23) & 0xFF;
    unsigned frac = uf & 0x7FFFFF;
    int expExc = exp - 127;
    int rta;

    if (exp == 0xFF || expExc > 30) {
        return 0x80000000u;
    }
    if (expExc < 0) {
        rta = 0;
    } else {
        frac |= 0x800000;
        if (expExc > 23) {
            rta = frac << (expExc - 23);
        } else {
            rta = frac >> (23 - expExc);
        }
    }
    return sign ? -rta : rta;
}

/*
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 *
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatPower2(int x) {
    int expMask = (x + 127) << 23 ;
    int signMask = 0x80000000 & x;

    if (x < -126) return 0;
    if (x > 127) return 0x7f800000;
    return signMask | expMask;
}

