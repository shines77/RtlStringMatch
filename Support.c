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
NTAPI
FitRtlUpcaseAnsiChar(
    _In_ CHAR SourceChar
    )
{
    RTL_PAGED_CODE();
    return __InlineUpcaseAnsiChar(SourceChar);
}

CHAR
NTAPI
FitRtlDowncaseAnsiChar(
    _In_ CHAR SourceChar
    )
{
    RTL_PAGED_CODE();
    return __InlineDowncaseAnsiChar(SourceChar);
}

WCHAR
NTAPI
FitRtlUpcaseUnicodeChar(
    _In_ WCHAR SourceChar
    )
{
    RTL_PAGED_CODE();
    return __InlineUpcaseUnicodeChar(SourceChar);
}

WCHAR
NTAPI
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
NTAPI
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
NTAPI
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
NTAPI
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

///////////////////////////////////////////////////////////////////////////
//
// Rtl: The routine of find search string in match string.
//
///////////////////////////////////////////////////////////////////////////

NTSTATUS
NTAPI
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
    CONST BOOLEAN IsReversiSearch = (BOOLEAN)((Flags & RTL_REVERSE_SEARCH) != 0);
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
        if (IsReversiSearch) {
            // Reverse search
            SaveSearchString = SearchString;
            SaveMatchString  = MatchString;
            SearchStringEnd  = SearchString + SearchLength;
            MatchStringStart = MatchString + (MatchLength - SearchLength);
            MatchStringEnd   = MatchString;
            MatchString      = MatchStringStart;
            do {
                MatchChar  = *MatchString;
                SearchChar = *SearchString;
                if (CaseInSensitive) {
                    MatchChar  = __InlineDowncaseUnicodeChar(MatchChar);
                    SearchChar = __InlineDowncaseUnicodeChar(SearchChar);
                }
                if (MatchChar != SearchChar) {
                    MatchStringStart--;
                    if (MatchStringStart < MatchStringEnd) {
                        Status = STATUS_NOT_FOUND;
                        break;
                    }
                    MatchString = MatchStringStart;
                    if (SearchString != SaveSearchString)
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
            SaveSearchString = SearchString;
            SaveMatchString  = MatchString;
            SearchStringEnd  = SearchString + SearchLength;
            MatchStringStart = MatchString;
            MatchStringEnd   = MatchString + (MatchLength - SearchLength);
            do {
                MatchChar  = *MatchString;
                SearchChar = *SearchString;
                if (CaseInSensitive) {
                    MatchChar  = __InlineDowncaseUnicodeChar(MatchChar);
                    SearchChar = __InlineDowncaseUnicodeChar(SearchChar);
                }
                if (MatchChar != SearchChar) {
                    MatchStringStart++;
                    if (MatchStringStart > MatchStringEnd) {
                        Status = STATUS_NOT_FOUND;
                        break;
                    }
                    MatchString = MatchStringStart;
                    if (SearchString != SaveSearchString)
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
NTAPI
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
NTAPI
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
NTAPI
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

VOID
NTAPI
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
NTAPI
RtlAllocateUnicodeString(
    _Out_ PUNICODE_STRING UnicodeString,
    _In_ USHORT MaximumLength
    )
{
    UnicodeString->Length = 0;
    UnicodeString->MaximumLength = MaximumLength;
    UnicodeString->Buffer = (PWCHAR)malloc(MaximumLength * sizeof(CHAR));
}

VOID
NTAPI
RtlFreeUnicodeString(
    _In_ PUNICODE_STRING UnicodeString
    )
{
    if (UnicodeString->Buffer != NULL) {
        free(UnicodeString->Buffer);
        UnicodeString->Length = 0;
        UnicodeString->MaximumLength = 0;
        UnicodeString->Buffer = NULL;
    }
}

VOID
NTAPI
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
        SourceLength = fit_min(DestString->MaximumLength, SourceString->Length);
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
NTAPI
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
        SourceLength = fit_min(DestString->MaximumLength, SourceLength);
        DestString->Length = (USHORT)SourceLength;

        RtlCopyMemory(DestString->Buffer, SourceString, SourceLength);

        if (DestString->Length < DestString->MaximumLength) {
            DestString->Buffer[SourceLength / sizeof(WCHAR)] = UNICODE_NULL;
        }
    }
}

////////////////////////////////////////////////////////////////////////

VOID
NTAPI
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
NTAPI
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
NTAPI
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
