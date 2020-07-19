/*++

Copyright(c) 2016-2017 Skyinno Inc.

Module Name:
    Support.c

Environment:
    Kernel and user mode

--*/

#include "Support.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>

///////////////////////////////////////////////////////////////////////////
//
// Rtl: Upper or lower case the character.
//
///////////////////////////////////////////////////////////////////////////

FIT_FORCE_INLINE
CHAR
__InlineUpcaseAnsiChar(
    _In_ CHAR SourceChar
    )
{
    if (SourceChar >= 'a' && SourceChar <= 'z')
        return (SourceChar - 'a' + 'A');
    else
        return SourceChar;
}

FIT_FORCE_INLINE
CHAR
__InlineDowncaseAnsiChar(
    _In_ CHAR SourceChar
    )
{
    if (SourceChar >= 'A' && SourceChar <= 'Z')
        return (SourceChar - 'A' + 'a');
    else
        return SourceChar;
}

FIT_FORCE_INLINE
WCHAR
__InlineUpcaseUnicodeChar(
    _In_ WCHAR SourceChar
    )
{
    if (SourceChar >= L'a' && SourceChar <= L'z')
        return (SourceChar - L'a' + L'A');
    else
        return SourceChar;
}

FIT_FORCE_INLINE
WCHAR
__InlineDowncaseUnicodeChar(
    _In_ WCHAR SourceChar
    )
{
    if (SourceChar >= L'A' && SourceChar <= L'Z')
        return (SourceChar - L'A' + L'a');
    else
        return SourceChar;
}

CHAR
FIT_NTAPI
FitRtlUpcaseAnsiChar(
    _In_ CHAR SourceChar
    )
{
    RTL_PAGED_CODE();
    return __InlineUpcaseAnsiChar(SourceChar);
}

CHAR
FIT_NTAPI
FitRtlDowncaseAnsiChar(
    _In_ CHAR SourceChar
    )
{
    RTL_PAGED_CODE();
    return __InlineDowncaseAnsiChar(SourceChar);
}

WCHAR
FIT_NTAPI
FitRtlUpcaseUnicodeChar(
    _In_ WCHAR SourceChar
    )
{
    RTL_PAGED_CODE();
    return __InlineUpcaseUnicodeChar(SourceChar);
}

WCHAR
FIT_NTAPI
FitRtlDowncaseUnicodeChar(
    _In_ WCHAR SourceChar
    )
{
    RTL_PAGED_CODE();
    return __InlineDowncaseUnicodeChar(SourceChar);
}

///////////////////////////////////////////////////////////////////////////
//
// Rtl: The routine of find search character in match string.
//
///////////////////////////////////////////////////////////////////////////

FIT_FORCE_INLINE
NTSTATUS
__RtlFindCharInUnicodeChar(
    _In_ PWCHAR SourceString,
    _In_ ULONG SourceLength,
    _In_ WCHAR TargetChar,
    _In_ BOOLEAN CaseInSensitive,
    _Inout_ PLONG IndexOf
    )
{
    USHORT i;
    WCHAR SourceChar;
    NTSTATUS Status = STATUS_NOT_FOUND;

    FLT_ASSERT(SourceString != NULL);
    FLT_ASSERT(IndexOf != NULL);

    *IndexOf = -1;

    if (CaseInSensitive) {
        TargetChar = __InlineUpcaseUnicodeChar(TargetChar);
    }
    for (i = 0; i < SourceLength; ++i) {
        SourceChar = SourceString[i];
        if (CaseInSensitive) {
            SourceChar = __InlineUpcaseUnicodeChar(SourceChar);
        }
        if (TargetChar == SourceChar) {
            *IndexOf = (LONG)(i * sizeof(WCHAR));
            Status = STATUS_SUCCESS;
            break;
        }
        if (SourceChar == L'\0')
            break;
    }

    return Status;
}

