default rel
global _NiTransformVectorsPentium

section .text

_NiTransformVectorsPentium:
    push ebp
    mov ebp, esp
    push ebx

    mov edx, [ebp+8];
    cmp edx, 0;
    jnz NIASMVECLOOPSETUP;
    jmp NIASMVECEND;

NIASMVECLOOPSETUP:
    mov eax, [ebp+12];
    mov ebx, [ebp+16];
    mov ecx, [ebp+20];

NIASMVECLOOPTOP:
    fld dword [eax];          mX        1        1
    fmul dword [ecx];         mX*r00    2        2
    fld dword [eax+4];        mY        3        3  2
    fmul dword [ecx+4];       mY*r01    4        4  2
    fld dword [eax+8];        mZ        5        5  4  2
    fmul dword [ecx+8];       mZ*r02    6        6  4  2
    fxch;                                   7        4  6  2
    faddp st2,st0;            X0+Y0     8        6  8
    fld dword [eax];          mX        9        9  6  8
    fmul dword [ecx+12];      mX*r10    10      10  6  8
    fxch st2;                             11       8  6 10
    faddp st1,st0;            X0+Y0+Z0  12      12 10
    fxch;                                           10 12
    fld dword [eax+4];        mY        13      13 10 12
    fmul dword [ecx+16];      mY*r11    14      14 10 12
    fld dword [eax+8];        mZ        15      15 14 10 12
    fmul dword [ecx+20];      mZ*r12    16      16 14 10 12
    fxch st3;                             17      12 14 10 16
    fstp dword [ebx];         X0+Y0+Z0  18      14 10 16
    faddp st1,st0;            X1+Y1     19      19 16
    fld dword [eax];          mX        20      20 19 16
    fmul dword [ecx+24];      mX*r20    21      21 19 16
    fxch st2;                             22      16 19 21
    faddp st1,st0;            X1+Y1+Z1  23      23 21
    fld dword [eax+4];        mY        24      24 23 21
    fmul dword [ecx+28];      mY*r21    25      25 23 21
    fld dword [eax+8];        mZ        26      26 25 23 21
    fmul dword [ecx+32];      mZ*r22    27      27 25 23 21
    fxch;                                   28      25 27 23 21
    faddp st3,st0;            X2+Y2     29      27 23 29
    fxch;                                   30      23 27 29
    fstp dword [ebx+4];       X1+Y1+Z1  31      27 29
    fadd;                         X2+Y2+Y2  32      32
    add eax, 12;
    add ebx, 12;
    fstp dword [ebx-4];       X2+Y2+Y2  33      EMPTY
    dec edx;
    jnz NIASMVECLOOPTOP;
NIASMVECEND:
    pop ebx
    pop ebp
    ret

_NiTransformPointsPentiumRun:
    push ebp
    mov ebp, esp
    push ebx

    mov edx, [ebp+8];
    cmp edx, 0;
    jnz NIASMPTLOOPSETUP;
    jmp NIASMPTEND;

NIASMPTLOOPSETUP:

    mov eax, [ebp+12];
    mov ebx, [ebp+16];
    mov ecx, [ebp+20];

NIASMPTLOOPTOP:
    fld dword [eax];          mX        1        1
    fmul dword [ecx];         mX*r00    2        2
    fld dword [eax+4];        mY        3        3  2
    fmul dword [ecx+4];       mY*r01    4        4  2
    fld dword [eax+8];        mZ        5        5  4  2
    fmul dword [ecx+8];       mZ*r02    6        6  4  2
    fxch;                                            4  6  2
    faddp st2,st0;            X0+Y0     7        6  7
    fld dword [eax];          mX        8        8  6  7
    fmul dword [ecx+12];      mX*r10    9        9  6  7
    fxch st2;                                      7  6  9
    faddp st1,st0;            X0+Y0+Z0  10      10  9
    fxch;                                            9 10
    fld dword [eax+4];        mY        11      11  9 10
    fmul dword [ecx+16];      mY*r11    12      12  9 10
    fld dword [eax+8];        mZ        13      13 12  9 10
    fmul dword [ecx+20];      mZ*r12    14      14 12  9 10
    fxch;                                           12 14  9 10
    faddp st2,st0;            X1+Y1     15      14 15 10
    fxch st2;                                     10 15 14
    fadd dword [ebp+24];                     R0+TX     16      16 15 14
    fld dword [ecx+24];       r20       17      17 16 15 14
    fxch st2;                                     15 16 17 14
    faddp st3,st0;            X1+Y1+Z1  18      16 17 18
    fxch;                                           17 16 18
    fmul dword [eax];         mX*r20    19      19 16 18
    fld dword [ecx+28];       r21       20      20 19 16 18
    fld dword [ecx+32];       r22       21      21 20 19 16 18
    fxch;                                           20 21 19 16 18
    fmul dword [eax+4];       mY*r21    22      22 21 19 16 18
    fxch st4;                                     18 21 19 16 22
    fadd dword [ebp+28];                     R1+TY     23      23 21 19 16 22
    fxch;                                           21 23 19 16 22
    fmul dword [eax+8];       mZ*r22    24      24 23 19 16 22
    fxch st4;                                     22 23 19 16 24
    faddp st2,st0;            X2+Y2     25      23 25 16 24
    fxch st2;                                     16 25 23 24
    fstp dword [ebx];         WX        26-27   25 23 24
    faddp st2,st0;            X2+Y2+Z2  28      23 28
    fstp dword [ebx+4];       WY        29-30   28
    fadd dword [ebp+32];                     R2+TZ     31      31
    add eax, 12;                            32      31
    add ebx, 12;                            33      31
    fstp dword [ebx-4];       WZ        34-35   EMPTY

    dec edx;
    jnz NIASMPTLOOPTOP;
NIASMPTEND:
    pop ebx
    pop ebp
    ret
