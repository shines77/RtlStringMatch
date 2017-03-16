
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

CONST WCHAR szFullPath[] = L"\\?\\C:\\Project\\OpenSrc\\Shines77\\jimi_http\\src\\test\\http_parser_test\\Stop_Watch.h";
CONST WCHAR * szSearchName[8] = {
    L"Stop_Watch.h",
    L"stop_watch.h",
    L"Shines77",
    L"shines77",
    L"\\?\\C:\\Project\\OpenSrc\\Shines77\\jimi_http\\src\\test\\http_parser_test\\Stop_Watch.h",
    L"\\?\\c:\\project\\opensrc\\shines77\\jimi_http\\src\\test\\http_parser_test\\stop_watch.h",
    L"\\?\\C:\\Project\\OpenSrc\\Shines77\\RtlStringMatch\\src\\test\\unittest\\main.cpp",
    L"\\?\\c:\\project\\opensrc\\shines77\\rtlstringmatch\\src\\test\\unittest\\main.cpp"
};

void RtlUnicodeStringIndexOf_PerformanceTest()
{
    UNICODE_STRING usFullPath, usSearchName;
    StopWatch sw;
    LONG IndexOf = 0, Sum, n, i;
    NTSTATUS Status;

    RtlAllocateUnicodeString(&usFullPath,   MAX_PATH * sizeof(WCHAR));
    RtlAllocateUnicodeString(&usSearchName, MAX_PATH * sizeof(WCHAR));

    for (n = 0; n < sizeof(szSearchName) / sizeof(WCHAR *); ++n) {
        RtlCopyUnicodeStringFromChar(&usFullPath,   szFullPath);
        RtlCopyUnicodeStringFromChar(&usSearchName, szSearchName[n]);

        Sum = 0;
        sw.start();
        for (i = 0; i < Iteration; ++i) {
            Status = RtlUnicodeStringIndexOf(&usFullPath, &usSearchName, RTL_CASE_INSENSITIVE, &IndexOf);
            noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
                Sum += IndexOf;
            }
        }
        sw.stop();

        printf("[%2d] - CaseSensitive: No,  Iter: %d, Sum: %10d, Time Spent: %8.3f ms\n",
                n, Iteration, Sum, sw.getElapsedMillisec());

        Sum = 0;
        sw.start();
        for (i = 0; i < Iteration; ++i) {
            Status = RtlUnicodeStringIndexOf(&usFullPath, &usSearchName, RTL_CASE_SENSITIVE, &IndexOf);
            noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
                Sum += IndexOf;
            }
        }
        sw.stop();

        printf("[%2d] - CaseSensitive: Yes, Iter: %d, Sum: %10d, Time Spent: %8.3f ms\n",
                n, Iteration, Sum, sw.getElapsedMillisec());
    }

    RtlFreeUnicodeString(&usFullPath);
    RtlFreeUnicodeString(&usSearchName);
}

void RtlUnicodeStringIndexOf_ReverseSearch_PerformanceTest()
{
    UNICODE_STRING usFullPath, usSearchName;
    StopWatch sw;
    LONG IndexOf = 0, Sum, n, i;
    NTSTATUS Status;

    RtlAllocateUnicodeString(&usFullPath,   MAX_PATH * sizeof(WCHAR));
    RtlAllocateUnicodeString(&usSearchName, MAX_PATH * sizeof(WCHAR));

    for (n = 0; n < sizeof(szSearchName) / sizeof(WCHAR *); ++n) {
        RtlCopyUnicodeStringFromChar(&usFullPath,   szFullPath);
        RtlCopyUnicodeStringFromChar(&usSearchName, szSearchName[n]);

        Sum = 0;
        sw.start();
        for (i = 0; i < Iteration; ++i) {
            Status = RtlUnicodeStringIndexOf(&usFullPath, &usSearchName,
                                             RTL_CASE_INSENSITIVE | RTL_REVERSE_SEARCH,
                                             &IndexOf);
            noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
                Sum += IndexOf;
            }
        }
        sw.stop();

        printf("[%2d] - CaseSensitive: No,  Iter: %d, Sum: %10d, Time Spent: %8.3f ms\n",
                n, Iteration, Sum, sw.getElapsedMillisec());

        Sum = 0;
        sw.start();
        for (i = 0; i < Iteration; ++i) {
            Status = RtlUnicodeStringIndexOf(&usFullPath, &usSearchName,
                                             RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH,
                                             &IndexOf);
            noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
                Sum += IndexOf;
            }
        }
        sw.stop();

        printf("[%2d] - CaseSensitive: Yes, Iter: %d, Sum: %10d, Time Spent: %8.3f ms\n",
                n, Iteration, Sum, sw.getElapsedMillisec());
    }

    RtlFreeUnicodeString(&usFullPath);
    RtlFreeUnicodeString(&usSearchName);
}