NTSTATUS
FIT_NTAPI
RtlFindCharInUnicodeChar(
    _In_ PWCHAR SourceString,
    _In_ ULONG SourceLength,
    _In_ WCHAR TargetChar,
    _In_ BOOLEAN CaseInSensitive,
    _Inout_ PLONG IndexOf
    )
{
    RTL_PAGED_CODE();

    FLT_ASSERT(SourceString != NULL);

    if (IndexOf == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    if (SourceString != NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    return __RtlFindCharInUnicodeChar(SourceString,
                                      SourceLength,
                                      TargetChar,
                                      CaseInSensitive,
                                      IndexOf);
}

NTSTATUS
FIT_NTAPI
RtlFindCharInUnicodeString(
    _In_ PUNICODE_STRING SourceString,
    _In_ WCHAR TargetChar,
    _In_ BOOLEAN CaseInSensitive,
    _Inout_ PLONG IndexOf
    )
{
    return RtlFindCharInUnicodeChar(SourceString->Buffer, SourceString->Length / sizeof(WCHAR),
                                    TargetChar,
                                    CaseInSensitive, IndexOf);
}

///////////////////////////////////////////////////////////////////////////
//
// Rtl: The routine of find search characters in match string.
//
///////////////////////////////////////////////////////////////////////////

//
//
// Reference:
//
// RtlPrefixUnicodeString()
//   https://doxygen.reactos.org/df/d18/sdk_2lib_2rtl_2unicode_8c.html#a10d294efa6b9a2363116ad1cd670bc16
//
// RtlFindCharInUnicodeString()
//   https://doxygen.reactos.org/df/d18/sdk_2lib_2rtl_2unicode_8c.html#a891428ca9fc13d3c9e34269c96271b06
//
//
NTSTATUS
FIT_NTAPI
RtlFindCharsInUnicodeChar(
    _In_ PWCHAR SourceString,
    _In_ ULONG SourceLength,
    _In_ PWCHAR TargetChars,
    _In_ ULONG TargetLength,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    USHORT i;
    CONST BOOLEAN CaseInSensitive = (BOOLEAN)((Flags & RTL_CASE_INSENSITIVE) != 0);
    NTSTATUS Status = STATUS_NOT_FOUND;
    NTSTATUS FindStatus;
    LONG IndexOfChar;

    RTL_PAGED_CODE();

    FLT_ASSERT(TargetChars != NULL);
    FLT_ASSERT(SourceString != NULL);

    if (IndexOf == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    *IndexOf = -1;

    // The length of a substring is greater than the length of the string being matched.
    if (SourceLength < TargetLength || TargetLength <= 0) {
        goto Cleanup;
    }

    if ((ULONG_PTR)TargetChars & (ULONG_PTR)SourceString) {
        for (i = 0; i < TargetLength; ++i) {
            // Call this function inline version.
            FindStatus = __RtlFindCharInUnicodeChar(SourceString,
                                                    SourceLength,
                                                    TargetChars[i],
                                                    CaseInSensitive,
                                                    &IndexOfChar);
            if (NT_SUCCESS(FindStatus) && IndexOfChar >= 0) {
                *IndexOf = (LONG)(i * sizeof(WCHAR));
                Status = STATUS_SUCCESS;
                goto Cleanup;
            }
        }
    }
    else {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

Cleanup:
    return Status;
}

#define USE_MALLOC_AND_UPDOWN_CASE   0

///////////////////////////////////////////////////////////////////////////
//
// Rtl: The routine of find search string in match string.
//
///////////////////////////////////////////////////////////////////////////

NTSTATUS
FIT_NTAPI
RtlUnicodeCharIndexOf(
    _In_ PWCHAR SourceString,
    _In_ ULONG SourceLength,
    _In_ PWCHAR TargetString,
    _In_ ULONG TargetLength,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    CONST BOOLEAN CaseInSensitive = (BOOLEAN)((Flags & RTL_CASE_SENSITIVE) == 0);
    CONST BOOLEAN IsReverseSearch = (BOOLEAN)((Flags & RTL_REVERSE_SEARCH) != 0);
    NTSTATUS Status = STATUS_NOT_FOUND;

    PWCHAR TargetStringEnd;
    PWCHAR SourceStringStart, SourceStringEnd;
    PWCHAR TargetStringFirst, SourceStringFirst;
    WCHAR TargetChar, SourceChar;
#if defined(USE_MALLOC_AND_UPDOWN_CASE) && (USE_MALLOC_AND_UPDOWN_CASE != 0)
    PWCHAR NewTargetString = NULL, NewSourceString = NULL;
    PWCHAR Dest, Source;
    ULONG i;
#endif

    RTL_PAGED_CODE();

    FLT_ASSERT(SourceString != NULL);
    FLT_ASSERT(TargetString != NULL);

    if (IndexOf == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    *IndexOf = -1;

    if (TargetLength == 0) {
        *IndexOf = 0;
        Status = STATUS_NOT_FOUND;
        goto Cleanup;
    }

    // The length of a substring is greater than the length of the string being matched.
    if (SourceLength < TargetLength) {
        goto Cleanup;
    }

    FLT_ASSERT(SourceLength >= TargetLength);
    if ((ULONG_PTR)TargetString & (ULONG_PTR)SourceString) {
#if defined(USE_MALLOC_AND_UPDOWN_CASE) && (USE_MALLOC_AND_UPDOWN_CASE != 0)
        if (CaseInSensitive) {
            NewSourceString = (PWCHAR)RTL_MALLOC((SourceLength + 1) * sizeof(WCHAR));
            Dest    = NewSourceString;
            Source = SourceString;
            for (i = 0; i < SourceLength; ++i) {
                *Dest++ = __InlineDowncaseUnicodeChar(*Source++);
            }
            *Dest = UNICODE_NULL;
            NewTargetString = (PWCHAR)RTL_MALLOC((TargetLength + 1) * sizeof(WCHAR));
            Dest    = NewTargetString;
            Source = TargetString;
            for (i = 0; i < TargetLength; ++i) {
                *Dest++ = __InlineDowncaseUnicodeChar(*Source++);
            }
            *Dest = UNICODE_NULL;
            SourceString = NewSourceString;
            TargetString  = NewTargetString;
        }
#endif // USE_MALLOC_AND_UPDOWN_CASE
        TargetStringFirst  = TargetString;
        SourceStringFirst  = SourceString;
        TargetStringEnd    = TargetString + TargetLength;
        if (IsReverseSearch) {
            // Reverse search
            SourceStringStart = SourceString + (SourceLength - TargetLength);
            SourceStringEnd   = SourceString;
            SourceString      = SourceStringStart;
            do {
                SourceChar = *SourceString;
                TargetChar  = *TargetString;
#if !defined(USE_MALLOC_AND_UPDOWN_CASE) || (USE_MALLOC_AND_UPDOWN_CASE == 0)
                if (CaseInSensitive) {
                    SourceChar = __InlineDowncaseUnicodeChar(SourceChar);
                    TargetChar  = __InlineDowncaseUnicodeChar(TargetChar);
                }
#endif // !USE_MALLOC_AND_UPDOWN_CASE
                if (TargetChar != SourceChar) {
                    SourceStringStart--;
                    if (SourceStringStart < SourceStringEnd) {
                        Status = STATUS_NOT_FOUND;
                        break;
                    }
                    SourceString = SourceStringStart;
                    TargetString = TargetStringFirst;
                }
                else {
                    SourceString++;
                    TargetString++;
                    if (TargetString == TargetStringEnd) {
                        FLT_ASSERT(SourceStringStart >= SourceStringFirst);
                        *IndexOf = (LONG)((SourceStringStart - SourceStringFirst) * sizeof(WCHAR));
                        Status = STATUS_SUCCESS;
                        break;
                    }
                    FLT_ASSERT(SourceString < (SourceStringFirst + SourceLength));
                }
            } while (1);
        }
        else {
            // Forward search
            SourceStringStart = SourceString;
            SourceStringEnd   = SourceString + (SourceLength - TargetLength);
            do {
                SourceChar = *SourceString;
                TargetChar = *TargetString;
#if !defined(USE_MALLOC_AND_UPDOWN_CASE) || (USE_MALLOC_AND_UPDOWN_CASE == 0)
                if (CaseInSensitive) {
                    SourceChar = __InlineDowncaseUnicodeChar(SourceChar);
                    TargetChar  = __InlineDowncaseUnicodeChar(TargetChar);
                }
#endif // !USE_MALLOC_AND_UPDOWN_CASE
                if (TargetChar != SourceChar) {
                    SourceStringStart++;
                    if (SourceStringStart > SourceStringEnd) {
                        Status = STATUS_NOT_FOUND;
                        break;
                    }
                    SourceString = SourceStringStart;
                    TargetString = TargetStringFirst;
                }
                else {
                    SourceString++;
                    TargetString++;
                    if (TargetString == TargetStringEnd) {
                        FLT_ASSERT(SourceStringStart >= SourceStringFirst);
                        *IndexOf = (LONG)((SourceStringStart - SourceStringFirst) * sizeof(WCHAR));
                        Status = STATUS_SUCCESS;
                        break;
                    }
                    FLT_ASSERT(SourceString < (SourceStringFirst + SourceLength));
                }
            } while (1);
        }
    }
    else {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

Cleanup:
#if defined(USE_MALLOC_AND_UPDOWN_CASE) && (USE_MALLOC_AND_UPDOWN_CASE != 0)
    if (CaseInSensitive) {
        if (NewSourceString != NULL) {
            RTL_FREE(NewSourceString);
        }
        if (NewTargetString != NULL) {
            RTL_FREE(NewTargetString);
        }
    }
#endif
    return Status;
}

NTSTATUS
FIT_NTAPI
RtlUnicodeCharWithIndexOf(    
    _In_ PUNICODE_STRING SourceString,
    _In_ PWCHAR TargetString,
    _In_ ULONG TargetLength,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    return RtlUnicodeCharIndexOf(SourceString->Buffer, SourceString->Length / sizeof(WCHAR),
                                 TargetString, TargetLength,
                                 Flags, IndexOf);
}

NTSTATUS
FIT_NTAPI
RtlUnicodeStringWithIndexOf(
    _In_ PWCHAR SourceString,
    _In_ ULONG SourceLength,
    _In_ PUNICODE_STRING TargetString,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    return RtlUnicodeCharIndexOf(SourceString, SourceLength,
                                 TargetString->Buffer, TargetString->Length / sizeof(WCHAR),
                                 Flags, IndexOf);
}

NTSTATUS
FIT_NTAPI
RtlUnicodeStringIndexOf(    
    _In_ PUNICODE_STRING SourceString,
    _In_ PUNICODE_STRING TargetString,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    return RtlUnicodeCharIndexOf(SourceString->Buffer, SourceString->Length / sizeof(WCHAR),
                                 TargetString->Buffer, TargetString->Length / sizeof(WCHAR),
                                 Flags, IndexOf);
}

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
    )
{
    CONST BOOLEAN IsReverseSearch = (BOOLEAN)((Flags & RTL_REVERSE_SEARCH) != 0);
    NTSTATUS Status = STATUS_NOT_FOUND;

    PWCHAR TargetStringEnd;
    PWCHAR SourceStringStart, SourceStringEnd;
    PWCHAR TargetStringFirst, SourceStringFirst;

    RTL_PAGED_CODE();

    FLT_ASSERT(SourceString != NULL);
    FLT_ASSERT(TargetString != NULL);

    if (IndexOf == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    *IndexOf = -1;

    if (TargetLength == 0) {
        *IndexOf = 0;
        Status = STATUS_NOT_FOUND;
        goto Cleanup;
    }

    // The length of a substring is greater than the length of the string being matched.
    if (SourceLength < TargetLength) {
        goto Cleanup;
    }

    FLT_ASSERT(SourceLength >= TargetLength);
    if ((ULONG_PTR)TargetString & (ULONG_PTR)SourceString) {
        TargetStringFirst = TargetString;
        SourceStringFirst = SourceString;
        TargetStringEnd   = TargetString + TargetLength;
        if (IsReverseSearch) {
            // Reverse search
            SourceStringStart = SourceString + (SourceLength - TargetLength);
            SourceStringEnd   = SourceString;
            SourceString      = SourceStringStart;
            do {
                if (*TargetString != *SourceString) {
                    SourceStringStart--;
                    if (SourceStringStart < SourceStringEnd) {
                        Status = STATUS_NOT_FOUND;
                        break;
                    }
                    SourceString = SourceStringStart;
                    TargetString = TargetStringFirst;
                }
                else {
                    SourceString++;
                    TargetString++;
                    if (TargetString == TargetStringEnd) {
                        FLT_ASSERT(SourceStringStart >= SourceStringFirst);
                        *IndexOf = (LONG)((SourceStringStart - SourceStringFirst) * sizeof(WCHAR));
                        Status = STATUS_SUCCESS;
                        break;
                    }
                    FLT_ASSERT(SourceString < (SourceStringFirst + SourceLength));
                }
            } while (1);
        }
        else {
            // Forward search
            SourceStringStart = SourceString;
            SourceStringEnd   = SourceString + (SourceLength - TargetLength);
            do {
                if (*TargetString != *SourceString) {
                    SourceStringStart++;
                    if (SourceStringStart > SourceStringEnd) {
                        Status = STATUS_NOT_FOUND;
                        break;
                    }
                    SourceString = SourceStringStart;
                    TargetString = TargetStringFirst;
                }
                else {
                    SourceString++;
                    TargetString++;
                    if (TargetString == TargetStringEnd) {
                        FLT_ASSERT(SourceStringStart >= SourceStringFirst);
                        *IndexOf = (LONG)((SourceStringStart - SourceStringFirst) * sizeof(WCHAR));
                        Status = STATUS_SUCCESS;
                        break;
                    }
                    FLT_ASSERT(SourceString < (SourceStringFirst + SourceLength));
                }
            } while (1);
        }
    }
    else {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

Cleanup:
    return Status;
}

NTSTATUS
FIT_NTAPI
RtlUnicodeStringIndexOf_CaseSensitive(    
    _In_ PUNICODE_STRING SourceString,
    _In_ PUNICODE_STRING TargetString,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    return RtlUnicodeCharIndexOf_CaseSensitive(SourceString->Buffer, SourceString->Length / sizeof(WCHAR),
                                               TargetString->Buffer, TargetString->Length / sizeof(WCHAR),
                                               Flags, IndexOf);
}

////////////////////////////////////////////////////////////////////////

LONG *
Prepare_KMP2_Next(
    _In_ PWCHAR TargetString,
    _In_ ULONG TargetLength
    )
{
    LONG * kmpNext;
    LONG match, search;

    kmpNext = RTL_MALLOC((TargetLength + 1) * sizeof(LONG));
    if (kmpNext != NULL) {
        match = 0;
        search = -1;
        kmpNext[0] = -1;
        while (match < (LONG)TargetLength) {
            while (search > -1 && TargetString[match] != TargetString[search]) {
                FLT_ASSERT(search <= (LONG)TargetLength);
                search = kmpNext[search];
            }
            match++;
            search++;
            FLT_ASSERT(match <= (LONG)TargetLength);
            FLT_ASSERT(search <= (LONG)TargetLength);
            if (TargetString[match] == TargetString[search]) {
                kmpNext[match] = kmpNext[search];
            }
            else {
                kmpNext[match] = search;
            }
        }
    }
    return kmpNext;
}

////////////////////////////////////////////////////////////////////////

//
// See: http://www-igm.univ-mlv.fr/~lecroq/string/
// See: http://www-igm.univ-mlv.fr/~lecroq/string/node8.html#SECTION0080
//
// PS: This method seems to be inefficient, not recommended.
//
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
    )
{
    NTSTATUS Status = STATUS_NOT_FOUND;

    PWCHAR SourceStringFirst, SourceStringEnd;
    WCHAR SourceChar;
    LONG TargetIndex, SourceIndex;
//    LONG * KmpNext = NULL;

    RTL_PAGED_CODE();

    FLT_ASSERT(SourceString != NULL);
    FLT_ASSERT(TargetString != NULL);

    if (IndexOf == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    *IndexOf = -1;

    if (TargetLength == 0) {
        *IndexOf = 0;
        Status = STATUS_NOT_FOUND;
        goto Cleanup;
    }

    // The length of a substring is greater than the length of the string being matched.
    if (SourceLength < TargetLength) {
        goto Cleanup;
    }

    FLT_ASSERT(SourceLength >= TargetLength);
    if ((ULONG_PTR)TargetString & (ULONG_PTR)SourceString) {
#if 0
        // Prepare the partial match table.
        KmpNext = Prepare_KMP_Next(TargetString, TargetLength);
        FLT_ASSERT(KmpNext != NULL);
#endif
        SourceStringFirst = SourceString;
        SourceStringEnd   = SourceString + SourceLength;

        // Start search ...
        TargetIndex = 0;
        do {
            SourceChar = *SourceString;
            while (TargetString[TargetIndex] != SourceChar) {
                TargetIndex = KmpNext[TargetIndex];
                if (TargetIndex < 0) {
                    break;
                }
            }
            TargetIndex++;
            SourceString++;
            if (TargetIndex >= (LONG)TargetLength) {
                SourceIndex = (LONG)(SourceString - SourceStringFirst);
                *IndexOf = (LONG)((SourceIndex - TargetIndex) * sizeof(WCHAR));
                Status = STATUS_SUCCESS;
                break;
            }            
        } while (SourceString < SourceStringEnd);
    }
    else {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

Cleanup:
#if 0
    if (KmpNext != NULL) {
        RTL_FREE(KmpNext);
    }
#endif
    return Status;
}

NTSTATUS
FIT_NTAPI
RtlUnicodeStringIndexOf_KMP2_CaseSensitive(    
    _In_ PUNICODE_STRING SourceString,
    _In_ PUNICODE_STRING TargetString,
    _In_ LONG * KmpNext,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    return RtlUnicodeCharIndexOf_KMP2_CaseSensitive(SourceString->Buffer, SourceString->Length / sizeof(WCHAR),
                                                    TargetString->Buffer, TargetString->Length / sizeof(WCHAR),
                                                    KmpNext, Flags, IndexOf);
}

////////////////////////////////////////////////////////////////////////

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
    )
{
    NTSTATUS Status = STATUS_NOT_FOUND;

    LONG TargetIndex, SourceIndex;
    //LONG * KmpNext = NULL;

    RTL_PAGED_CODE();

    FLT_ASSERT(SourceString != NULL);
    FLT_ASSERT(TargetString != NULL);

    if (IndexOf == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    *IndexOf = -1;

    if (TargetLength == 0) {
        *IndexOf = 0;
        Status = STATUS_NOT_FOUND;
        goto Cleanup;
    }

    // The length of a substring is greater than the length of the string being matched.
    if (SourceLength < TargetLength) {
        goto Cleanup;
    }

    FLT_ASSERT(SourceLength >= TargetLength);
    if ((ULONG_PTR)TargetString & (ULONG_PTR)SourceString) {
#if 0
        // Prepare the partial match table.
        KmpNext = Prepare_KMP_Next(TargetString, TargetLength);
        FLT_ASSERT(KmpNext != NULL);
#endif

        // Start search ...
        TargetIndex = SourceIndex = 0;
        while (SourceIndex < (LONG)SourceLength) {
            while ((TargetIndex >= 0) && (TargetString[TargetIndex] != SourceString[SourceIndex])) {
                TargetIndex = KmpNext[TargetIndex];
            }
            TargetIndex++;
            SourceIndex++;
            if (TargetIndex >= (LONG)TargetLength) {
                *IndexOf = (LONG)((SourceIndex - TargetIndex) * sizeof(WCHAR));
                Status = STATUS_SUCCESS;
                break;
            }
        }
    }
    else {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

Cleanup:
#if 0
    if (KmpNext != NULL) {
        RTL_FREE(KmpNext);
    }
#endif
    return Status;
}

NTSTATUS
FIT_NTAPI
RtlUnicodeStringIndexOf_KMP2a_CaseSensitive(    
    _In_ PUNICODE_STRING SourceString,
    _In_ PUNICODE_STRING TargetString,
    _In_ LONG * KmpNext,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    return RtlUnicodeCharIndexOf_KMP2a_CaseSensitive(SourceString->Buffer, SourceString->Length / sizeof(WCHAR),
                                                     TargetString->Buffer, TargetString->Length / sizeof(WCHAR),
                                                     KmpNext, Flags, IndexOf);
}

////////////////////////////////////////////////////////////////////////

LONG *
Prepare_KMP_PartialSourceTable(
    _In_ PWCHAR TargetString,
    _In_ ULONG TargetLength
    )
{
    LONG * SourceTable;
    ULONG max_comm_len;
    BOOLEAN is_equal;
    WCHAR * prefix, * suffix;
    ULONG step_len, cmp_len, i;
    SourceTable = RTL_MALLOC(TargetLength * sizeof(LONG));
    if (SourceTable != NULL) {
        SourceTable[0] = 0;
        for (step_len = 2; step_len <= TargetLength; ++step_len) {
            max_comm_len = 0;
            for (cmp_len = 1; cmp_len < step_len; ++cmp_len) {
                prefix = TargetString;
                suffix = TargetString + step_len - cmp_len;
                is_equal = TRUE;
                for (i = 0; i < cmp_len; ++i) {
                    if (*prefix != *suffix) {
                        is_equal = FALSE;
                        break;
                    }
                    prefix++;
                    suffix++;
                }
                if ((is_equal == TRUE) && (cmp_len > max_comm_len)) {
                    max_comm_len = cmp_len;
                }
            }
            SourceTable[step_len - 1] = max_comm_len;
        }
    }
    return SourceTable;
}

LONG *
Prepare_KMP_Next(
    _In_ PWCHAR TargetString,
    _In_ ULONG TargetLength
    )
{
    LONG * KmpNext;
    ULONG Index;
    KmpNext = RTL_MALLOC(TargetLength * sizeof(LONG));
    if (KmpNext != NULL) {
        KmpNext[0] = 0;
        for (Index = 1; Index < TargetLength; ++Index) {
            if (TargetString[Index] == TargetString[KmpNext[Index - 1]]) {
                KmpNext[Index] = KmpNext[Index - 1] + 1;
            }
            else {
                KmpNext[Index] = 0;
            }
        }
    }
    return KmpNext;
}

////////////////////////////////////////////////////////////////////////

//
// KMP
//
//   See: http://www.ruanyifeng.com/blog/2013/05/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm.html
//
// Boyer-Moore
//
//   See: http://www.ruanyifeng.com/blog/2013/05/boyer-moore_string_search_algorithm.html
//
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
    )
{
    CONST BOOLEAN IsReverseSearch = (BOOLEAN)((Flags & RTL_REVERSE_SEARCH) != 0);
    NTSTATUS Status = STATUS_NOT_FOUND;

    PWCHAR TargetStringEnd;
    PWCHAR SourceStringStart, SourceStringEnd;
    PWCHAR TargetStringFirst, SourceStringFirst;
    WCHAR TargetChar, SourceChar;
//    LONG * KmpNext = NULL;

    RTL_PAGED_CODE();

    FLT_ASSERT(SourceString != NULL);
    FLT_ASSERT(TargetString != NULL);

    if (IndexOf == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    *IndexOf = -1;

    if (TargetLength == 0) {
        *IndexOf = 0;
        Status = STATUS_NOT_FOUND;
        goto Cleanup;
    }

    // The length of a substring is greater than the length of the string being matched.
    if (SourceLength < TargetLength) {
        goto Cleanup;
    }

    FLT_ASSERT(SourceLength >= TargetLength);
    if ((ULONG_PTR)TargetString & (ULONG_PTR)SourceString) {
#if 0
        // Prepare the partial match table.
        KmpNext = Prepare_KMP_Next(TargetString, TargetLength);
#endif

        // Save the original values.
        TargetStringFirst = TargetString;
        SourceStringFirst = SourceString;
        TargetStringEnd   = TargetString + TargetLength;

        if (!IsReverseSearch || TRUE) {
            // Forward search
            SourceStringStart = SourceString;
            SourceStringEnd   = SourceString + (SourceLength - TargetLength);
            do {
                SourceChar = *SourceString;
                TargetChar = *TargetString;
                if (TargetChar != SourceChar) {
                    if (TargetString == TargetStringFirst) {
                        SourceString++;
                        SourceStringStart++;
                        if (SourceStringStart > SourceStringEnd) {
                            Status = STATUS_NOT_FOUND;
                            break;
                        }
                    }
                    else {
                        LONG TargetIndex = (LONG)(TargetString - TargetStringFirst);
                        FLT_ASSERT(TargetIndex >= 1);
                        LONG TargetOffset = KmpNext[TargetIndex - 1];
                        LONG Offset = TargetIndex - TargetOffset;
                        FLT_ASSERT(Offset >= 1);
                        TargetString = TargetStringFirst + TargetOffset;
                        SourceStringStart += Offset;
                        SourceString  = SourceStringStart;
                        if (SourceStringStart > SourceStringEnd) {
                            Status = STATUS_NOT_FOUND;
                            break;
                        }
                    }
                }
                else {
                    SourceString++;
                    TargetString++;
                    if (TargetString == TargetStringEnd) {
                        FLT_ASSERT(SourceStringStart >= SourceStringFirst);
                        *IndexOf = (LONG)((SourceStringStart - SourceStringFirst) * sizeof(WCHAR));
                        Status = STATUS_SUCCESS;
                        break;
                    }
                    FLT_ASSERT(SourceString < (SourceStringFirst + SourceLength));
                }
            } while (1);
        }
        else {
            // Reverse search
        }
    }
    else {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

Cleanup:
#if 0
    if (KmpNext != NULL) {
        RTL_FREE(KmpNext);
    }
#endif
    return Status;
}

NTSTATUS
FIT_NTAPI
RtlUnicodeStringIndexOf_KMP_CaseSensitive(    
    _In_ PUNICODE_STRING SourceString,
    _In_ PUNICODE_STRING TargetString,
    _In_ LONG * KmpNext,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    return RtlUnicodeCharIndexOf_KMP_CaseSensitive(SourceString->Buffer, SourceString->Length / sizeof(WCHAR),
                                                   TargetString->Buffer, TargetString->Length / sizeof(WCHAR),
                                                   KmpNext, Flags, IndexOf);
}

////////////////////////////////////////////////////////////////////////

VOID
FIT_NTAPI
RtlInitUnicodeString(
    _Out_ PUNICODE_STRING DestString,
    _In_opt_z_ PCWSTR SourceString
    )
{
    DestString->Length = (USHORT)(wcslen(SourceString) * sizeof(WCHAR));
    DestString->MaximumLength = (USHORT)((wcslen(SourceString) + 1) * sizeof(WCHAR));
    DestString->Buffer = (PWCHAR)SourceString;
}

VOID
FIT_NTAPI
RtlAllocateUnicodeString(
    _Out_ PUNICODE_STRING UnicodeString,
    _In_ USHORT MaximumLength
    )
{
    UnicodeString->Length = 0;
    UnicodeString->MaximumLength = MaximumLength;
    UnicodeString->Buffer = (PWCHAR)RTL_MALLOC(MaximumLength * sizeof(CHAR));
}

VOID
FIT_NTAPI
RtlFreeUnicodeString(
    _In_ PUNICODE_STRING UnicodeString
    )
{
    if (UnicodeString->Buffer != NULL) {
        RTL_FREE(UnicodeString->Buffer);
        UnicodeString->Length = 0;
        UnicodeString->MaximumLength = 0;
        UnicodeString->Buffer = NULL;
    }
}

VOID
FIT_NTAPI
RtlCopyUnicodeString(
    _Inout_ PUNICODE_STRING DestString,
    _In_ PUNICODE_STRING SourceString
    )
{
    ULONG SourceLength;
    if (SourceString == NULL) {
        DestString->Length = 0;
    }
    else {
        SourceLength = FIT_MIN(DestString->MaximumLength, SourceString->Length);
        DestString->Length = (USHORT)SourceLength;

        RtlCopyMemory(DestString->Buffer,
                      SourceString->Buffer,
                      SourceLength);

        if (DestString->Length < DestString->MaximumLength) {
            DestString->Buffer[SourceLength / sizeof(WCHAR)] = UNICODE_NULL;
        }
    }
}

VOID
FIT_NTAPI
RtlCopyUnicodeStringFromChar(
    _Inout_ PUNICODE_STRING DestString,
    _In_ CONST WCHAR * SourceString
    )
{
    ULONG SourceLength = (ULONG)(wcslen(SourceString) * sizeof(WCHAR));
    if (SourceString == NULL) {
        DestString->Length = 0;
    }
    else {
        SourceLength = FIT_MIN(DestString->MaximumLength, SourceLength);
        DestString->Length = (USHORT)SourceLength;

        RtlCopyMemory(DestString->Buffer, SourceString, SourceLength);

        if (DestString->Length < DestString->MaximumLength) {
            DestString->Buffer[SourceLength / sizeof(WCHAR)] = UNICODE_NULL;
        }
    }
}

///////////////////////////////////////////////////////////////////////////
//
// Rtl: The routine of find search string in match string.
//
///////////////////////////////////////////////////////////////////////////

NTSTATUS
FIT_NTAPI
RtlUnicodeCharFastSearch(
    _In_ PWCHAR SourceString,
    _In_ ULONG SourceLength,
    _In_ PWCHAR TargetString,
    _In_ ULONG TargetLength,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    CONST BOOLEAN CaseInSensitive = (BOOLEAN)((Flags & RTL_CASE_SENSITIVE) == 0);
    CONST BOOLEAN IsReverseSearch = (BOOLEAN)((Flags & RTL_REVERSE_SEARCH) != 0);
    CONST BOOLEAN StringNoAnyCase = (BOOLEAN)((Flags & RTL_STRING_CASE_MASK) == 0);
    
    CONST BOOLEAN SourceStringNoAnyCase   = (BOOLEAN)((Flags & RTL_SOURCE_STRING_CASE_MASK) == 0);
    CONST BOOLEAN SourceStringIsDowncase  = (BOOLEAN)((Flags & RTL_SOURCE_STRING_CASE_MASK) == RTL_TARGET_STRING_IS_DOWNCASE);
    CONST BOOLEAN SourceStringIsUpcase    = (BOOLEAN)((Flags & RTL_SOURCE_STRING_CASE_MASK) == RTL_SOURCE_STRING_IS_UPCASE);

    CONST BOOLEAN TargetStringNoAnyCase  = (BOOLEAN)((Flags & RTL_TARGET_STRING_CASE_MASK) == 0);
    CONST BOOLEAN TargetStringIsDowncase = (BOOLEAN)((Flags & RTL_TARGET_STRING_CASE_MASK) == RTL_TARGET_STRING_IS_DOWNCASE);
    CONST BOOLEAN TargetStringIsUpcase   = (BOOLEAN)((Flags & RTL_TARGET_STRING_CASE_MASK) == RTL_TARGET_STRING_IS_UPCASE);

    NTSTATUS Status = STATUS_NOT_FOUND;

    PWCHAR TargetPtr, SourcePtr;
    WCHAR  TargetChar, SourceChar;

    RTL_PAGED_CODE();

    FLT_ASSERT(SourceString != NULL);
    FLT_ASSERT(TargetString != NULL);

    if (IndexOf == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    *IndexOf = -1;

    // The length of a substring is greater than the length of the string being matched.
    if (SourceLength < TargetLength || TargetLength <= 0) {
        goto Cleanup;
    }

    FLT_ASSERT(TargetLength > 0);
    FLT_ASSERT(SourceLength >= TargetLength);
    if (((ULONG_PTR)SourceString & (ULONG_PTR)TargetString) != 0) {
        // Priority reverse search.
        SourcePtr = SourceString + SourceLength - 1;
        TargetPtr  = TargetString + TargetLength - 1;
        // Start compare the search strings.
        if (!CaseInSensitive || TargetStringNoAnyCase) {
            // Don't need any case
            do {
                SourceChar = *SourcePtr;
                TargetChar = *TargetPtr;
                if (TargetChar != SourceChar) {
                    Status = STATUS_NOT_FOUND;
                    break;
                }
                else {
                    SourcePtr--;
                    TargetPtr--;
                    if (TargetPtr <= TargetString) {
                        FLT_ASSERT(SourcePtr >= SourceString);
                        *IndexOf = (LONG)((SourcePtr - SourceString) * sizeof(WCHAR));
                        Status = STATUS_SUCCESS;
                        break;
                    }
                    FLT_ASSERT(SourcePtr >= SourceString);
                    FLT_ASSERT(SourcePtr < (SourceString + SourceLength));
                }
            } while (1);
        }
        else if (TargetStringIsDowncase) {
            // Target string is downcase already.
            do {
                SourceChar = __InlineDowncaseUnicodeChar(*SourcePtr);
                TargetChar = *TargetPtr;
                if (TargetChar != SourceChar) {
                    Status = STATUS_NOT_FOUND;
                    break;
                }
                else {
                    SourcePtr--;
                    TargetPtr--;
                    if (TargetPtr <= TargetString) {
                        FLT_ASSERT(SourcePtr >= SourceString);
                        *IndexOf = (LONG)((SourcePtr - SourceString) * sizeof(WCHAR));
                        Status = STATUS_SUCCESS;
                        break;
                    }
                    FLT_ASSERT(SourcePtr >= SourceString);
                    FLT_ASSERT(SourcePtr < (SourceString + SourceLength));
                }
            } while (1);
        }
        else if (TargetStringIsUpcase) {
            // Target string is upcase already.
            do {
                SourceChar = __InlineUpcaseUnicodeChar(*SourcePtr);
                TargetChar = *TargetPtr;
                if (TargetChar != SourceChar) {
                    Status = STATUS_NOT_FOUND;
                    break;
                }
                else {
                    SourcePtr--;
                    TargetPtr--;
                    if (TargetPtr <= TargetString) {
                        FLT_ASSERT(SourcePtr >= SourceString);
                        *IndexOf = (LONG)((SourcePtr - SourceString) * sizeof(WCHAR));
                        Status = STATUS_SUCCESS;
                        break;
                    }
                    FLT_ASSERT(SourcePtr >= SourceString);
                    FLT_ASSERT(SourcePtr < (SourceString + SourceLength));
                }
            } while (1);
        }
        else {
            // String compare is case insensitive, CaseInSensitive is TRUE.
            do {
                SourceChar = __InlineUpcaseUnicodeChar(*SourcePtr);
                TargetChar = __InlineUpcaseUnicodeChar(*TargetPtr);
                if (TargetChar != SourceChar) {
                    Status = STATUS_NOT_FOUND;
                    break;
                }
                else {
                    SourcePtr--;
                    TargetPtr--;
                    if (TargetPtr <= TargetString) {
                        FLT_ASSERT(SourcePtr >= SourceString);
                        *IndexOf = (LONG)((SourcePtr - SourceString) * sizeof(WCHAR));
                        Status = STATUS_SUCCESS;
                        break;
                    }
                    FLT_ASSERT(SourcePtr >= SourceString);
                    FLT_ASSERT(SourcePtr < (SourceString + SourceLength));
                }
            } while (1);
        }
    }
    else {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

Cleanup:
    return Status;
}

////////////////////////////////////////////////////////////////////////

/***
* strstr.c - search for one string inside another
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
* Purpose:
*       defines strstr() - search for one string inside another
*
*******************************************************************************/

/***
* char *strstr(string1, string2) - search for string2 in string1
*
* Purpose:
*       finds the first occurrence of string2 in string1
*
* Entry:
*       char *string1 - string to search in
*       char *string2 - string to search for
*
* Exit:
*       returns a pointer to the first occurrence of string2 in
*       string1, or NULL if string2 does not occur in string1
*
* Uses:
*
*       Exceptions:
*
*******************************************************************************/
 
char * __stdcall my_strstr(
    const char * str1,
    const char * str2
    )
{
    char * cp = (char *)str1;
    char * s1, * s2;

    if (!*str2)
        return((char *)str1);

    while (*cp) {
        s1 = cp;
        s2 = (char *)str2;

        while (*s1 && *s2 && !(*s1 - *s2))
            s1++, s2++;

        if (!*s2)
            return (cp);

        cp++;
    }

    return (NULL);
}

wchar_t * __stdcall my_wcsstr(
    const wchar_t * str1,
    const wchar_t * str2
    )
{
    wchar_t * cp = (wchar_t *)str1;
    wchar_t * s1, * s2;

    if (!*str2)
        return((wchar_t *)str1);

    while (*cp) {
        s1 = cp;
        s2 = (wchar_t *)str2;

        while (*s1 && *s2 && !(*s1 - *s2))
            s1++, s2++;

        if (!*s2)
            return (cp);

        cp++;
    }

    return (NULL);
}

////////////////////////////////////////////////////////////////////////

#define ASIZE   256
#define XSIZE   512

int bmGs[XSIZE], bmBc[ASIZE];

void preBmBc(WCHAR * pattern, int pattern_len, int bmBc[])
{
    int i;

    for (i = 0; i < ASIZE; ++i)
        bmBc[i] = pattern_len;
    for (i = 0; i < pattern_len - 1; ++i)
        bmBc[pattern[i]] = pattern_len - i - 1;
}

void bmSuffixes(WCHAR * pattern, int length, int * suffix)
{
    int f, g, i;

    suffix[length - 1] = length;
    g = length - 1;
    for (i = length - 2; i >= 0; --i) {
        if (i > g && suffix[i + length - 1 - f] < i - g) {
            suffix[i] = suffix[i + length - 1 - f];
        }
        else {
            if (i < g) {
                g = i;
            }
            f = i;
            while ((g >= 0) && (pattern[g] == pattern[g + length - 1 - f])) {
                --g;
            }
            suffix[i] = f - g;
        }
    }
}

void preBmGs(WCHAR * pattern, int length, int bmGs[])
{
    int i, j, suffix[XSIZE];

    bmSuffixes(pattern, length, suffix);

    for (i = 0; i < length; ++i)
        bmGs[i] = length;

    j = 0;
    for (i = length - 1; i >= 0; --i) {
        if (suffix[i] == i + 1) {
            for (; j < length - 1 - i; ++j) {
                if (bmGs[j] == length) {
                    bmGs[j] = length - 1 - i;
                }
            }
        }
    }
    for (i = 0; i <= length - 2; ++i) {
        bmGs[length - 1 - suffix[i]] = length - 1 - i;
    }
}

void BoyerMoore_Preprocessing(WCHAR * pattern, int pattern_len)
{
    /* Preprocessing */

    // Preprocessing good suffixs.
    preBmGs(pattern, pattern_len, bmGs);
    // Preprocessing bad characters.
    preBmBc(pattern, pattern_len, bmBc);
}

int BoyerMoore(WCHAR * target, int target_len,
               WCHAR * pattern, int pattern_len,
               LONG * index_of)
{
    int pattern_idx, target_idx;

    // The length of a substring is greater than the length of the string being matched.
    if (target_len < pattern_len) {
        goto Cleanup;
    }

    /* Searching */
    target_idx = 0;
    while (target_idx <= target_len - pattern_len) {
        for (pattern_idx = pattern_len - 1; pattern_idx >= 0; --pattern_idx) {
            if ((pattern[pattern_idx] != target[target_idx + pattern_idx])) {
                break;
            }
        }

        if (pattern_idx >= 0) {
            target_idx += FIT_MAX(bmGs[pattern_idx],
                bmBc[target[target_idx + pattern_idx]] - ((pattern_len - 1) - pattern_idx));
        }
        else {
            FLT_ASSERT(target_idx >= 0);
            FLT_ASSERT(target_idx < target_len);
            FLT_ASSERT(index_of != NULL);
            *index_of = (LONG)(target_idx * sizeof(WCHAR));
            return target_idx;
        }
    }

Cleanup:
    FLT_ASSERT(index_of != NULL);
    *index_of = -1;
    return -1;
}

////////////////////////////////////////////////////////////////////////

VOID
FIT_NTAPI
RtlUnicodeAssert(
    _In_ ULONG No,
    _In_ NTSTATUS Status,
    _In_ LONG IndexOf,
    _In_ LONG Value
    )
{
    Value = ((Value >= 0) ? Value * sizeof(WCHAR) : Value);
    if (NT_SUCCESS(Status)) {
        if (Value == IndexOf) {
            FltDebugTraceTemp("Test %2d: Passed. IndexOf = %d\n", No, IndexOf);
        }
        else {
            FltDebugTraceTemp("Test %2d: Error.  IndexOf = %d\n", No, IndexOf);
        }
    }
    else {
        if (Status == STATUS_NOT_FOUND) {
            if (Value == IndexOf) {
                FltDebugTraceTemp("Test %2d: Passed. IndexOf = %d\n", No, IndexOf);
            }
            else {
                FltDebugTraceTemp("Test %2d: Failed, not found. Status = 0x%08X, IndexOf = %d\n", No, Status, IndexOf);
            }
        }
        else {
            FltDebugTraceTemp("Test %2d: Failed, unknown. Status = 0x%08X, IndexOf = %d\n", No, Status, IndexOf);
        }
    }
}

///////////////////////////////////////////////////////////////////////////
//
// Rtl: UnitTest routine about unicode string function.
//
///////////////////////////////////////////////////////////////////////////

VOID
FIT_NTAPI
RtlUnicodeStringIndexOfTest(
    VOID
    )
{
    NTSTATUS Status;
    LONG IndexOf = 0;
    UNICODE_STRING usFileName, usTargetStr;

    //---------------------------------------------------------------------------------------

    //
    // "\Device\Volume3\Windows\system32\NotePad.exe"
    //
    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usTargetStr, L"NotePad.exe");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_INSENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(1, Status, IndexOf, 33);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_SENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(2, Status, IndexOf, 33);

    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usTargetStr, L"notepad.exe");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_INSENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(3, Status, IndexOf, 33);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_SENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(4, Status, IndexOf, -1);

    //---------------------------------------------------------------------------------------

    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usTargetStr, L"System32");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_INSENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(5, Status, IndexOf, 24);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_SENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(6, Status, IndexOf, 24);

    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usTargetStr, L"system32");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_INSENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(7, Status, IndexOf, 24);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_SENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(8, Status, IndexOf, -1);

    //---------------------------------------------------------------------------------------

    //                                  1                  17       25        34        43
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usTargetStr, L"System32");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_INSENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(9, Status, IndexOf, 24);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_SENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(10, Status, IndexOf, 24);

    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usTargetStr, L"system32");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_INSENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(11, Status, IndexOf, 24);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_SENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(12, Status, IndexOf, -1);

    //---------------------------------------------------------------------------------------

    //
    // "\Device\Volume3\Windows\System32\Device01\NotePad.exe"
    //
    //                                  1                  17       25      33
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\Device01\\NotePad.exe");
    RtlInitUnicodeString(&usTargetStr, L"\\Device01");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_INSENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(13, Status, IndexOf, 32);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_SENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(14, Status, IndexOf, 32);

    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\Device01\\NotePad.exe");
    RtlInitUnicodeString(&usTargetStr, L"\\device01");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_INSENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(15, Status, IndexOf, 32);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_SENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(16, Status, IndexOf, -1);

    //---------------------------------------------------------------------------------------
}

VOID
FIT_NTAPI
RtlUnicodeStringReverseIndexOfTest(
    VOID
    )
{
    NTSTATUS Status;
    LONG IndexOf = 0;
    UNICODE_STRING usFileName, usTargetStr;

    //---------------------------------------------------------------------------------------

    //
    // "\Device\Volume3\Windows\system32\NotePad.exe"
    //
    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usTargetStr, L"NotePad.exe");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(1, Status, IndexOf, 33);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(2, Status, IndexOf, 33);

    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usTargetStr, L"notepad.exe");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(3, Status, IndexOf, 33);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(4, Status, IndexOf, -1);

    //---------------------------------------------------------------------------------------

    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usTargetStr, L"System32");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(5, Status, IndexOf, 24);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(6, Status, IndexOf, 24);

    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usTargetStr, L"system32");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(7, Status, IndexOf, 24);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(8, Status, IndexOf, -1);

    //---------------------------------------------------------------------------------------

    //                                  1                  17       25        34        43
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usTargetStr, L"System32");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(9, Status, IndexOf, 33);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(10, Status, IndexOf, 33);

    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usTargetStr, L"system32");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(11, Status, IndexOf, 33);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(12, Status, IndexOf, -1);

    //---------------------------------------------------------------------------------------

    //
    // "\Device\Volume3\Windows\System32\Device01\NotePad.exe"
    //
    //                                  1                  17       25      33
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\Device01\\NotePad.exe");
    RtlInitUnicodeString(&usTargetStr, L"\\Device01");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(13, Status, IndexOf, 32);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(14, Status, IndexOf, 32);

    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\Device01\\NotePad.exe");
    RtlInitUnicodeString(&usTargetStr, L"\\device01");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(15, Status, IndexOf, 32);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usTargetStr, RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(16, Status, IndexOf, -1);

    //---------------------------------------------------------------------------------------
}
