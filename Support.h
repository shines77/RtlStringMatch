/*++

Copyright(c) 2016-2017 Skyinno Inc.

Module Name:
    Support.h

Environment:
    Kernel and user mode

--*/

#ifndef FITMON_SUPPORT_H
#define FITMON_SUPPORT_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#define UMDF_USING_NTSTATUS

//
// For include <ntstatus.h> file in UMDF (User Model Driver Framework).
//
#include <ntstatus.h>

//#define WIN32_NO_STATUS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // WIN32_LEAN_AND_MEAN

#include <ntsecapi.h>
#include <tchar.h>

#ifndef RTL_PAGED_CODE
#define RTL_PAGED_CODE()    ((void)0)
#endif

#ifndef FIT_INLINE
#ifdef __cplusplus
#define FIT_FORCE_INLINE inline
#else
#define FIT_FORCE_INLINE __inline
#endif
#endif // FIT_INLINE

#ifndef FIT_FORCE_INLINE
#if (_MSC_VER >= 1200)
#define FIT_FORCE_INLINE __forceinline
#else
#define FIT_FORCE_INLINE __inline
#endif
#endif // FIT_FORCE_INLINE

#define CONST           const

#define FLT_ASSERT      assert

#ifndef NT_SUCCESS
#define NT_SUCCESS(s)   (s >= 0)
#endif // NT_SUCCESS

#if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
#define FIT_NTAPI   __stdcall
//#define FIT_NTAPI   __cdecl
#else
#define _cdecl
#define __cdecl
#define FIT_NTAPI
#endif

#ifndef noif
#define noif(statement)     ((void)0);
#endif

#if 0
typedef long                NTSTATUS;

typedef char                CHAR;
typedef char *              PCHAR;
typedef unsigned short      WCHAR;
typedef unsigned short *    PWCHAR;
typedef const PWCHAR        PCWSTR;
typedef char                BOOLEAN;
typedef long                LONG;
typedef long *              PLONG;
typedef unsigned long       ULONG;
typedef unsigned long *     PULONG;
typedef unsigned short      USHORT;
typedef unsigned short *    PUSHORT;
typedef void                VOID;
typedef void *              PVOID;

typedef size_t              ULONG_PTR;

typedef struct _UNICODE_STRING {
    USHORT Length, MaximumLength;
    PWCHAR Buffer;
} UNICODE_STRING, * PUNICODE_STRING;
#endif

#ifdef __cplusplus
    #define EXTERN_C       extern "C"
    #define EXTERN_C_START extern "C" {
    #define EXTERN_C_END   }
#else
    #define EXTERN_C       extern
    #define EXTERN_C_START
    #define EXTERN_C_END
#endif

#ifndef _In_
#define _In_
#endif

#ifndef _In_
#define _Inout_
#endif

#ifndef _In_
#define _Inout_opt
#endif

#ifndef FltDebugTraceTemp
#define FltDebugTraceTemp   printf
#endif

#ifndef RtlCopyMemory
#define RtlCopyMemory   memcpy
#endif

#ifndef UNICODE_NULL
#define UNICODE_NULL    L'\0'
#endif // UNICODE_NULL

#ifndef RTL_UNICODE_ASSERT
#define RTL_UNICODE_ASSERT(_No, _Status, _IndexOf, _Value)  RtlUnicodeAssert(_No, _Status, _IndexOf, _Value)
#endif // RTL_UNICODE_ASSERT

#ifndef FIT_MIN
#define FIT_MIN(a, b)   (((a) <= (b)) ? (a) : (b))
#endif

#ifndef FIT_MAX
#define FIT_MAX(a, b)   (((a) >= (b)) ? (a) : (b))
#endif

#ifndef RTL_MALLOC
#if !defined(_NTIFS_) && !defined(_NTDDK_)
#define RTL_MALLOC(_Size)           malloc(_Size)
#define RTL_FREE(_Ptr)              free(_Ptr)
#else
#define RTL_MALLOC(_Size)           ExAllocatePoolWithTag(NonPagedPool, _Size, RTL_STRING_TAG)
#define RTL_FREE(_Ptr)              ExFreePoolWithTag(_Ptr, RTL_STRING_TAG)
#endif
#endif // RTL_MALLOC

#define RTL_STRING_TAG                  'rtst'

////////////////////////////////////////////////////////////////////////

#define RTL_CASE_INSENSITIVE            0x00000000UL
#define RTL_CASE_SENSITIVE              0x00000001UL
#define RTL_REVERSE_SEARCH              0x00000002UL

