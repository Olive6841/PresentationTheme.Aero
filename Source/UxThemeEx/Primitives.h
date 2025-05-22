#pragma once
#include <vector>
#include <vssym32.h>
#include <windows.h>

namespace uxtheme
{
    struct THEMEMETRICS
    {
        LOGFONT lfFonts[9];
        COLORREF crColors[31];
        int iSizes[10];
        BOOL fBools[1];
        int iStringOffsets[4];
        int iInts[1];
    };

    struct DIBREUSEDATA
    {
        int iDIBBitsOffset;
        short iWidth;
        short iHeight;
    };

    struct DIBDATA
    {
        void* pDIBBits;
        DIBREUSEDATA dibReuseData;
    };

    union MIXEDPTRS
    {
        BYTE* pb;
        char* pc;
        WORD* pw;
        SHORT* ps;
        WCHAR* px;
        int* pi;
        DWORD* pdw;
        POINT* ppt;
        SIZE* psz;
        RECT* prc;
    };

    struct ENTRYHDR
    {
        USHORT usTypeNum;
        BYTE ePrimVal;
        DWORD dwDataLen;

        BYTE* Data()
        {
            return reinterpret_cast<BYTE*>(reinterpret_cast<uintptr_t>(this) +
                sizeof(ENTRYHDR));
        }

        BYTE const* Data() const
        {
            return reinterpret_cast<BYTE*>(reinterpret_cast<uintptr_t>(this) +
                sizeof(ENTRYHDR));
        }

        ENTRYHDR* Next()
        {
            return reinterpret_cast<ENTRYHDR*>(reinterpret_cast<uintptr_t>(this) +
                sizeof(ENTRYHDR) + dwDataLen);
        }

        ENTRYHDR const* Next() const
        {
            return reinterpret_cast<ENTRYHDR*>(reinterpret_cast<uintptr_t>(this) +
                sizeof(ENTRYHDR) + dwDataLen);
        }
    };

    struct NONSHARABLEDATAHDR
    {
        DWORD dwFlags;
        int iLoadId;
        int cBitmaps;
        int iBitmapsOffset;
    };

    struct PARTOBJHDR
    {
        int iPartId;
        int iStateId;
    };

    struct REGIONLISTHDR
    {
        char cStates;
    };

    struct alignas(4) PARTJUMPTABLEHDR
    {
        int iBaseClassIndex;
        int iFirstDrawObjIndex;
        int iFirstTextObjIndex;
        char cParts;
    };

    struct alignas(8) STATEJUMPTABLEHDR
    {
        char cStates;
    };

    struct PART_STATE_INDEX
    {
        int iPartId;
        int iStateId;
        int iIndex;
        int iLen;
    };

    struct APPCLASSINFO
    {
        int iAppNameIndex = 0;
        int iClassNameIndex = 0;
    };

    struct alignas(8) APPCLASSLIVE
    {
        APPCLASSINFO AppClassInfo;
        int iIndex;
        int iLen;
        int iBaseClassIndex;
    };

    struct APPCLASSLOCAL
    {
        std::wstring csAppName;
        std::wstring csClassName;
        int iMaxPartNum = 0;
        std::vector<PART_STATE_INDEX> PartStateIndexes;
        APPCLASSINFO AppClassInfo;
    };

    struct THEMEHDR
    {
        char szSignature[8];
        DWORD dwVersion;
        FILETIME ftModifTimeStamp;
        DWORD dwTotalLength;
        int iDllNameOffset;
        int iColorParamOffset;
        int iSizeParamOffset;
        DWORD dwLangID;
        int iLoadDPI;
        DWORD dwLoadDPIs;
        int iLoadPPI;
        int iStringsOffset;
        int iStringsLength;
        int iSectionIndexOffset;
        int iSectionIndexLength;
        int iGlobalsOffset;
        int iGlobalsTextObjOffset;
        int iGlobalsDrawObjOffset;
        int iSysMetricsOffset;
        int iFontsOffset;
        int cFonts;
    };

    union HBITMAP64
    {
        HBITMAP hBitmap;
        void* hBitmap64;
    };

    struct alignas(8) DIBINFO
    {
        HBITMAP64 uhbm;
        int iDibOffset;
        int iSingleWidth;
        int iSingleHeight;
        int iRgnListOffset;
        SIZINGTYPE eSizingType;
        BOOL fBorderOnly;
        BOOL fPartiallyTransparent;
        int iAlphaThreshold;
        int iMinDpi;
        SIZE szMinSize;
    };

