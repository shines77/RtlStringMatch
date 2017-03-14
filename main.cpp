
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <cstdio>
#include <memory.h>
#include <string.h>
#include <assert.h>

#include "Support.h"
#include "stop_watch.h"

using namespace jimi;

#ifndef _DEBUG
const LONG Iteration = 1000000;
#else
const LONG Iteration = 10000;
#endif

void RtlUnicodeStringIndexOf_PerformanceTest()
{
    CONST WCHAR szFullPath[] = L"\\?\\C:\\Project\\OpenSrc\\Shines77\\jimi_http\\src\\test\\http_parser_test\\Stop_Watch.h";
    CONST WCHAR szSearchName1[] = L"Stop_Watch.h";
    CONST WCHAR szSearchName2[] = L"stop_watch.h";
    CONST WCHAR szSearchName3[] = L"Shines77";
    CONST WCHAR szSearchName4[] = L"shines77";

    UNICODE_STRING usFullPath, usSearchName;
    StopWatch sw;
    LONG IndexOf = 0, Sum, i;
    NTSTATUS Status;

    RtlAllocateUnicodeString(&usFullPath,   MAX_PATH * sizeof(WCHAR));
    RtlAllocateUnicodeString(&usSearchName, MAX_PATH * sizeof(WCHAR));

    //----------------------------------------------------------------------------

    RtlCopyUnicodeStringFromChar(&usFullPath,   szFullPath);
    RtlCopyUnicodeStringFromChar(&usSearchName, szSearchName1);

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        Status = RtlUnicodeStringIndexOf(&usFullPath, &usSearchName, RTL_CASE_INSENSITIVE, &IndexOf);
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        Status = RtlUnicodeStringIndexOf(&usFullPath, &usSearchName, RTL_CASE_SENSITIVE, &IndexOf);
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    //----------------------------------------------------------------------------

    RtlCopyUnicodeStringFromChar(&usFullPath,   szFullPath);
    RtlCopyUnicodeStringFromChar(&usSearchName, szSearchName2);

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        Status = RtlUnicodeStringIndexOf(&usFullPath, &usSearchName, RTL_CASE_INSENSITIVE, &IndexOf);
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        Status = RtlUnicodeStringIndexOf(&usFullPath, &usSearchName, RTL_CASE_SENSITIVE, &IndexOf);
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    //----------------------------------------------------------------------------

    RtlCopyUnicodeStringFromChar(&usFullPath,   szFullPath);
    RtlCopyUnicodeStringFromChar(&usSearchName, szSearchName3);

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        Status = RtlUnicodeStringIndexOf(&usFullPath, &usSearchName, RTL_CASE_INSENSITIVE, &IndexOf);
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        Status = RtlUnicodeStringIndexOf(&usFullPath, &usSearchName, RTL_CASE_SENSITIVE, &IndexOf);
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    //----------------------------------------------------------------------------

    RtlCopyUnicodeStringFromChar(&usFullPath,   szFullPath);
    RtlCopyUnicodeStringFromChar(&usSearchName, szSearchName4);

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        Status = RtlUnicodeStringIndexOf(&usFullPath, &usSearchName, RTL_CASE_INSENSITIVE, &IndexOf);
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        Status = RtlUnicodeStringIndexOf(&usFullPath, &usSearchName, RTL_CASE_SENSITIVE, &IndexOf);
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    //----------------------------------------------------------------------------

    RtlFreeUnicodeString(&usFullPath);
    RtlFreeUnicodeString(&usSearchName);
}

