// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//      Copyright (c) 2022-2023 Arves100/Made In Server Developers.
//      Copyright (c) 1996-2009 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net

#ifdef EE_HAVE_INLINE_ASM
//--------------------------------------------------------------------------------------------------
// Use 128 bit aligned arrays with size multiples of 4 floats on SSE
// capable PCs. Use on __declspec(align(16)) float or DWORD arrays
inline void NiMemcpyFloatArraySSE(float* pDst, const float* pSrc, int SizeInFloats)
{
    int iCount = SizeInFloats >> 2;


#ifdef EE_COMPILER_MSVC
    __asm {
        mov     ecx,    iCount
        mov     edi,    pDst
        mov     esi,    pSrc

    loop1:
        movaps  xmm0,   [esi]
        movaps  [edi],  xmm0

        add     esi,    16
        add     edi,    16

        dec     ecx
        jnz     loop1
    }
#else
    asm(
        "movl %0, %ecx\n\t"
        "movl %1, %edi\n\t"
        "movl %2, %esi\n\t"

     "loop1:\n"
        "movaps (%esi), xmm0\n\t"
        "movaps xmm0, (%esi)\n\t"
        "addl $16, %esi\n\t"
        "addl $16, %edi\n\t"
        "dec %ecx\n\t"
        "jnz loop1"
        : "=r" (pDst)
        : "r" (iCount), "r"(pSrc)
    );
#endif
}

//--------------------------------------------------------------------------------------------------
// Use 32 bit aligned arrays with size multiples of 4 bytes.
// Use on __declspec(align(4)) float or DWORD arrays
inline void NiMemcpyFloatArray(float* pDst, const float* pSrc, int SizeInFloats)
{
    int iBytes = sizeof(float) * SizeInFloats;

#ifdef EE_COMPILER_MSVC
    __asm {
        mov     ecx,    iBytes
        shr     ecx,    2

        cld                         // clear direction flag
        mov     esi,    pSrc
        mov     edi,    pDst
        rep     movsd               // copy dword at a time
    }
#else
    asm(
        "movl %0, %ecx\n\t"
        "shrl $2, %ecx\n\t"
        "cld\n\t"
        "movl %1, %esi\n\t"
        "movl %2, %edi\n\t"
        "rep movsd"
        : "=r"(pDst)
        : "r"(iBytes), "r"(pSrc)
    );
#endif
}

//--------------------------------------------------------------------------------------------------
inline int NiMemcpyBytes(long* pDst,const long* pSrc, unsigned long SizeInBytes)
{
#ifdef EE_COMPILER_MSVC
    _asm
    {
        pusha;

        mov  ebx,SizeInBytes;
        mov  edi,pDst;
        mov  esi,pSrc;

looping:
        // read to register
        mov ecx, [esi];
        // write from register
        mov [edi], ecx;

        add esi, 1;
        add edi, 1;
        dec ebx;

        jnz  looping;

        popa;
    }
#else
    asm(
        "pusha\n\t"
        "movl %0, %ebx\n\t"
        "movl %1, %edi\n\t"
        "movl %2, %esi\n\t"

    "looping:\n\t"
        // read to register
        "movl (%esi), %ecx\n\t"
        // write from register
        "movl %ecx, (%edi)\n\t"
        
        "addl $1, %esi\n\t"
        "addl $1, %edi\n\t"
        "dec %ebx\n\t"

        "jnz looping\n\t"

        "popa"
        : "=r"(pDst)
        : "r"(SizeInBytes), "r"(pSrc)
    );
#endif

    return 0;
}
#endif

//--------------------------------------------------------------------------------------------------