    struct TRUESTRETCHINFO
    {
        BOOL fForceStretch;
        BOOL fFullStretch;
        BOOL fGdiScaledContent; // @Todo: Find out when this was added
        BOOL fGdiScaledBitBlt; // @Todo: Find out when this was added
        SIZE szDrawSize;
    };

    struct REUSEDATAHDR
    {
        WCHAR szSharableSectionName[MAX_PATH];
        int iDIBReuseRecordsCount;
        int iDIBReuseRecordsOffset;
        DWORD dwTotalLength;
    };

    typedef BOOL (CALLBACK*OVERRIDEWNDPROC)(HWND, UINT, WPARAM, LPARAM, LRESULT*, void**);
    typedef int (__fastcall *GETSCROLLINFOPROC)(HWND, int, SCROLLINFO*);
    typedef int (__fastcall *SETSCROLLINFOPROC)(HWND, int, const SCROLLINFO*, int);
    typedef int (__fastcall *ENABLESCROLLBARPROC)(HWND, unsigned int, unsigned int);
    typedef int (__fastcall *ADJUSTWINDOWRECTEXPROC)(RECT*, unsigned int, int, unsigned int);
    typedef int (__fastcall *SETWINDOWRGNPROC)(HWND, HRGN, int);
    typedef int (__fastcall *GETSYSTEMMETRICSPROC)(int);
    typedef int (__fastcall *SYSTEMPARAMETERSINFO)(unsigned int, unsigned int, void*, unsigned int);
    typedef int (__fastcall *FORCERESETUSERAPIHOOK)(HINSTANCE);
    typedef int (__fastcall *DRAWFRAMECONTROLPROC)(HDC, RECT*, unsigned int, unsigned int);
    typedef int (__fastcall *DRAWCAPTIONPROC)(HWND, HDC, const RECT*, unsigned int);
    typedef void (__fastcall *MDIREDRAWFRAMEPROC)(HWND__*, int);
    typedef void (__fastcall *BROADCASTTHEMECHANGEPROC)(WPARAM, LPARAM);
    typedef int (__fastcall *GETSYSTEMMETRICSFORDPI)(int, unsigned int);
    typedef int (__fastcall *SYSTEMPARAMETERSINFOFORDPI)(unsigned int, unsigned int, void*, unsigned int, unsigned int);
    typedef unsigned int (__fastcall *GETWINDOWPROCESSPROC)(HWND);

    typedef struct tagMSGMASK
    {
        BYTE* rgb;
        DWORD cb;
    } MSGMASK;

    typedef struct tagUSEROWPINFO
    {
        OVERRIDEWNDPROC pfnBeforeOWP;
        OVERRIDEWNDPROC pfnAfterOWP;
        MSGMASK mm;
    } USEROWPINFO;

    typedef struct tagUSERAPIHOOK
    {
        DWORD cbSize;
        WNDPROC pfnDefWindowProcA;
        WNDPROC pfnDefWindowProcW;
        MSGMASK mmDWP;
        GETSCROLLINFOPROC pfnGetScrollInfo;
        SETSCROLLINFOPROC pfnSetScrollInfo;
        ENABLESCROLLBARPROC pfnEnableScrollBar;
        ADJUSTWINDOWRECTEXPROC pfnAdjustWindowRectEx;
        SETWINDOWRGNPROC pfnSetWindowRgn;
        USEROWPINFO uoiWnd;
        USEROWPINFO uoiDlg;
        GETSYSTEMMETRICSPROC pfnGetSystemMetrics;
        SYSTEMPARAMETERSINFO pfnSystemParametersInfoA;
        SYSTEMPARAMETERSINFO pfnSystemParametersInfoW;
        FORCERESETUSERAPIHOOK pfnForceResetUserApiHook;
        DRAWFRAMECONTROLPROC pfnDrawFrameControl;
        DRAWCAPTIONPROC pfnDrawCaption;
        MDIREDRAWFRAMEPROC pfnMDIRedrawFrame;
        BROADCASTTHEMECHANGEPROC pfnBroadcastThemeChange;
        GETSYSTEMMETRICSFORDPI pfnGetSystemMetricsForDpi;
        SYSTEMPARAMETERSINFOFORDPI pfnSystemParametersInfoForDpi;
        GETWINDOWPROCESSPROC pfnGetWindowProcess;
    } USERAPIHOOK, *PUSERAPIHOOK;
} // namespace uxtheme
