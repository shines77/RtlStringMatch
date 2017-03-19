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
    _In_ PWCHAR MatchString,
    _In_ ULONG MatchLength,
    _In_ WCHAR SearchChar,
    _In_ BOOLEAN CaseInSensitive,
    _Inout_ PLONG IndexOf
    )
{
    USHORT i;
    WCHAR MatchChar;
    NTSTATUS Status = STATUS_NOT_FOUND;

    FLT_ASSERT(MatchString != NULL);
    FLT_ASSERT(IndexOf != NULL);

    *IndexOf = -1;

    if (CaseInSensitive) {
        SearchChar = __InlineUpcaseUnicodeChar(SearchChar);
    }
    for (i = 0; i < MatchLength; ++i) {
        MatchChar = MatchString[i];
        if (CaseInSensitive) {
            MatchChar = __InlineUpcaseUnicodeChar(MatchChar);
        }
        if (SearchChar == MatchChar) {
            *IndexOf = (LONG)(i * sizeof(WCHAR));
            Status = STATUS_SUCCESS;
            break;
        }
        if (MatchChar == L'\0')
            break;
    }

    return Status;
}

NTSTATUS
FIT_NTAPI
RtlFindCharInUnicodeChar(
    _In_ PWCHAR MatchString,
    _In_ ULONG MatchLength,
    _In_ WCHAR SearchChar,
    _In_ BOOLEAN CaseInSensitive,
    _Inout_ PLONG IndexOf
    )
{
    RTL_PAGED_CODE();

    FLT_ASSERT(MatchString != NULL);

    if (IndexOf == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    if (MatchString != NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    return __RtlFindCharInUnicodeChar(MatchString,
                                      MatchLength,
                                      SearchChar,
                                      CaseInSensitive,
                                      IndexOf);
}

NTSTATUS
FIT_NTAPI
RtlFindCharInUnicodeString(
    _In_ PUNICODE_STRING MatchString,
    _In_ WCHAR SearchChar,
    _In_ BOOLEAN CaseInSensitive,
    _Inout_ PLONG IndexOf
    )
{
    return RtlFindCharInUnicodeChar(MatchString->Buffer, MatchString->Length / sizeof(WCHAR),
                                    SearchChar,
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
    _In_ PWCHAR MatchString,
    _In_ ULONG MatchLength,
    _In_ PWCHAR SearchChars,
    _In_ ULONG SearchLength,
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

    FLT_ASSERT(SearchChars != NULL);
    FLT_ASSERT(MatchString != NULL);

    if (IndexOf == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    *IndexOf = -1;

    // The length of a substring is greater than the length of the string being matched.
    if (MatchLength < SearchLength || SearchLength <= 0) {
        goto Cleanup;
    }

    if ((ULONG_PTR)SearchChars & (ULONG_PTR)MatchString) {
        for (i = 0; i < SearchLength; ++i) {
            // Call this function inline version.
            FindStatus = __RtlFindCharInUnicodeChar(MatchString,
                                                    MatchLength,
                                                    SearchChars[i],
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
    _In_ PWCHAR MatchString,
    _In_ ULONG MatchLength,
    _In_ PWCHAR SearchString,
    _In_ ULONG SearchLength,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    CONST BOOLEAN CaseInSensitive = (BOOLEAN)((Flags & RTL_CASE_SENSITIVE) == 0);
    CONST BOOLEAN IsReverseSearch = (BOOLEAN)((Flags & RTL_REVERSE_SEARCH) != 0);
    NTSTATUS Status = STATUS_NOT_FOUND;

    PWCHAR SearchStringEnd;
    PWCHAR MatchStringStart, MatchStringEnd;
    PWCHAR SaveSearchString, SaveMatchString;
    WCHAR SearchChar, MatchChar;
#if defined(USE_MALLOC_AND_UPDOWN_CASE) && (USE_MALLOC_AND_UPDOWN_CASE != 0)
    PWCHAR NewSearchString = NULL, NewMatchString = NULL;
    PWCHAR Dest, Source;
    ULONG i;
#endif

    RTL_PAGED_CODE();

    FLT_ASSERT(MatchString != NULL);
    FLT_ASSERT(SearchString != NULL);

    if (IndexOf == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    *IndexOf = -1;

    if (SearchLength == 0) {
        *IndexOf = 0;
        Status = STATUS_NOT_FOUND;
        goto Cleanup;
    }

    // The length of a substring is greater than the length of the string being matched.
    if (MatchLength < SearchLength) {
        goto Cleanup;
    }

    FLT_ASSERT(MatchLength >= SearchLength);
    if ((ULONG_PTR)SearchString & (ULONG_PTR)MatchString) {
#if defined(USE_MALLOC_AND_UPDOWN_CASE) && (USE_MALLOC_AND_UPDOWN_CASE != 0)
        if (CaseInSensitive) {
            NewMatchString = (PWCHAR)RTL_MALLOC((MatchLength + 1) * sizeof(WCHAR));
            Dest   = NewMatchString;
            Source = MatchString;
            for (i = 0; i < MatchLength; ++i) {
                *Dest++ = __InlineDowncaseUnicodeChar(*Source++);
            }
            *Dest = UNICODE_NULL;
            NewSearchString = (PWCHAR)RTL_MALLOC((SearchLength + 1) * sizeof(WCHAR));
            Dest   = NewSearchString;
            Source = SearchString;
            for (i = 0; i < SearchLength; ++i) {
                *Dest++ = __InlineDowncaseUnicodeChar(*Source++);
            }
            *Dest = UNICODE_NULL;
            MatchString  = NewMatchString;
            SearchString = NewSearchString;
        }
#endif // USE_MALLOC_AND_UPDOWN_CASE
        SaveSearchString = SearchString;
        SaveMatchString  = MatchString;
        SearchStringEnd  = SearchString + SearchLength;
        if (IsReverseSearch) {
            // Reverse search
            MatchStringStart = MatchString + (MatchLength - SearchLength);
            MatchStringEnd   = MatchString;
            MatchString      = MatchStringStart;
            do {
                MatchChar  = *MatchString;
                SearchChar = *SearchString;
#if !defined(USE_MALLOC_AND_UPDOWN_CASE) || (USE_MALLOC_AND_UPDOWN_CASE == 0)
                if (CaseInSensitive) {
                    MatchChar  = __InlineDowncaseUnicodeChar(MatchChar);
                    SearchChar = __InlineDowncaseUnicodeChar(SearchChar);
                }
#endif // !USE_MALLOC_AND_UPDOWN_CASE
                if (MatchChar != SearchChar) {
                    MatchStringStart--;
                    if (MatchStringStart < MatchStringEnd) {
                        Status = STATUS_NOT_FOUND;
                        break;
                    }
                    MatchString = MatchStringStart;
                    SearchString = SaveSearchString;
                }
                else {
                    MatchString++;
                    SearchString++;
                    if (SearchString == SearchStringEnd) {
                        FLT_ASSERT(MatchStringStart >= SaveMatchString);
                        *IndexOf = (LONG)((MatchStringStart - SaveMatchString) * sizeof(WCHAR));
                        Status = STATUS_SUCCESS;
                        break;
                    }
                    FLT_ASSERT(MatchString < (SaveMatchString + MatchLength));
                }
            } while (1);
        }
        else {
            // Forward search
            MatchStringStart = MatchString;
            MatchStringEnd   = MatchString + (MatchLength - SearchLength);
            do {
                MatchChar  = *MatchString;
                SearchChar = *SearchString;
#if !defined(USE_MALLOC_AND_UPDOWN_CASE) || (USE_MALLOC_AND_UPDOWN_CASE == 0)
                if (CaseInSensitive) {
                    MatchChar  = __InlineDowncaseUnicodeChar(MatchChar);
                    SearchChar = __InlineDowncaseUnicodeChar(SearchChar);
                }
#endif // !USE_MALLOC_AND_UPDOWN_CASE
                if (MatchChar != SearchChar) {
                    MatchStringStart++;
                    if (MatchStringStart > MatchStringEnd) {
                        Status = STATUS_NOT_FOUND;
                        break;
                    }
                    MatchString  = MatchStringStart;
                    SearchString = SaveSearchString;
                }
                else {
                    MatchString++;
                    SearchString++;
                    if (SearchString == SearchStringEnd) {
                        FLT_ASSERT(MatchStringStart >= SaveMatchString);
                        *IndexOf = (LONG)((MatchStringStart - SaveMatchString) * sizeof(WCHAR));
                        Status = STATUS_SUCCESS;
                        break;
                    }
                    FLT_ASSERT(MatchString < (SaveMatchString + MatchLength));
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
        if (NewMatchString != NULL) {
            RTL_FREE(NewMatchString);
        }
        if (NewSearchString != NULL) {
            RTL_FREE(NewSearchString);
        }
    }
#endif
    return Status;
}

NTSTATUS
FIT_NTAPI
RtlUnicodeCharWithIndexOf(    
    _In_ PUNICODE_STRING MatchString,
    _In_ PWCHAR SearchString,
    _In_ ULONG SearchLength,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    return RtlUnicodeCharIndexOf(MatchString->Buffer, MatchString->Length / sizeof(WCHAR),
                                 SearchString, SearchLength,
                                 Flags, IndexOf);
}

NTSTATUS
FIT_NTAPI
RtlUnicodeStringWithIndexOf(
    _In_ PWCHAR MatchString,
    _In_ ULONG MatchLength,
    _In_ PUNICODE_STRING SearchString,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    return RtlUnicodeCharIndexOf(MatchString, MatchLength,
                                 SearchString->Buffer, SearchString->Length / sizeof(WCHAR),
                                 Flags, IndexOf);
}

NTSTATUS
FIT_NTAPI
RtlUnicodeStringIndexOf(    
    _In_ PUNICODE_STRING MatchString,
    _In_ PUNICODE_STRING SearchString,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    return RtlUnicodeCharIndexOf(MatchString->Buffer, MatchString->Length / sizeof(WCHAR),
                                 SearchString->Buffer, SearchString->Length / sizeof(WCHAR),
                                 Flags, IndexOf);
}

////////////////////////////////////////////////////////////////////////

NTSTATUS
FIT_NTAPI
RtlUnicodeCharIndexOf_CaseSensitive(
    _In_ PWCHAR MatchString,
    _In_ ULONG MatchLength,
    _In_ PWCHAR SearchString,
    _In_ ULONG SearchLength,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    CONST BOOLEAN IsReverseSearch = (BOOLEAN)((Flags & RTL_REVERSE_SEARCH) != 0);
    NTSTATUS Status = STATUS_NOT_FOUND;

    PWCHAR SearchStringEnd;
    PWCHAR MatchStringStart, MatchStringEnd;
    PWCHAR SaveSearchString, SaveMatchString;
    WCHAR SearchChar, MatchChar;

    RTL_PAGED_CODE();

    FLT_ASSERT(MatchString != NULL);
    FLT_ASSERT(SearchString != NULL);

    if (IndexOf == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    *IndexOf = -1;

    if (SearchLength == 0) {
        *IndexOf = 0;
        Status = STATUS_NOT_FOUND;
        goto Cleanup;
    }

    // The length of a substring is greater than the length of the string being matched.
    if (MatchLength < SearchLength) {
        goto Cleanup;
    }

    FLT_ASSERT(MatchLength >= SearchLength);
    if ((ULONG_PTR)SearchString & (ULONG_PTR)MatchString) {
        SaveSearchString = SearchString;
        SaveMatchString  = MatchString;
        SearchStringEnd  = SearchString + SearchLength;
        if (IsReverseSearch) {
            // Reverse search
            MatchStringStart = MatchString + (MatchLength - SearchLength);
            MatchStringEnd   = MatchString;
            MatchString      = MatchStringStart;
            do {
                MatchChar  = *MatchString;
                SearchChar = *SearchString;
                if (MatchChar != SearchChar) {
                    MatchStringStart--;
                    if (MatchStringStart < MatchStringEnd) {
                        Status = STATUS_NOT_FOUND;
                        break;
                    }
                    MatchString = MatchStringStart;
                    SearchString = SaveSearchString;
                }
                else {
                    MatchString++;
                    SearchString++;
                    if (SearchString == SearchStringEnd) {
                        FLT_ASSERT(MatchStringStart >= SaveMatchString);
                        *IndexOf = (LONG)((MatchStringStart - SaveMatchString) * sizeof(WCHAR));
                        Status = STATUS_SUCCESS;
                        break;
                    }
                    FLT_ASSERT(MatchString < (SaveMatchString + MatchLength));
                }
            } while (1);
        }
        else {
            // Forward search
            MatchStringStart = MatchString;
            MatchStringEnd   = MatchString + (MatchLength - SearchLength);
            do {
                MatchChar  = *MatchString;
                SearchChar = *SearchString;
                if (MatchChar != SearchChar) {
                    MatchStringStart++;
                    if (MatchStringStart > MatchStringEnd) {
                        Status = STATUS_NOT_FOUND;
                        break;
                    }
                    MatchString  = MatchStringStart;
                    SearchString = SaveSearchString;
                }
                else {
                    MatchString++;
                    SearchString++;
                    if (SearchString == SearchStringEnd) {
                        FLT_ASSERT(MatchStringStart >= SaveMatchString);
                        *IndexOf = (LONG)((MatchStringStart - SaveMatchString) * sizeof(WCHAR));
                        Status = STATUS_SUCCESS;
                        break;
                    }
                    FLT_ASSERT(MatchString < (SaveMatchString + MatchLength));
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
    _In_ PUNICODE_STRING MatchString,
    _In_ PUNICODE_STRING SearchString,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    return RtlUnicodeCharIndexOf_CaseSensitive(MatchString->Buffer, MatchString->Length / sizeof(WCHAR),
                                               SearchString->Buffer, SearchString->Length / sizeof(WCHAR),
                                               Flags, IndexOf);
}

////////////////////////////////////////////////////////////////////////

LONG *
Prepare_KMP2_Next(
    _In_ PWCHAR SearchString,
    _In_ ULONG SearchLength
    )
{
    LONG * kmpNext;
    LONG match, search;

    kmpNext = RTL_MALLOC((SearchLength + 1) * sizeof(LONG));
    if (kmpNext != NULL) {
        match = 0;
        search = -1;
        kmpNext[0] = -1;
        while (match < (LONG)SearchLength) {
            while (search > -1 && SearchString[match] != SearchString[search]) {
                FLT_ASSERT(search <= (LONG)SearchLength);
                search = kmpNext[search];
            }
            match++;
            search++;
            FLT_ASSERT(match <= (LONG)SearchLength);
            FLT_ASSERT(search <= (LONG)SearchLength);
            if (SearchString[match] == SearchString[search]) {
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
    _In_ PWCHAR MatchString,
    _In_ ULONG MatchLength,
    _In_ PWCHAR SearchString,
    _In_ ULONG SearchLength,
    _In_ LONG * KmpNext,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    NTSTATUS Status = STATUS_NOT_FOUND;

    PWCHAR SaveMatchString, MatchStringEnd;
    WCHAR MatchChar;
    LONG SearchIndex, MatchIndex;
//    LONG * KmpNext = NULL;

    RTL_PAGED_CODE();

    FLT_ASSERT(MatchString != NULL);
    FLT_ASSERT(SearchString != NULL);

    if (IndexOf == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    *IndexOf = -1;

    if (SearchLength == 0) {
        *IndexOf = 0;
        Status = STATUS_NOT_FOUND;
        goto Cleanup;
    }

    // The length of a substring is greater than the length of the string being matched.
    if (MatchLength < SearchLength) {
        goto Cleanup;
    }

    FLT_ASSERT(MatchLength >= SearchLength);
    if ((ULONG_PTR)SearchString & (ULONG_PTR)MatchString) {
#if 0
        // Prepare the partial match table.
        KmpNext = Prepare_KMP_Next(SearchString, SearchLength);
        FLT_ASSERT(KmpNext != NULL);
#endif
        SaveMatchString = MatchString;
        MatchStringEnd  = MatchString + MatchLength;

        // Start search ...
        SearchIndex = 0;
        do {
            MatchChar = *MatchString;
            while (SearchString[SearchIndex] != MatchChar) {
                SearchIndex = KmpNext[SearchIndex];
                if (SearchIndex < 0) {
                    break;
                }
            }
            SearchIndex++;
            MatchString++;
            if (SearchIndex >= (LONG)SearchLength) {
                MatchIndex = (LONG)(MatchString - SaveMatchString);
                *IndexOf = (LONG)((MatchIndex - SearchIndex) * sizeof(WCHAR));
                Status = STATUS_SUCCESS;
                break;
            }            
        } while (MatchString < MatchStringEnd);
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
    _In_ PUNICODE_STRING MatchString,
    _In_ PUNICODE_STRING SearchString,
    _In_ LONG * KmpNext,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    return RtlUnicodeCharIndexOf_KMP2_CaseSensitive(MatchString->Buffer, MatchString->Length / sizeof(WCHAR),
                                                    SearchString->Buffer, SearchString->Length / sizeof(WCHAR),
                                                    KmpNext, Flags, IndexOf);
}

////////////////////////////////////////////////////////////////////////

NTSTATUS
FIT_NTAPI
RtlUnicodeCharIndexOf_KMP2a_CaseSensitive(
    _In_ PWCHAR MatchString,
    _In_ ULONG MatchLength,
    _In_ PWCHAR SearchString,
    _In_ ULONG SearchLength,
    _In_ LONG * KmpNext,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    NTSTATUS Status = STATUS_NOT_FOUND;

    LONG SearchIndex, MatchIndex;
//    LONG * KmpNext = NULL;

    RTL_PAGED_CODE();

    FLT_ASSERT(MatchString != NULL);
    FLT_ASSERT(SearchString != NULL);

    if (IndexOf == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    *IndexOf = -1;

    if (SearchLength == 0) {
        *IndexOf = 0;
        Status = STATUS_NOT_FOUND;
        goto Cleanup;
    }

    // The length of a substring is greater than the length of the string being matched.
    if (MatchLength < SearchLength) {
        goto Cleanup;
    }

    FLT_ASSERT(MatchLength >= SearchLength);
    if ((ULONG_PTR)SearchString & (ULONG_PTR)MatchString) {
#if 0
        // Prepare the partial match table.
        KmpNext = Prepare_KMP_Next(SearchString, SearchLength);
        FLT_ASSERT(KmpNext != NULL);
#endif

        // Start search ...
        SearchIndex = MatchIndex = 0;
        while (MatchIndex < (LONG)MatchLength) {
            while ((SearchIndex >= 0) && (SearchString[SearchIndex] != MatchString[MatchIndex])) {
                SearchIndex = KmpNext[SearchIndex];
            }
            SearchIndex++;
            MatchIndex++;
            if (SearchIndex >= (LONG)SearchLength) {
                *IndexOf = (LONG)((MatchIndex - SearchIndex) * sizeof(WCHAR));
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
    _In_ PUNICODE_STRING MatchString,
    _In_ PUNICODE_STRING SearchString,
    _In_ LONG * KmpNext,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    return RtlUnicodeCharIndexOf_KMP2a_CaseSensitive(MatchString->Buffer, MatchString->Length / sizeof(WCHAR),
                                                     SearchString->Buffer, SearchString->Length / sizeof(WCHAR),
                                                     KmpNext, Flags, IndexOf);
}

////////////////////////////////////////////////////////////////////////

LONG *
Prepare_KMP_PartialMatchTable(
    _In_ PWCHAR SearchString,
    _In_ ULONG SearchLength
    )
{
    LONG * MatchTable;
    ULONG max_comm_len;
    BOOLEAN is_equal;
    WCHAR * prefix, * suffix;
    ULONG step_len, cmp_len, i;
    MatchTable = RTL_MALLOC(SearchLength * sizeof(LONG));
    if (MatchTable != NULL) {
        MatchTable[0] = 0;
        for (step_len = 2; step_len <= SearchLength; ++step_len) {
            max_comm_len = 0;
            for (cmp_len = 1; cmp_len < step_len; ++cmp_len) {
                prefix = SearchString;
                suffix = SearchString + step_len - cmp_len;
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
            MatchTable[step_len - 1] = max_comm_len;
        }
    }
    return MatchTable;
}

LONG *
Prepare_KMP_Next(
    _In_ PWCHAR SearchString,
    _In_ ULONG SearchLength
    )
{
    LONG * KmpNext;
    ULONG Index;
    KmpNext = RTL_MALLOC(SearchLength * sizeof(LONG));
    if (KmpNext != NULL) {
        KmpNext[0] = 0;
        for (Index = 1; Index < SearchLength; ++Index) {
            if (SearchString[Index] == SearchString[KmpNext[Index - 1]]) {
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
    _In_ PWCHAR MatchString,
    _In_ ULONG MatchLength,
    _In_ PWCHAR SearchString,
    _In_ ULONG SearchLength,
    _In_ LONG * KmpNext,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    CONST BOOLEAN IsReverseSearch = (BOOLEAN)((Flags & RTL_REVERSE_SEARCH) != 0);
    NTSTATUS Status = STATUS_NOT_FOUND;

    PWCHAR SearchStringEnd;
    PWCHAR MatchStringStart, MatchStringEnd;
    PWCHAR SaveSearchString, SaveMatchString;
    WCHAR SearchChar, MatchChar;
//    LONG * KmpNext = NULL;

    RTL_PAGED_CODE();

    FLT_ASSERT(MatchString != NULL);
    FLT_ASSERT(SearchString != NULL);

    if (IndexOf == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    *IndexOf = -1;

    if (SearchLength == 0) {
        *IndexOf = 0;
        Status = STATUS_NOT_FOUND;
        goto Cleanup;
    }

    // The length of a substring is greater than the length of the string being matched.
    if (MatchLength < SearchLength) {
        goto Cleanup;
    }

    FLT_ASSERT(MatchLength >= SearchLength);
    if ((ULONG_PTR)SearchString & (ULONG_PTR)MatchString) {
#if 0
        // Prepare the partial match table.
        KmpNext = Prepare_KMP_Next(SearchString, SearchLength);
#endif

        // Save the original values.
        SaveSearchString = SearchString;
        SaveMatchString  = MatchString;
        SearchStringEnd  = SearchString + SearchLength;

        if (!IsReverseSearch || TRUE) {
            // Forward search
            MatchStringStart = MatchString;
            MatchStringEnd   = MatchString + (MatchLength - SearchLength);
            do {
                MatchChar  = *MatchString;
                SearchChar = *SearchString;
                if (MatchChar != SearchChar) {
                    if (SearchString == SaveSearchString) {
                        MatchString++;
                        MatchStringStart++;
                        if (MatchStringStart > MatchStringEnd) {
                            Status = STATUS_NOT_FOUND;
                            break;
                        }
                    }
                    else {
                        LONG SearchIndex = (LONG)(SearchString - SaveSearchString);
                        FLT_ASSERT(SearchIndex >= 1);
                        LONG SearchOffset = KmpNext[SearchIndex - 1];
                        LONG Offset = SearchIndex - SearchOffset;
                        FLT_ASSERT(Offset >= 1);
                        SearchString = SaveSearchString + SearchOffset;
                        MatchStringStart += Offset;
                        MatchString  = MatchStringStart;
                        if (MatchStringStart > MatchStringEnd) {
                            Status = STATUS_NOT_FOUND;
                            break;
                        }
                    }
                }
                else {
                    MatchString++;
                    SearchString++;
                    if (SearchString == SearchStringEnd) {
                        FLT_ASSERT(MatchStringStart >= SaveMatchString);
                        *IndexOf = (LONG)((MatchStringStart - SaveMatchString) * sizeof(WCHAR));
                        Status = STATUS_SUCCESS;
                        break;
                    }
                    FLT_ASSERT(MatchString < (SaveMatchString + MatchLength));
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
    _In_ PUNICODE_STRING MatchString,
    _In_ PUNICODE_STRING SearchString,
    _In_ LONG * KmpNext,
    _In_ ULONG Flags,
    _Inout_ PLONG IndexOf
    )
{
    return RtlUnicodeCharIndexOf_KMP_CaseSensitive(MatchString->Buffer, MatchString->Length / sizeof(WCHAR),
                                                   SearchString->Buffer, SearchString->Length / sizeof(WCHAR),
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
    int target_idx, pattern_idx;

    if (pattern_len != target_len) {
        /* Searching */
        pattern_idx = 0;
        while (pattern_idx <= target_len - pattern_len) {
            for (target_idx = pattern_len - 1; target_idx >= 0; --target_idx) {
                if ((pattern[target_idx] != target[target_idx + pattern_idx])) {
                    break;
                }
            }

            if (target_idx >= 0) {
                pattern_idx += FIT_MAX(bmGs[target_idx],
                    bmBc[target[target_idx + pattern_idx]] - pattern_len + 1 + target_idx);
            }
            else {
                FLT_ASSERT(pattern_idx >= 0);
                FLT_ASSERT(pattern_idx < target_len);
                *index_of = (LONG)(pattern_idx * sizeof(WCHAR));
                return pattern_idx;
            }
        }
    }
    else {
        pattern_idx = 0;
        for (target_idx = pattern_len - 1; target_idx >= 0; --target_idx) {
            if ((pattern[target_idx] != target[target_idx])) {
                pattern_idx = 1;
                break;
            }
        }
        if (pattern_idx == 0) {
            *index_of = 0;
            return 0;
        }
    }

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
    UNICODE_STRING usFileName, usSearchString;

    //---------------------------------------------------------------------------------------

    //
    // "\Device\Volume3\Windows\system32\NotePad.exe"
    //
    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usSearchString, L"NotePad.exe");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_INSENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(1, Status, IndexOf, 33);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_SENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(2, Status, IndexOf, 33);

    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usSearchString, L"notepad.exe");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_INSENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(3, Status, IndexOf, 33);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_SENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(4, Status, IndexOf, -1);

    //---------------------------------------------------------------------------------------

    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usSearchString, L"System32");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_INSENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(5, Status, IndexOf, 24);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_SENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(6, Status, IndexOf, 24);

    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usSearchString, L"system32");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_INSENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(7, Status, IndexOf, 24);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_SENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(8, Status, IndexOf, -1);

    //---------------------------------------------------------------------------------------

    //                                  1                  17       25        34        43
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usSearchString, L"System32");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_INSENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(9, Status, IndexOf, 24);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_SENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(10, Status, IndexOf, 24);

    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usSearchString, L"system32");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_INSENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(11, Status, IndexOf, 24);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_SENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(12, Status, IndexOf, -1);

    //---------------------------------------------------------------------------------------

    //
    // "\Device\Volume3\Windows\System32\Device01\NotePad.exe"
    //
    //                                  1                  17       25      33
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\Device01\\NotePad.exe");
    RtlInitUnicodeString(&usSearchString, L"\\Device01");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_INSENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(13, Status, IndexOf, 32);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_SENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(14, Status, IndexOf, 32);

    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\Device01\\NotePad.exe");
    RtlInitUnicodeString(&usSearchString, L"\\device01");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_INSENSITIVE, &IndexOf);
    RTL_UNICODE_ASSERT(15, Status, IndexOf, 32);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_SENSITIVE, &IndexOf);
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
    UNICODE_STRING usFileName, usSearchString;

    //---------------------------------------------------------------------------------------

    //
    // "\Device\Volume3\Windows\system32\NotePad.exe"
    //
    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usSearchString, L"NotePad.exe");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(1, Status, IndexOf, 33);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(2, Status, IndexOf, 33);

    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usSearchString, L"notepad.exe");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(3, Status, IndexOf, 33);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(4, Status, IndexOf, -1);

    //---------------------------------------------------------------------------------------

    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usSearchString, L"System32");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(5, Status, IndexOf, 24);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(6, Status, IndexOf, 24);

    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usSearchString, L"system32");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(7, Status, IndexOf, 24);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(8, Status, IndexOf, -1);

    //---------------------------------------------------------------------------------------

    //                                  1                  17       25        34        43
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usSearchString, L"System32");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(9, Status, IndexOf, 33);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(10, Status, IndexOf, 33);

    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\System32\\NotePad.exe");
    RtlInitUnicodeString(&usSearchString, L"system32");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(11, Status, IndexOf, 33);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(12, Status, IndexOf, -1);

    //---------------------------------------------------------------------------------------

    //
    // "\Device\Volume3\Windows\System32\Device01\NotePad.exe"
    //
    //                                  1                  17       25      33
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\Device01\\NotePad.exe");
    RtlInitUnicodeString(&usSearchString, L"\\Device01");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(13, Status, IndexOf, 32);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(14, Status, IndexOf, 32);

    //                                  1                  17       25        34
    RtlInitUnicodeString(&usFileName, L"\\Device\\Volume3\\Windows\\System32\\Device01\\NotePad.exe");
    RtlInitUnicodeString(&usSearchString, L"\\device01");

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(15, Status, IndexOf, 32);

    Status = RtlUnicodeStringIndexOf(&usFileName, &usSearchString, RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
    RTL_UNICODE_ASSERT(16, Status, IndexOf, -1);

    //---------------------------------------------------------------------------------------
}
