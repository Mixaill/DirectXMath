//-------------------------------------------------------------------------------------
// DirectXMathBE.h -- Big-endian swap extensions for SIMD C++ Math library
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615560
//-------------------------------------------------------------------------------------

#ifdef _MSC_VER
#pragma once
#endif

#pragma warning(push)
#pragma warning(disable : 4987)
#include <intrin.h>
#pragma warning(pop)

#if !defined(_M_ARM) && !defined(__arm__) && !defined(__aarch64__)
#include <tmmintrin.h>
#endif

#include <DirectXMath.h>

namespace DirectX
{

inline XMVECTOR XM_CALLCONV XMVectorEndian
(
    FXMVECTOR V 
)
{
#if defined(_XM_ARM_NEON_INTRINSICS_) && !defined(_XM_NO_INTRINSICS_)
    static const XMVECTORU32 idx = { 0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F };

    int8x8x2_t tbl;
    tbl.val[0] = vget_low_f32(V);
    tbl.val[1] = vget_high_f32(V);

    const __n64 rL = vtbl2_u8( tbl, vget_low_f32(idx) );
    const __n64 rH = vtbl2_u8( tbl, vget_high_f32(idx) );
    return vcombine_f32( rL, rH );
#else
    XMVECTORU32 E;
    E.v = V;
    uint32_t value = E.u[0];
    E.u[0] = ( (value << 24) | ((value & 0xFF00) << 8) | ((value & 0xFF0000) >> 8) | (value >> 24) );
    value = E.u[1];
    E.u[1] = ( (value << 24) | ((value & 0xFF00) << 8) | ((value & 0xFF0000) >> 8) | (value >> 24) );
    value = E.u[2];
    E.u[2] = ( (value << 24) | ((value & 0xFF00) << 8) | ((value & 0xFF0000) >> 8) | (value >> 24) );
    value = E.u[3];
    E.u[3] = ( (value << 24) | ((value & 0xFF00) << 8) | ((value & 0xFF0000) >> 8) | (value >> 24) );
    return E.v;
#endif
}


#if !defined(_M_ARM) && !defined(__arm__) && !defined(__aarch64__)
namespace SSSE3
{

inline bool XMVerifySSSE3Support()
{
    // Should return true on AMD Bulldozer, Intel Core i7/i5/i3, Intel Atom, or later processors

    // See http://msdn.microsoft.com/en-us/library/hskdteyh.aspx
    int CPUInfo[4] = {-1};
#if defined(__clang__) || defined(__GNUC__)
    __cpuid(0, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
#else
    __cpuid(CPUInfo, 0);
#endif


    if ( CPUInfo[0] < 1  )
        return false;

#if defined(__clang__) || defined(__GNUC__)
    __cpuid(1, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
#else
    __cpuid(CPUInfo, 1);
#endif

    // Check for SSSE3 instruction set.
    return ( (CPUInfo[2] & 0x200) != 0 );
}

inline XMVECTOR XM_CALLCONV XMVectorEndian
(
    FXMVECTOR V 
)
{
    static const XMVECTORU32 idx = { 0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F };
   
    __m128i Result = _mm_shuffle_epi8( _mm_castps_si128(V), idx );
    return _mm_castsi128_ps( Result );
}

} // namespace SSSE3
#endif // !_M_ARM && !__arm__ && !__aarch64__

} // namespace DirectX;