void cruntime_wcsstr_PerformanceTest()
{
    UNICODE_STRING usFullPath, usSearchName;
    StopWatch sw;
    LONG IndexOf = 0, Sum, n, i;
    wchar_t * MatchPtr;

    RtlAllocateUnicodeString(&usFullPath,   MAX_PATH * sizeof(WCHAR));
    RtlAllocateUnicodeString(&usSearchName, MAX_PATH * sizeof(WCHAR));

    for (n = 0; n < sizeof(szSearchName) / sizeof(WCHAR *); ++n) {
        RtlCopyUnicodeStringFromChar(&usFullPath,   szFullPath);
        RtlCopyUnicodeStringFromChar(&usSearchName, szSearchName[n]);

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

        printf("[%2d] - CaseSensitive: Yes, Iter: %d, Sum: %10d, Time Spent: %8.3f ms\n",
                n, Iteration, Sum, sw.getElapsedMillisec());
    }

    RtlFreeUnicodeString(&usFullPath);
    RtlFreeUnicodeString(&usSearchName);
}

void my_wcsstr_PerformanceTest()
{
    UNICODE_STRING usFullPath, usSearchName;
    StopWatch sw;
    LONG IndexOf = 0, Sum, n, i;
    wchar_t * MatchPtr;

    RtlAllocateUnicodeString(&usFullPath,   MAX_PATH * sizeof(WCHAR));
    RtlAllocateUnicodeString(&usSearchName, MAX_PATH * sizeof(WCHAR));

    for (n = 0; n < sizeof(szSearchName) / sizeof(WCHAR *); ++n) {
        RtlCopyUnicodeStringFromChar(&usFullPath,   szFullPath);
        RtlCopyUnicodeStringFromChar(&usSearchName, szSearchName[n]);

        Sum = 0;
        sw.start();
        for (i = 0; i < Iteration; ++i) {
            MatchPtr = my_wcsstr(usFullPath.Buffer, usSearchName.Buffer);
            if (MatchPtr != NULL)
                IndexOf = (LONG)((ULONG_PTR)MatchPtr - (ULONG_PTR)usFullPath.Buffer);
            else
                IndexOf = -1;
            noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
                Sum += IndexOf;
            }
        }
        sw.stop();

        printf("[%2d] - CaseSensitive: Yes, Iter: %d, Sum: %10d, Time Spent: %8.3f ms\n",
                n, Iteration, Sum, sw.getElapsedMillisec());
    }

    RtlFreeUnicodeString(&usFullPath);
    RtlFreeUnicodeString(&usSearchName);
}

void RtlUnicodeStringIndexOf_CaseSensitive_PerformanceTest()
{
    UNICODE_STRING usFullPath, usSearchName;
    StopWatch sw;
    LONG IndexOf = 0, Sum, n, i;
    NTSTATUS Status;

    RtlAllocateUnicodeString(&usFullPath,   MAX_PATH * sizeof(WCHAR));
    RtlAllocateUnicodeString(&usSearchName, MAX_PATH * sizeof(WCHAR));

    for (n = 0; n < sizeof(szSearchName) / sizeof(WCHAR *); ++n) {
        RtlCopyUnicodeStringFromChar(&usFullPath,   szFullPath);
        RtlCopyUnicodeStringFromChar(&usSearchName, szSearchName[n]);

        Sum = 0;
        sw.start();
        for (i = 0; i < Iteration; ++i) {
            Status = RtlUnicodeStringIndexOf_CaseSensitive(&usFullPath, &usSearchName,
                                                           RTL_CASE_SENSITIVE,
                                                           &IndexOf);
            noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
                Sum += IndexOf;
            }
        }
        sw.stop();

        printf("[%2d] - CaseSensitive: Yes, Iter: %d, Sum: %10d, Time Spent: %8.3f ms\n",
                n, Iteration, Sum, sw.getElapsedMillisec());
    }

    RtlFreeUnicodeString(&usFullPath);
    RtlFreeUnicodeString(&usSearchName);
}

