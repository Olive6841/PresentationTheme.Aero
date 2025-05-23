#pragma once
#include "ImageFile.h"
#include "Primitives.h"
#include "Utils.h"
#include "VSUnpack.h"

#include <memory>
#include <mutex>
#include <vector>

namespace UxTheme
{

struct AnimationProperty;
class CRenderList;
class CTextDraw;
class CUxThemeFile;

class CStateIdObjectCache
{
public:
    template<typename T>
    HRESULT Expand(int cStates)
    {
        if (cStates > 96)
            return E_FAIL;

        GetObjects<T>().resize(cStates + 1);
        return S_OK;
    }

    template<typename T>
    std::vector<T*>& GetObjects();

private:
    std::vector<CDrawBase*> DrawObjs;
    std::vector<CTextDraw*> TextObjs;
};

template<>
inline std::vector<CDrawBase*>& CStateIdObjectCache::GetObjects<CDrawBase>()
{
    return DrawObjs;
}

template<>
inline std::vector<CTextDraw*>& CStateIdObjectCache::GetObjects<CTextDraw>()
{
    return TextObjs;
}

class CImageDecoder
{};

class CImageEncoder
{};

class CRenderCache
{
public:
    CRenderCache(CRenderObj* pRenderObj);
    void FreeDisplayFontHandle();
    HRESULT GetScaledFontHandle(HDC hdc, LOGFONTW const* plfUnscaled, HFONT* phFont);
    HRESULT GetDisplayFontHandle(HDC hdc, LOGFONTW const& lf, HFONT* phFont);

private:
    CRenderObj* _pRenderObj;
    HFONT _hFont = nullptr;
    LOGFONTW* _plfFont;
    HFONT _hDisplayFont = nullptr;
    LOGFONTW _lfDisplayFont;
};

class CRenderObj
{
public:
    static HRESULT Create(CUxThemeFile* pThemeFile, int iCacheSlot, int iThemeOffset,
                          int iAppNameOffset, int iClassNameOffset, int64_t iUniqueId,
                          bool fEnableCache, CDrawBase* pBaseObj, CTextDraw* pTextObj,
                          int iTargetDpi, bool fIsStronglyAssociatedDpi,
                          unsigned dwOtdFlags, CRenderObj** ppObj);
    ~CRenderObj();
    CRenderObj(CUxThemeFile* pThemeFile, int iCacheSlot, int iThemeOffset,
               int iClassNameOffset, int64_t iUniqueId, bool fEnableCache, int iTargetDpi,
               bool fIsStronglyAssociatedDpi, unsigned dwOtdFlags);
    
    bool GetFontTableIndex(int iPartId, int iStateId, int iPropId,
                           unsigned short* pFontIndex) const;
    HRESULT ExternalGetColor(int iPartId, int iStateId, int iPropId, COLORREF* pColor) const;
    HRESULT ExternalGetMetric(HDC hdc, int iPartId, int iStateId, int iPropId,
                              int* piVal) const;
    HRESULT ExternalGetString(int iPartId, int iStateId, int iPropId, wchar_t* pszBuff,
                              unsigned cchBuff) const;
    HRESULT ExternalGetBool(int iPartId, int iStateId, int iPropId, int* pfVal) const;
    HRESULT ExternalGetInt(int iPartId, int iStateId, int iPropId, int* piVal) const;
    HRESULT ExternalGetEnumValue(int iPartId, int iStateId, int iPropId,
                                 int* piVal) const;
    HRESULT ExternalGetPosition(int iPartId, int iStateId, int iPropId,
                                POINT* pPoint) const;
    HRESULT ExternalGetFont(HDC hdc, int iPartId, int iStateId, int iPropId,
                            bool fWantHdcScaling, LOGFONTW* pFont) const;
    HRESULT ExternalGetMargins(HDC hdc, int iPartId, int iStateId, int iPropId,
                               RECT const* formal, MARGINS* pMargins) const;
    HRESULT ExternalGetIntList(int iPartId, int iStateId, int iPropId,
                               INTLIST* pIntList) const;
    HRESULT ExternalGetRect(int iPartId, int iStateId, int iPropId, RECT* pRect) const;
    // HRESULT ExternalGetFilename(int, int, int, wchar_t*, unsigned long);
    HRESULT GetPropertyOrigin(int iPartId, int iStateId, int iTarget,
                              PROPERTYORIGIN* pOrigin) const;
    HRESULT ExternalGetStream(int iPartId, int iStateId, int iPropId, void** ppvStream,
                              DWORD* pcbStream, HINSTANCE hInst) const;
    // ENTRYHDR* GetEntry(int, int, int);
    BOOL IsPartDefined(int iPartId, int iStateId) const;
    HRESULT ExternalGetBitmap(HDC hdc, int iDibOffset, unsigned dwFlags,
                              HBITMAP* phBitmap) const;
    HRESULT GetTransitionDuration(int iPartId, int iStateIdFrom, int iStateIdTo,
                                  int iPropId, DWORD* pdwDuration) const;
    HRESULT GetAnimationProperty(int iPartId, int iStateId,
                                 AnimationProperty const** ppAnimationProperty) const;
    HRESULT GetAnimationTransform(int iPartId, int iStateId, DWORD dwIndex,
                                  TA_TRANSFORM const** ppAnimationTransform) const;
    HRESULT GetTimingFunction(int iTimingFunctionId,
                              TA_TIMINGFUNCTION const** ppTimingFunc) const;
    HRESULT GetScaledFontHandle(HDC hdc, unsigned short FontIndex, HFONT* phFont);
    HRESULT GetCachedDisplayFontHandle(HDC hdc, LOGFONTW const& lf, HFONT* phFont);
    void ReturnBitmap(HBITMAP hbmp) { DeleteObject(hbmp); }

