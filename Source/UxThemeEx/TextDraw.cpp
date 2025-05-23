#include "TextDraw.h"

#include "RenderObj.h"
#include "ScalingUtil.h"
#include "Utils.h"
#include "UxThemeEx.h"
#include <vssym32.h>

namespace UxTheme
{

static HRESULT DrawTextWithGlow(HDC hdcMem, wchar_t const* pszText, unsigned cch,
                                RECT* prc, DWORD dwFlags, COLORREF crText,
                                COLORREF crGlow, unsigned nGlowRadius,
                                unsigned nGlowIntensity, BOOL fPreMultiply,
                                DTT_CALLBACK_PROC pfnDrawTextCallback, LPARAM lParam)
{
    return S_OK;
}

BOOL CTextDraw::KeyProperty(int iPropId)
{
    switch (iPropId) {
    case TMT_FONT:
    case TMT_HEADING1FONT:
    case TMT_HEADING2FONT:
    case TMT_BODYFONT:
    case TMT_TEXTAPPLYOVERLAY:
    case TMT_TEXTGLOW:
    case TMT_TEXTBORDERSIZE:
    case TMT_TEXTGLOWSIZE:
    case TMT_TEXTSHADOWOFFSET:
    case TMT_TEXTCOLOR:
    case TMT_EDGELIGHTCOLOR:
    case TMT_EDGEHIGHLIGHTCOLOR:
    case TMT_EDGESHADOWCOLOR:
    case TMT_EDGEDKSHADOWCOLOR:
    case TMT_EDGEFILLCOLOR:
    case TMT_TEXTBORDERCOLOR:
    case TMT_TEXTSHADOWCOLOR:
    case TMT_HEADING1TEXTCOLOR:
    case TMT_HEADING2TEXTCOLOR:
    case TMT_BODYTEXTCOLOR:
    case TMT_TEXTSHADOWTYPE:
        return TRUE;

    default:
        return FALSE;
    }
}

HRESULT CTextDraw::PackProperties(CRenderObj* pRender, int iPartId, int iStateId)
{
    memset(this, 0, sizeof(CTextDraw));

    _iSourcePartId = iPartId;
    _iSourceStateId = iStateId;

    // Color
    if (FAILED(pRender->ExternalGetColor(iPartId, iStateId, TMT_TEXTCOLOR, &_crText)))
        _crText = 0;

    // Shadow properties
    if (SUCCEEDED(pRender->ExternalGetPosition(iPartId, iStateId, TMT_TEXTSHADOWOFFSET, &_ptShadowOffset)))
    {
        if (FAILED(pRender->ExternalGetColor(iPartId, iStateId, TMT_TEXTSHADOWCOLOR, &_crShadow)))
            _crShadow = 0;
        if (FAILED(pRender->ExternalGetEnumValue(iPartId, iStateId, TMT_TEXTSHADOWTYPE, (int*)&_eShadowType)))
            _eShadowType = TST_NONE;
    }

    // Border properties
    if (FAILED(pRender->ExternalGetInt(iPartId, iStateId, TMT_TEXTBORDERSIZE, &_iBorderSize)))
    {
        _iBorderSize = 0;
    }
    else
    {
        if (FAILED(pRender->ExternalGetColor(iPartId, iStateId, TMT_TEXTBORDERCOLOR, &_crBorder)))
            _crBorder = 0;
        if (FAILED(pRender->ExternalGetBool(iPartId, iStateId, TMT_TEXTAPPLYOVERLAY, &_fApplyOverlay)))
            _fApplyOverlay = FALSE;
    }

    // Glow properties
    if (FAILED(pRender->ExternalGetInt(iPartId, iStateId, TMT_TEXTGLOWSIZE, &_iGlowSize)))
        _iGlowSize = 0;
    if (FAILED(pRender->ExternalGetInt(iPartId, iStateId, TMT_GLOWINTENSITY, &_iGlowIntensity)))
        _iGlowIntensity = 0;
    if (FAILED(pRender->ExternalGetColor(iPartId, iStateId, TMT_GLOWCOLOR, &_crGlow)))
        _crGlow = 0xFFFFFF;

    // Font properties
    pRender->GetFontTableIndex(iPartId, iStateId, TMT_FONT, &_iFontIndex);
    if (FAILED(pRender->ExternalGetBool(iPartId, iStateId, TMT_TEXTITALIC, &_fItalicFont)))
        _fItalicFont = FALSE;

    // Edge color properties
    if (FAILED(pRender->ExternalGetColor(iPartId, iStateId, TMT_EDGELIGHTCOLOR, &_crEdgeLight)))
        _crEdgeLight = 0xC0C0C0;
    if (FAILED(pRender->ExternalGetColor(iPartId, iStateId, TMT_EDGEHIGHLIGHTCOLOR, &_crEdgeHighlight)))
        _crEdgeHighlight = 0xFFFFFF;
    if (FAILED(pRender->ExternalGetColor(iPartId, iStateId, TMT_EDGESHADOWCOLOR, &_crEdgeShadow)))
        _crEdgeShadow = 0x808080;
    if (FAILED(pRender->ExternalGetColor(iPartId, iStateId, TMT_EDGEDKSHADOWCOLOR, &_crEdgeDkShadow)))
        _crEdgeDkShadow = 0;
    if (FAILED(pRender->ExternalGetColor(iPartId, iStateId, TMT_EDGEFILLCOLOR, &_crEdgeFill)))
        _crEdgeFill = _crEdgeLight;

    // Composition related properties
    if (FAILED(pRender->ExternalGetBool(iPartId, iStateId, TMT_COMPOSITED, &_fComposited)))
        _fComposited = FALSE;
    BOOL fGlow;
    if (!_fComposited && (FAILED(pRender->ExternalGetBool(iPartId, iStateId, TMT_TEXTGLOW, &fGlow)) || !fGlow))
        _iGlowSize = 0;

    return S_OK;
}

HRESULT CTextDraw::GetTextExtent(CRenderObj* pRender, HDC hdc, int iPartId, int iStateId,
                                 wchar_t const* _pszText, int iCharCount,
                                 unsigned dwTextFlags, RECT const* pBoundingRect,
                                 RECT* pExtentRect)
{
    HFONT font = nullptr;
    HFONT oldFont = nullptr;

    HRESULT hr = S_OK;
    if (_iFontIndex) {
        hr = pRender->GetScaledFontHandle(hdc, _iFontIndex, &font);
        if (hr >= 0)
            oldFont = (HFONT)SelectObject(hdc, font);
    }

    if (hr >= 0) {
        RECT rc = {};
        if (pBoundingRect)
            rc = *pBoundingRect;
        else
            SetRect(&rc, 0, 0, 0, 0);

        auto flags = dwTextFlags & 0xFFFEFFFF | DT_CALCRECT;
        if (!_fComposited) {
            if (!DrawTextExW(hdc, (LPWSTR)_pszText, iCharCount, &rc, flags, nullptr))
                hr = MakeErrorLast();
        } else {
            hr = DrawTextWithGlow(hdc, _pszText, iCharCount, &rc, flags, _crText, _crGlow,
                                  _iGlowSize, _iGlowIntensity, 1, nullptr, 0);
        }

        if (hr >= 0)
            *pExtentRect = rc;
    }

    if (oldFont)
        SelectObject(hdc, oldFont);
    if (font && !pRender->_fCacheEnabled)
        DeleteObject(font);

    return hr;
}

HRESULT CTextDraw::GetTextMetricsW(CRenderObj* pRender, HDC hdc, int iPartId,
                                   int iStateId, TEXTMETRICW* ptm)
{
    if (!ptm)
        return E_INVALIDARG;

    HRESULT hr = S_OK;
    HFONT oldFont = nullptr;
    HFONT font = nullptr;
    if (_iFontIndex) {
        hr = pRender->GetScaledFontHandle(hdc, _iFontIndex, &font);
        if (hr >= 0)
            oldFont = (HFONT)SelectObject(hdc, font);
    }

    if (hr >= 0) {
        if (!::GetTextMetricsW(hdc, ptm))
            hr = MakeErrorLast();
        if (oldFont)
            SelectObject(hdc, oldFont);
    }

    if (font && !pRender->_fCacheEnabled)
        DeleteObject(font);

    return hr;
}

HRESULT CTextDraw::DrawTextW(HTHEMEFILE hThemeFile, CRenderObj* pRender, HDC hdc,
                             int iPartId, int iStateId, wchar_t const* pszText,
                             unsigned dwCharCount, unsigned dwTextFlags, RECT* pRect,
                             DTTOPTS const* pOptions)
{
    HRESULT hr;
    RECT rc = {};
    LOGFONTW userFont = {};
    COLORREF oldTextColor = 0;
    HFONT hFont = nullptr;
    HFONT oldFont = nullptr;

    POINT shadowOffset = _ptShadowOffset;
    COLORREF crText = _crText;
    COLORREF crBorder = _crBorder;
    COLORREF crShadow = _crShadow;
    DWORD eShadowType = _eShadowType;
    int borderSize = _iBorderSize;
    int applyOverlay = _fApplyOverlay;
    unsigned glowSize = _iGlowSize;

    bool isComposited = false;
    bool hasChangedTextColor = false;
    bool hasFontProp = false;
    bool hasCallback = false;
    unsigned textFlags = dwTextFlags & ~DT_MODIFYSTRING;
    int oldBkMode = SetBkMode(hdc, TRANSPARENT);

    if (pOptions) {
        unsigned flags = pOptions->dwFlags;
        if (flags & DTT_TEXTCOLOR)
            crText = pOptions->crText;
        if (flags & DTT_BORDERCOLOR)
            crBorder = pOptions->crBorder;
        if (flags & DTT_SHADOWCOLOR)
            crShadow = pOptions->crShadow;
        if (flags & DTT_SHADOWTYPE)
            eShadowType = pOptions->iTextShadowType;
        if (flags & DTT_SHADOWOFFSET)
            shadowOffset = pOptions->ptShadowOffset;
        if (flags & DTT_BORDERSIZE)
            borderSize = pOptions->iBorderSize;
        if (flags & DTT_FONTPROP) {
            pRender->ExternalGetFont(nullptr, iPartId, iStateId, pOptions->iFontPropId,
                                     false, &userFont);
            hasFontProp = true;
        }
        if (flags & DTT_COLORPROP) {
            pRender->ExternalGetInt(iPartId, iStateId, pOptions->iColorPropId,
                                    reinterpret_cast<int*>(&crText));
        }
        if (flags & DTT_APPLYOVERLAY)
            applyOverlay = pOptions->fApplyOverlay;
        if (flags & DTT_GLOWSIZE)
            glowSize = pOptions->iGlowSize;
        if (flags & DTT_CALLBACK)
            hasCallback = true;
        if (flags & DTT_COMPOSITED)
            isComposited = true;
    }

    if (hasFontProp) {
        ScaleThemeFont(hdc, pRender, &userFont);
        hFont = CreateFontIndirectW(&userFont);
        oldFont = (HFONT)SelectObject(hdc, hFont);
    } else if (_iFontIndex != 0) {
        hr = pRender->GetScaledFontHandle(hdc, _iFontIndex, &hFont);
        if (hr < 0)
            goto done;

        oldFont = (HFONT)SelectObject(hdc, hFont);
    } else if (_fItalicFont) {
        LOGFONT lf;
        if (GetObjectW(oldFont, sizeof(lf), &lf) == sizeof(lf)) {
            lf.lfItalic = 1;
            if (pRender->GetCachedDisplayFontHandle(hdc, lf, &hFont) < 0) {
                hFont = CreateFontIndirectW(&lf);
                hasFontProp = true;
            }

            if (hFont)
                oldFont = (HFONT)SelectObject(hdc, hFont);
        }
    }

    if (_fComposited || isComposited) {
        if (textFlags & DT_CALCRECT) {
            isComposited = true;
        } else {
            auto hbmp = (HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP);
            DIBSECTION ds;
            isComposited = hbmp && GetObjectW(hbmp, sizeof(ds), &ds) &&
                           ds.dsBm.bmBitsPixel == 32 && ds.dsBm.bmBits != nullptr;
        }
    }

    if (isComposited) {
        DTT_CALLBACK_PROC const callback = hasCallback ? pOptions->pfnDrawTextCallback
                                                       : nullptr;
        LPARAM const callbackParam = hasCallback ? pOptions->lParam : 0;

        rc = *pRect;
        hr = DrawTextWithGlow(hdc, pszText, dwCharCount, &rc, textFlags, crText, _crGlow,
                              glowSize, _iGlowIntensity, TRUE, callback, callbackParam);

        if (hr >= 0)
            hr = 0;
    } else {
        if (glowSize) {
            HTHEME themeTextGlow = UxOpenThemeData(hThemeFile, nullptr, L"TEXTGLOW");
            if (themeTextGlow) {
                rc = *pRect;
                if (DrawTextExW(hdc, (LPWSTR)pszText, dwCharCount, &rc,
                                textFlags | DT_CALCRECT, nullptr)) {
                    long width = rc.right - rc.left;
                    long x;
                    if (textFlags & DT_RIGHT) {
                        x = pRect->right - width;
                    } else if (textFlags & DT_CENTER) {
                        x = pRect->left +
                            ((unsigned)(pRect->right - pRect->left - width) >> 1);
                    } else {
                        x = pRect->left;
                    }

                    long height = rc.bottom - rc.top;
                    long y;
                    if (textFlags & DT_BOTTOM) {
                        y = pRect->bottom - height;
                    } else if (textFlags & DT_VCENTER) {
                        y = pRect->top +
                            ((unsigned)(pRect->bottom - pRect->top - height) >> 1);
                    } else {
                        y = pRect->top;
                    }

                    rc.left = x;
                    rc.right = x + width;
                    rc.top = y;
                    rc.bottom = y + height;
                    InflateRect(&rc, glowSize, glowSize);
                    UxDrawThemeBackground(hThemeFile, themeTextGlow, hdc, 1, 0, &rc,
                                          nullptr);
                    UxCloseThemeData(hThemeFile, themeTextGlow);
                }
            }
        }

        switch (eShadowType) {
        case TST_SINGLE:
            oldTextColor = SetTextColor(hdc, crShadow);
            hasChangedTextColor = true;
            rc.left = pRect->left + shadowOffset.x;
            rc.top = pRect->top + shadowOffset.y;
            rc.right = pRect->right + shadowOffset.x;
            rc.bottom = pRect->bottom;
            if (!DrawTextExW(hdc, (LPWSTR)pszText, dwCharCount, &rc, textFlags,
                             nullptr)) {
                hr = MakeErrorLast();
                goto done;
            }

            [[fallthrough]];

        case TST_NONE: {
            rc = *pRect;
            if (!borderSize) {
                goto LABEL_16;
            }

            if (!BeginPath(hdc)) {
                hr = MakeErrorLast();
                goto done;
            }

            LOGBRUSH logBrush = {};
            logBrush.lbColor = crBorder;

            GdiPenHandle pen{
                ExtCreatePen(PS_GEOMETRIC, borderSize, &logBrush, 0, nullptr)};
            if (!pen) {
                AbortPath(hdc);
                hr = MakeErrorLast();
                goto done;
            }

            SelectObjectScope<HPEN> oldPen{hdc, pen};

            GdiBrushHandle brush{CreateSolidBrush(applyOverlay ? crBorder : crText)};
            if (brush) {
                SelectObjectScope<HBRUSH> oldBrush{hdc, brush};
                if (!DrawTextExW(hdc, (LPWSTR)pszText, dwCharCount, &rc, 0, nullptr)) {
                    AbortPath(hdc);
                    hr = MakeErrorLast();
                    goto done;
                }

                EndPath(hdc);
                StrokeAndFillPath(hdc);
            }
            break;
        }

        case TST_CONTINUOUS:
            rc = *pRect;
            DrawShadowText(hdc, pszText, dwCharCount, &rc, textFlags, crText, crShadow,
                           shadowOffset.x, shadowOffset.y);
            break;
        }

        if (applyOverlay) {
        LABEL_16:
            if (hasChangedTextColor) {
                SetTextColor(hdc, crText);
            } else {
                oldTextColor = SetTextColor(hdc, crText);
                hasChangedTextColor = true;
            }

            hr = 0;
            if (!DrawTextExW(hdc, (LPWSTR)pszText, dwCharCount, &rc, textFlags, nullptr))
                hr = MakeErrorLast();
        } else {
            hr = 0;
        }
    }

done:
    SetBkMode(hdc, oldBkMode);
    if (hasChangedTextColor)
        SetTextColor(hdc, oldTextColor);
    if (oldFont)
        SelectObject(hdc, oldFont);
    if (hasFontProp)
        pRender->ReturnFontHandle(hFont);

    if (FAILED(hr))
        return hr;

    if (pOptions && pOptions->dwFlags & DTT_CALCRECT && pRect)
        CopyRect(pRect, &rc);

    return hr;
}

} // namespace UxTheme
