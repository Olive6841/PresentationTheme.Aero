#pragma once
#include "ImageFile.h"
#include "UxThemeFile.h"
#include "VSUnpack.h"
#include <vector>
#include <windows.h>

namespace uxtheme
{
    class CRenderObj;

    struct LOADTHEMEMETRICS : THEMEMETRICS
    {
        std::wstring wsStrings[4];
    };

    typedef HRESULT (*PFNALLOCSECTIONS)(CUxThemeFile*, wchar_t*, unsigned, int, wchar_t*, unsigned, int, int);

    class CThemeLoader : public IParserCallBack
    {
    public:
        CThemeLoader();
        CThemeLoader(const CThemeLoader& ctlOther) = delete;

        ~CThemeLoader();

        HRESULT LoadTheme(HINSTANCE hInst, wchar_t const* pszThemeName, wchar_t const* pszColorParam, wchar_t const* pszSizeParam, HANDLE* phReuseSection, bool fGlobalTheme, bool fHighContrast);
        // LoadThemeForTesting
        // SetWindowThemeInfo
        HRESULT AddIndex(wchar_t const* pszAppName, wchar_t const* pszClassName, int iPartId, int iStateId, int iIndex, int iLen) override;
        HRESULT AddData(SHORT sTypeNum, BYTE ePrimVal, const void* pData, DWORD dwLen) override;
        int GetNextDataIndex() override { return _iLocalLen; }
        int AddToDIBDataArray(void* pDIBBits, short width, short height) override;
        HRESULT AddBaseClass(int idClass, int idBaseClass) override;
        int GetScreenPpi() override;
        // SetScreenPpi
        // GetUserLangID
        // SetUserLangID

// protected:
        HRESULT PackAndLoadTheme(HANDLE hFile, wchar_t const* pszThemeName,
                                 wchar_t const* pszColorParam, wchar_t const* pszSizeParam,
                                 unsigned cbMaxDesiredSharableSectionSize,
                                 wchar_t* pszSharableSectionName,
                                 unsigned cchSharableSectionName,
                                 wchar_t* pszNonSharableSectionName,
                                 unsigned cchNonSharableSectionName, HANDLE* phReuseSection,
                                 PFNALLOCSECTIONS pfnAllocSections);
        HRESULT CopyLocalThemeToLive(void* hFile, int iTotalLength, wchar_t const* pszThemeName, wchar_t const* pszColorParam, wchar_t const* pszSizeParam);
        HRESULT CopyNonSharableDataToLive(HANDLE hReuseSection);
        HRESULT CopyDummyNonSharableDataToLive();
        HRESULT CreateReuseSection(wchar_t const* pszSharableSectionName, HANDLE* phReuseSection);
        void FreeLocalTheme();
        HRESULT PackMetrics();
        // FixupBaseClassIndex
        BOOL KeyDrawPropertyFound(int iStateDataOffset);
        BOOL KeyTextPropertyFound(int iStateDataOffset);

        HRESULT PackDrawObject(MIXEDPTRS* u, CRenderObj* pRender, int iPartId, int iStateId);
        HRESULT PackTextObject(MIXEDPTRS* u, CRenderObj* pRender, int iPartId, int iStateId);
        HRESULT PackDrawObjects(MIXEDPTRS* uOut, CRenderObj* pRender, int iMaxPart, bool fGlobals);
        HRESULT PackTextObjects(MIXEDPTRS* uOut, CRenderObj* pRender, int iMaxPart, bool fGlobals);

        HRESULT CopyPartGroup(APPCLASSLOCAL* pac, MIXEDPTRS* u, int iPartId, int* piPartJumpTable, int iPartZeroIndex, int iBaseClassIndex, bool fGlobalsGroup);
        int GetPartOffset(CRenderObj* pRender, int iPartId);
        HRESULT CopyClassGroup(APPCLASSLOCAL* pac, MIXEDPTRS* u, APPCLASSLIVE* pacl);
        // GetMaxState
        HRESULT AddIndexInternal(wchar_t const* pszAppName, wchar_t const* pszClassName, int iPartId, int iStateId, int iIndex, int iLen);
        BOOL IndexExists(wchar_t const* pszAppName, wchar_t const* pszClassName, int iPartId, int iStateId);
        HRESULT AddMissingParent(wchar_t const* pszAppName, wchar_t const* pszClassName, int iPartId, int iStateId);
        HRESULT EmitEntryHdr(MIXEDPTRS* u, short propnum, BYTE privnum);
        int EndEntry(MIXEDPTRS* u);

        HRESULT PackImageFileInfo(DIBINFO* pdi, CImageFile* pImageObj, MIXEDPTRS* u, CRenderObj* pRender, int iPartId, int iStateId);
        HRESULT GetFontTableIndex(LOGFONTW const* pFont, unsigned short* pIndex);
        // EmitFontTable
        HRESULT AddDataInternal(short sTypeNum, unsigned char ePrimVal, void const* pData, unsigned dwLen);
        HRESULT AllocateThemeFileBytes(BYTE* upb, unsigned dwAdditionalLen);
        HRESULT EmitAndCopyBlock(MIXEDPTRS* u, void const* pSrc, unsigned dwLen);
        HRESULT EmitString(MIXEDPTRS* u, wchar_t const* pszSrc, unsigned cchSrc, int* piOffSet);
        HRESULT EmitObject(MIXEDPTRS* u, short propnum, unsigned char privnum, void* pHdr, unsigned dwHdrLen, void* pObj, unsigned dwObjLen, CRenderObj*);
        HRESULT MakeStockObject(CRenderObj* pRender, DIBINFO* pdi);
        // GetClassIdForShellTarget

        enum
        {
            MAXDATASIZE = INT_MAX
        };

        std::wstring _wsThemeFileName;
        int _iGlobalsOffset;
        int _iSysMetricsOffset;
        int _iGlobalsTextObj;
        int _iGlobalsDrawObj;
        BYTE* _pbLocalData;
        int _iLocalLen;
        std::vector<APPCLASSLOCAL> _LocalIndexes;
        ENTRYHDR* _pbEntryHdrs[5];
        int _iEntryHdrLevel;
        CUxThemeFile _LoadingThemeFile;
        LOADTHEMEMETRICS _LoadThemeMetrics;
        BOOL _fGlobalTheme;
        int _iCurrentScreenPpi;
        LANGID _wCurrentLangID;
        THEMEHDR* _hdr;
        DWORD _dwPageSize;
        std::vector<DIBDATA> _rgDIBDataArray;   // CDIBDataArray
        std::vector<int> _rgBaseClassIds;       // CThemeArray
        std::vector<LOGFONTW> _fontTable;       // CThemeArray
    };
} // namespace uxtheme