#define RTL_SOURCE_STRING_CASE_MASK     0x00010001UL
#define RTL_SOURCE_STRING_IS_DOWNCASE   0x00010000UL
#define RTL_SOURCE_STRING_IS_UPCASE     0x00010001UL

#define RTL_TARGET_STRING_CASE_MASK     0x00020001UL
#define RTL_TARGET_STRING_IS_DOWNCASE   0x00020000UL
#define RTL_TARGET_STRING_IS_UPCASE     0x00020001UL

#define RTL_STRING_CASE_MASK            (RTL_SOURCE_STRING_CASE_MASK | RTL_TARGET_STRING_CASE_MASK)

#define RTL_INDEXOF_DEFAULT \
    (RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH)

////////////////////////////////////////////////////////////////////////

EXTERN_C_START

////////////////////////////////////////////////////////////////////////
//
// Rtl: Upper or lower case the character.
//
////////////////////////////////////////////////////////////////////////

FIT_FORCE_INLINE
CHAR
__InlineUpcaseAnsiChar(
    _In_ CHAR SourceChar
    );

FIT_FORCE_INLINE
CHAR
__InlineDowncaseAnsiChar(
    _In_ CHAR SourceChar
    );

FIT_FORCE_INLINE
WCHAR
__InlineUpcaseUnicodeChar(
    _In_ WCHAR SourceChar
    );

FIT_FORCE_INLINE
WCHAR
__InlineDowncaseUnicodeChar(
    _In_ WCHAR SourceChar
    );

////////////////////////////////////////////////////////////////////////

CHAR
FIT_NTAPI
FitRtlUpcaseAnsiChar(
    _In_ CHAR SourceChar
    );

CHAR
FIT_NTAPI
FitRtlDowncaseAnsiChar(
    _In_ CHAR SourceChar
    );

WCHAR
FIT_NTAPI
FitRtlUpcaseUnicodeChar(
    _In_ WCHAR SourceChar
    );

WCHAR
FIT_NTAPI
FitRtlDowncaseUnicodeChar(
    _In_ WCHAR SourceChar
    );

////////////////////////////////////////////////////////////////////////

NTSTATUS
FIT_NTAPI
RtlFindCharInUnicodeChar(
    _In_ PWCHAR SourceString,
    _In_ ULONG SourceLength,
    _In_ WCHAR TargetChar,
    _In_ BOOLEAN CaseInSensitive,
    _Inout_ PLONG IndexOf
    );

NTSTATUS
FIT_NTAPI
RtlFindCharInUnicodeString(
    _In_ PUNICODE_STRING SourceString,
    _In_ WCHAR TargetChar,
    _In_ BOOLEAN CaseInSensitive,
    _Inout_ PLONG IndexOf
    );

////////////////////////////////////////////////////////////////////////

NTSTATUS
FIT_NTAPI
RtlFindCharsInUnicodeChar(
    _In_ PWCHAR SourceString,
    _In_ ULONG SourceLength,
    _In_ PWCHAR TargetChars,
    _In_ ULONG TargetLength,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    );

////////////////////////////////////////////////////////////////////////

NTSTATUS
FIT_NTAPI
RtlUnicodeCharIndexOf(
    _In_ PWCHAR SourceString,
    _In_ ULONG SourceLength,
    _In_ PWCHAR TargetString,
    _In_ ULONG TargetLength,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    );

NTSTATUS
FIT_NTAPI
RtlUnicodeCharWithIndexOf(    
    _In_ PUNICODE_STRING SourceString,
    _In_ PWCHAR TargetString,
    _In_ ULONG TargetLength,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    );

NTSTATUS
FIT_NTAPI
RtlUnicodeStringWithIndexOf(
    _In_ PWCHAR SourceString,
    _In_ ULONG SourceLength,
    _In_ PUNICODE_STRING TargetString,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    );

NTSTATUS
FIT_NTAPI
RtlUnicodeStringIndexOf(
    _In_ PUNICODE_STRING SourceString,
    _In_ PUNICODE_STRING TargetString,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    );

////////////////////////////////////////////////////////////////////////

LONG *
Prepare_KMP2_Next(
    _In_ PWCHAR TargetString,
    _In_ ULONG TargetLength
    );

NTSTATUS
FIT_NTAPI
RtlUnicodeCharIndexOf_KMP2_CaseSensitive(
    _In_ PWCHAR SourceString,
    _In_ ULONG SourceLength,
    _In_ PWCHAR TargetString,
    _In_ ULONG TargetLength,
    _In_ LONG * KmpNext,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    );