void RtlUnicodeStringReverseIndexOf_PerformanceTest()
{
    CONST WCHAR szFullPath[] = L"\\?\\C:\\Project\\OpenSrc\\Shines77\\jimi_http\\src\\test\\http_parser_test\\Stop_Watch.h";
    CONST WCHAR szSearchName1[] = L"Stop_Watch.h";
    CONST WCHAR szSearchName2[] = L"stop_watch.h";
    CONST WCHAR szSearchName3[] = L"Shines77";
    CONST WCHAR szSearchName4[] = L"shines77";

    UNICODE_STRING usFullPath, usSearchName;
    StopWatch sw;
    LONG IndexOf = 0, Sum, i;
    NTSTATUS Status;

    RtlAllocateUnicodeString(&usFullPath,   MAX_PATH * sizeof(WCHAR));
    RtlAllocateUnicodeString(&usSearchName, MAX_PATH * sizeof(WCHAR));

    //----------------------------------------------------------------------------

    RtlCopyUnicodeStringFromChar(&usFullPath,   szFullPath);
    RtlCopyUnicodeStringFromChar(&usSearchName, szSearchName1);

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        Status = RtlUnicodeStringIndexOf(&usFullPath, &usSearchName, RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        Status = RtlUnicodeStringIndexOf(&usFullPath, &usSearchName, RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    //----------------------------------------------------------------------------

    RtlCopyUnicodeStringFromChar(&usFullPath,   szFullPath);
    RtlCopyUnicodeStringFromChar(&usSearchName, szSearchName2);

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        Status = RtlUnicodeStringIndexOf(&usFullPath, &usSearchName, RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        Status = RtlUnicodeStringIndexOf(&usFullPath, &usSearchName, RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    //----------------------------------------------------------------------------

    RtlCopyUnicodeStringFromChar(&usFullPath,   szFullPath);
    RtlCopyUnicodeStringFromChar(&usSearchName, szSearchName3);

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        Status = RtlUnicodeStringIndexOf(&usFullPath, &usSearchName, RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        Status = RtlUnicodeStringIndexOf(&usFullPath, &usSearchName, RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    //----------------------------------------------------------------------------

    RtlCopyUnicodeStringFromChar(&usFullPath,   szFullPath);
    RtlCopyUnicodeStringFromChar(&usSearchName, szSearchName4);

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        Status = RtlUnicodeStringIndexOf(&usFullPath, &usSearchName, RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        Status = RtlUnicodeStringIndexOf(&usFullPath, &usSearchName, RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH, &IndexOf);
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    //----------------------------------------------------------------------------

    RtlFreeUnicodeString(&usFullPath);
    RtlFreeUnicodeString(&usSearchName);
}

void cruntime_strstr_PerformanceTest()
{
    CONST WCHAR szFullPath[] = L"\\?\\C:\\Project\\OpenSrc\\Shines77\\jimi_http\\src\\test\\http_parser_test\\Stop_Watch.h";
    CONST WCHAR szSearchName1[] = L"Stop_Watch.h";
    CONST WCHAR szSearchName2[] = L"stop_watch.h";
    CONST WCHAR szSearchName3[] = L"Shines77";
    CONST WCHAR szSearchName4[] = L"shines77";

    UNICODE_STRING usFullPath, usSearchName;
    StopWatch sw;
    LONG IndexOf = 0, Sum, i;
    wchar_t * MatchPtr;

    RtlAllocateUnicodeString(&usFullPath,   MAX_PATH * sizeof(WCHAR));
    RtlAllocateUnicodeString(&usSearchName, MAX_PATH * sizeof(WCHAR));

    //----------------------------------------------------------------------------

    RtlCopyUnicodeStringFromChar(&usFullPath,   szFullPath);
    RtlCopyUnicodeStringFromChar(&usSearchName, szSearchName1);

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        MatchPtr = wcsstr(usFullPath.Buffer, usSearchName.Buffer);
        if (MatchPtr != NULL)
            IndexOf = (LONG)((ULONG_PTR)MatchPtr - (ULONG_PTR)usFullPath.Buffer);
        else
            IndexOf = -1;
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        MatchPtr = wcsstr(usFullPath.Buffer, usSearchName.Buffer);
        if (MatchPtr != NULL)
            IndexOf = (LONG)((ULONG_PTR)MatchPtr - (ULONG_PTR)usFullPath.Buffer);
        else
            IndexOf = -1;
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    //----------------------------------------------------------------------------

    RtlCopyUnicodeStringFromChar(&usFullPath,   szFullPath);
    RtlCopyUnicodeStringFromChar(&usSearchName, szSearchName2);

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        MatchPtr = wcsstr(usFullPath.Buffer, usSearchName.Buffer);
        if (MatchPtr != NULL)
            IndexOf = (LONG)((ULONG_PTR)MatchPtr - (ULONG_PTR)usFullPath.Buffer);
        else
            IndexOf = -1;
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        MatchPtr = wcsstr(usFullPath.Buffer, usSearchName.Buffer);
        if (MatchPtr != NULL)
            IndexOf = (LONG)((ULONG_PTR)MatchPtr - (ULONG_PTR)usFullPath.Buffer);
        else
            IndexOf = -1;
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    //----------------------------------------------------------------------------

    RtlCopyUnicodeStringFromChar(&usFullPath,   szFullPath);
    RtlCopyUnicodeStringFromChar(&usSearchName, szSearchName3);

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        MatchPtr = wcsstr(usFullPath.Buffer, usSearchName.Buffer);
        if (MatchPtr != NULL)
            IndexOf = (LONG)((ULONG_PTR)MatchPtr - (ULONG_PTR)usFullPath.Buffer);
        else
            IndexOf = -1;
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        MatchPtr = wcsstr(usFullPath.Buffer, usSearchName.Buffer);
        if (MatchPtr != NULL)
            IndexOf = (LONG)((ULONG_PTR)MatchPtr - (ULONG_PTR)usFullPath.Buffer);
        else
            IndexOf = -1;
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    //----------------------------------------------------------------------------

    RtlCopyUnicodeStringFromChar(&usFullPath,   szFullPath);
    RtlCopyUnicodeStringFromChar(&usSearchName, szSearchName4);

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        MatchPtr = wcsstr(usFullPath.Buffer, usSearchName.Buffer);
        if (MatchPtr != NULL)
            IndexOf = (LONG)((ULONG_PTR)MatchPtr - (ULONG_PTR)usFullPath.Buffer);
        else
            IndexOf = -1;
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    Sum = 0;
    sw.start();
    for (i = 0; i < Iteration; ++i) {
        MatchPtr = wcsstr(usFullPath.Buffer, usSearchName.Buffer);
        if (MatchPtr != NULL)
            IndexOf = (LONG)((ULONG_PTR)MatchPtr - (ULONG_PTR)usFullPath.Buffer);
        else
            IndexOf = -1;
        noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
            Sum += IndexOf;
        }
    }
    sw.stop();

    printf("Iteration: %d, Check Sum: %11d, Time Spent: %8.3f ms\n", Iteration, Sum, sw.getElapsedMillisec());

    //----------------------------------------------------------------------------

    RtlFreeUnicodeString(&usFullPath);
    RtlFreeUnicodeString(&usSearchName);
}

void UnicodeString_IndexOf_UnitTest()
{
    printf("UnitTest:\n\n");
    RtlUnicodeStringIndexOfTest();
    printf("\n");
    RtlUnicodeStringReverseIndexOfTest();
    printf("\n");
}

void UnicodeString_IndexOf_PerformanceTest()
{
    printf("PerformanceTest:\n\n");
    RtlUnicodeStringIndexOf_PerformanceTest();
    printf("\n");
    RtlUnicodeStringReverseIndexOf_PerformanceTest();
    printf("\n");
    cruntime_strstr_PerformanceTest();
    printf("\n");
}

int main(int argc, char * argv[])
{
    UnicodeString_IndexOf_UnitTest();
    UnicodeString_IndexOf_PerformanceTest();

    ::system("pause");
    return 0;
}