void RtlUnicodeStringIndexOf_ReverseSearch_CaseSensitive_PerformanceTest()
{
    UNICODE_STRING usFullPath, usSearchName;
    StopWatch sw;
    LONG IndexOf = 0, Sum, n, i;
    NTSTATUS Status;

    RtlAllocateUnicodeString(&usFullPath,   MAX_PATH * sizeof(WCHAR));
    RtlAllocateUnicodeString(&usSearchName, MAX_PATH * sizeof(WCHAR));

    for (n = 0; n < sizeof(szSearchName) / sizeof(WCHAR *); ++n) {
        RtlCopyUnicodeStringFromChar(&usFullPath,   szFullPath);
        RtlCopyUnicodeStringFromChar(&usSearchName, szSearchName[n]);

        Sum = 0;
        sw.start();
        for (i = 0; i < Iteration; ++i) {
            Status = RtlUnicodeStringIndexOf_CaseSensitive(&usFullPath, &usSearchName,
                                                           RTL_CASE_SENSITIVE | RTL_REVERSE_SEARCH,
                                                           &IndexOf);
            noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
                Sum += IndexOf;
            }
        }
        sw.stop();

        printf("[%2d] - CaseSensitive: Yes, Iter: %d, Sum: %10d, Time Spent: %8.3f ms\n",
                n, Iteration, Sum, sw.getElapsedMillisec());
    }

    RtlFreeUnicodeString(&usFullPath);
    RtlFreeUnicodeString(&usSearchName);
}

void RtlUnicodeStringIndexOf_KMP2_CaseSensitive_PerformanceTest()
{
    UNICODE_STRING usFullPath, usSearchName;
    StopWatch sw;
    LONG IndexOf = 0, Sum, n, i;
    NTSTATUS Status;

    RtlAllocateUnicodeString(&usFullPath,   MAX_PATH * sizeof(WCHAR));
    RtlAllocateUnicodeString(&usSearchName, MAX_PATH * sizeof(WCHAR));

    for (n = 0; n < sizeof(szSearchName) / sizeof(WCHAR *); ++n) {
        RtlCopyUnicodeStringFromChar(&usFullPath,   szFullPath);
        RtlCopyUnicodeStringFromChar(&usSearchName, szSearchName[n]);

        LONG * kmpNext = Prepare_KMP2_Next(usSearchName.Buffer, usSearchName.Length / sizeof(WCHAR));

        Sum = 0;
        sw.start();
        for (i = 0; i < Iteration; ++i) {
            Status = RtlUnicodeStringIndexOf_KMP2_CaseSensitive(&usFullPath, &usSearchName,
                                                                kmpNext,
                                                                RTL_CASE_SENSITIVE,
                                                                &IndexOf);
            noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
                Sum += IndexOf;
            }
        }
        sw.stop();

        printf("[%2d] - CaseSensitive: Yes, Iter: %d, Sum: %10d, Time Spent: %8.3f ms\n",
                n, Iteration, Sum, sw.getElapsedMillisec());

        if (kmpNext != NULL) {
            RTL_FREE(kmpNext);
        }
    }

    RtlFreeUnicodeString(&usFullPath);
    RtlFreeUnicodeString(&usSearchName);
}

void RtlUnicodeStringIndexOf_KMP2a_CaseSensitive_PerformanceTest()
{
    UNICODE_STRING usFullPath, usSearchName;
    StopWatch sw;
    LONG IndexOf = 0, Sum, n, i;
    NTSTATUS Status;

    RtlAllocateUnicodeString(&usFullPath,   MAX_PATH * sizeof(WCHAR));
    RtlAllocateUnicodeString(&usSearchName, MAX_PATH * sizeof(WCHAR));

    for (n = 0; n < sizeof(szSearchName) / sizeof(WCHAR *); ++n) {
        RtlCopyUnicodeStringFromChar(&usFullPath,   szFullPath);
        RtlCopyUnicodeStringFromChar(&usSearchName, szSearchName[n]);

        LONG * kmpNext = Prepare_KMP2_Next(usSearchName.Buffer, usSearchName.Length / sizeof(WCHAR));

        Sum = 0;
        sw.start();
        for (i = 0; i < Iteration; ++i) {
            Status = RtlUnicodeStringIndexOf_KMP2a_CaseSensitive(&usFullPath, &usSearchName,
                                                                 kmpNext,
                                                                 RTL_CASE_SENSITIVE,
                                                                 &IndexOf);
            noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
                Sum += IndexOf;
            }
        }
        sw.stop();

        printf("[%2d] - CaseSensitive: Yes, Iter: %d, Sum: %10d, Time Spent: %8.3f ms\n",
                n, Iteration, Sum, sw.getElapsedMillisec());

        if (kmpNext != NULL) {
            RTL_FREE(kmpNext);
        }
    }

    RtlFreeUnicodeString(&usFullPath);
    RtlFreeUnicodeString(&usSearchName);
}