NTSTATUS
FIT_NTAPI
RtlUnicodeStringIndexOf_KMP2_CaseSensitive(    
    _In_ PUNICODE_STRING SourceString,
    _In_ PUNICODE_STRING TargetString,
    _In_ LONG * KmpNext,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    );

NTSTATUS
FIT_NTAPI
RtlUnicodeCharIndexOf_KMP2a_CaseSensitive(
    _In_ PWCHAR SourceString,
    _In_ ULONG SourceLength,
    _In_ PWCHAR TargetString,
    _In_ ULONG TargetLength,
    _In_ LONG * KmpNext,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    );

NTSTATUS
FIT_NTAPI
RtlUnicodeStringIndexOf_KMP2a_CaseSensitive(    
    _In_ PUNICODE_STRING SourceString,
    _In_ PUNICODE_STRING TargetString,
    _In_ LONG * KmpNext,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    );

////////////////////////////////////////////////////////////////////////

LONG *
Prepare_KMP_PartialSourceTable(
    _In_ PWCHAR TargetString,
    _In_ ULONG TargetLength
    );

LONG *
Prepare_KMP_Next(
    _In_ PWCHAR TargetString,
    _In_ ULONG TargetLength
    );

NTSTATUS
FIT_NTAPI
RtlUnicodeCharIndexOf_KMP_CaseSensitive(
    _In_ PWCHAR SourceString,
    _In_ ULONG SourceLength,
    _In_ PWCHAR TargetString,
    _In_ ULONG TargetLength,
    _In_ LONG * KmpNext,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    );

NTSTATUS
FIT_NTAPI
RtlUnicodeStringIndexOf_KMP_CaseSensitive(    
    _In_ PUNICODE_STRING SourceString,
    _In_ PUNICODE_STRING TargetString,
    _In_ LONG * KmpNext,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    );

////////////////////////////////////////////////////////////////////////

NTSTATUS
FIT_NTAPI
RtlUnicodeCharIndexOf_CaseSensitive(
    _In_ PWCHAR SourceString,
    _In_ ULONG SourceLength,
    _In_ PWCHAR TargetString,
    _In_ ULONG TargetLength,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    );

NTSTATUS
FIT_NTAPI
RtlUnicodeStringIndexOf_CaseSensitive(    
    _In_ PUNICODE_STRING SourceString,
    _In_ PUNICODE_STRING TargetString,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    );

////////////////////////////////////////////////////////////////////////

VOID
FIT_NTAPI
RtlInitUnicodeString(
    _Out_ PUNICODE_STRING DestString,
    _In_opt_z_ PCWSTR SourceString
    );

VOID
FIT_NTAPI
RtlAllocateUnicodeString(
    _Out_ PUNICODE_STRING UnicodeString,
    _In_ USHORT MaximumLength
    );

VOID
FIT_NTAPI
RtlFreeUnicodeString(
    _In_ PUNICODE_STRING UnicodeString
    );

VOID
FIT_NTAPI
RtlCopyUnicodeString(
    _Inout_ PUNICODE_STRING DestString,
    _In_ PUNICODE_STRING SourceString
    );

VOID
FIT_NTAPI
RtlCopyUnicodeStringFromChar(
    _Inout_ PUNICODE_STRING DestString,
    _In_ CONST WCHAR * SourceString
    );

////////////////////////////////////////////////////////////////////////

NTSTATUS
FIT_NTAPI
RtlUnicodeCharFastSearch(
    _In_ PWCHAR SourceString,
    _In_ ULONG SourceLength,
    _In_ PWCHAR TargetString,
    _In_ ULONG TargetLength,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    );

////////////////////////////////////////////////////////////////////////

void BoyerMoore_Preprocessing(WCHAR * pattern, int pattern_len);

int BoyerMoore(WCHAR * target, int target_len,
               WCHAR * pattern, int pattern_len,
               LONG * index_of);

////////////////////////////////////////////////////////////////////////

char * __stdcall my_strstr(
    const char * str1,
    const char * str2
    );

wchar_t * __stdcall my_wcsstr(
    const wchar_t * str1,
    const wchar_t * str2
    );

////////////////////////////////////////////////////////////////////////

VOID
FIT_NTAPI
RtlUnicodeAssert(
    _In_ ULONG No,
    _In_ NTSTATUS Status,
    _In_ LONG IndexOf,
    _In_ LONG Value
    );

VOID
FIT_NTAPI
RtlUnicodeStringIndexOfTest(
    VOID
    );

VOID
FIT_NTAPI
RtlUnicodeStringReverseIndexOfTest(
    VOID
    );

////////////////////////////////////////////////////////////////////////

EXTERN_C_END

//---------------------------------------------------------------------------

#endif // FITMON_SUPPORT_H