    void ReturnFontHandle(HFONT hFont)
    {
        if (hFont && !_fCacheEnabled)
            DeleteObject(hFont);
    }
    int GetAssociatedDpi() const { return _iAssociatedDpi; }
    bool IsStronglyAssociatedDpi() const { return _fIsStronglyAssociatedDpi; }
    void GetEffectiveDpi(HDC hdc, int* px, int* py) const;
    bool IsReady() const;
    int GetValueIndex(int iPartId, int iStateId, int iTarget) const;
    ENTRYHDR* GetEntryHeader(int index) const
    {
        return reinterpret_cast<ENTRYHDR*>(_pbSharableData + index - sizeof(ENTRYHDR));
    }

    HRESULT PrepareRegionDataForScaling(RGNDATA* pRgnData, RECT const* prcImage,
                                        MARGINS* pMargins);
    TMBITMAPHEADER* GetBitmapHeader(int iDibOffset) const;
    HBITMAP BitmapIndexToHandle(int index) const { return _phBitmapsArray[index].hBitmap; }
    void SetBitmapHandle(int iBitmapIndex, HBITMAP hbmp) { _phBitmapsArray[iBitmapIndex].hBitmap = hbmp; }
    BYTE const* GetLastValidThemeByte() const;
    CRenderCache* GetCacheObject();
    HRESULT ExpandPartObjectCache(int cParts);

    template<typename T>
    PARTOBJHDR* GetNextPartObject(MIXEDPTRS* u);

    template<typename T>
    T* FindClassPartObject(BYTE* const pb, int iPartId, int iStateId);

    template<class T>
    T* FindBaseClassPartObject(int iPartId, int iStateId);

    template<typename T>
    T* FixupPartObjectCache(int iPartId, int iStateId);

    template<typename T>
    HRESULT GetPartObject(int iPartId, int iStateId, T** ppvObj);

    int GetThemeOffset() const
    {
        return narrow_cast<int>((uintptr_t)_pbSectionData - (uintptr_t)_pbSharableData);
    }

    LOGFONTW const* GetFont(int index) const
    {
        if (index < 0 || index >= countof(_ptm->lfFonts))
            return nullptr;
        return &_ptm->lfFonts[index];
    }

private:
    bool _IsDWMAtlas() const;

public:
    CUxThemeFile* _pThemeFile;
    int _iCacheSlot;
    LONGLONG _iUniqueId; /* __int64 */
    BYTE* _pbSharableData;
    BYTE* _pbSectionData;
    BYTE* _pbNonSharableData;
    HBITMAP64* _phBitmapsArray;
    bool _fCacheEnabled;
    bool _fCloseThemeFile;
    bool _fPartCacheInitialized;
    THEMEMETRICS* _ptm;
    LPCWSTR _pszClassName;
    std::vector<std::unique_ptr<CStateIdObjectCache>> _pParts; // CThemeArray
    DWORD _dwOtdFlags;

private:
    std::unique_ptr<CImageDecoder> _pPngDecoder;
    std::unique_ptr<CImageEncoder> _pPngEncoder;
    std::unique_ptr<CRenderCache> _pCacheObj;
    std::mutex _lock;
    //DiagnosticAnimationXML* _pDiagnosticXML;
    bool _fDiagnosticModeEnabled; // @Note: Used for DiagnosticAnimationXML debugging stuff
    //bool _HasDiagnosticAnimationXMLChanged(bool);
    int _iAssociatedDpi;
    bool _fIsStronglyAssociatedDpi;
};

} // namespace UxTheme