void RtlUnicodeStringIndexOf_KMP_CaseSensitive_PerformanceTest()
{
    UNICODE_STRING usFullPath, usSearchName;
    StopWatch sw;
    LONG IndexOf = 0, Sum, n, i;
    NTSTATUS Status;

    RtlAllocateUnicodeString(&usFullPath,   MAX_PATH * sizeof(WCHAR));
    RtlAllocateUnicodeString(&usSearchName, MAX_PATH * sizeof(WCHAR));

    for (n = 0; n < sizeof(szSearchName) / sizeof(WCHAR *); ++n) {
        RtlCopyUnicodeStringFromChar(&usFullPath,   szFullPath);
        RtlCopyUnicodeStringFromChar(&usSearchName, szSearchName[n]);

        LONG * kmpNext = Prepare_KMP_Next(usSearchName.Buffer, usSearchName.Length / sizeof(WCHAR));

        Sum = 0;
        sw.start();
        for (i = 0; i < Iteration; ++i) {
            Status = RtlUnicodeStringIndexOf_KMP_CaseSensitive(&usFullPath, &usSearchName,
                                                               kmpNext,
                                                               RTL_CASE_SENSITIVE,
                                                               &IndexOf);
            noif (NT_SUCCESS(Status) || (Status == STATUS_NOT_FOUND)) {
                Sum += IndexOf;
            }
        }
        sw.stop();

        printf("[%2d] - CaseSensitive: Yes, Iter: %d, Sum: %10d, Time Spent: %8.3f ms\n",
                n, Iteration, Sum, sw.getElapsedMillisec());

        if (kmpNext != NULL) {
            RTL_FREE(kmpNext);
        }
    }

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

    printf("RtlUnicodeStringIndexOf() Forward Search:\n\n");
    RtlUnicodeStringIndexOf_PerformanceTest();
    printf("\n");

    printf("RtlUnicodeStringIndexOf() Reverse Search:\n\n");
    RtlUnicodeStringIndexOf_ReverseSearch_PerformanceTest();
    printf("\n");

    printf("cruntime wscstr():\n\n");
    cruntime_wcsstr_PerformanceTest();
    printf("\n");

    printf("my_wscstr():\n\n");
    my_wcsstr_PerformanceTest();
    printf("\n");

    printf("RtlUnicodeStringIndexOf_CaseSensitive() Forward Search:\n\n");
    RtlUnicodeStringIndexOf_CaseSensitive_PerformanceTest();
    printf("\n");

    printf("RtlUnicodeStringIndexOf_CaseSensitive() Reverse Search:\n\n");
    RtlUnicodeStringIndexOf_ReverseSearch_CaseSensitive_PerformanceTest();
    printf("\n");

    printf("RtlUnicodeStringIndexOf_KMP_CaseSensitive():\n\n");
    RtlUnicodeStringIndexOf_KMP_CaseSensitive_PerformanceTest();
    printf("\n");

#if 0
    printf("RtlUnicodeStringIndexOf_KMP2_CaseSensitive():\n\n");
    RtlUnicodeStringIndexOf_KMP2_CaseSensitive_PerformanceTest();
    printf("\n");

    printf("RtlUnicodeStringIndexOf_KMP2a_CaseSensitive():\n\n");
    RtlUnicodeStringIndexOf_KMP2a_CaseSensitive_PerformanceTest();
    printf("\n");
#endif
}

int main(int argc, char * argv[])
{
#if defined(_DEBUG) && 0
    LONG * MatchTable = Prepare_KMP_PartialMatchTable(L"ABCDABD", wcslen(L"ABCDABD"));
    LONG * kmpNext = Prepare_KMP_Next(L"ABCDABD", wcslen(L"ABCDABD"));
    LONG * kmpNext2 = Prepare_KMP2_Next(L"ABCDABD", wcslen(L"ABCDABD"));

    if (MatchTable) {
        RTL_FREE(MatchTable);
    }
    if (kmpNext) {
        RTL_FREE(kmpNext);
    }
    if (kmpNext2) {
        RTL_FREE(kmpNext2);
    }

    ::system("pause");
    return 0;
#endif

    UnicodeString_IndexOf_UnitTest();
    UnicodeString_IndexOf_PerformanceTest();

    ::system("pause");
    return 0;
}
