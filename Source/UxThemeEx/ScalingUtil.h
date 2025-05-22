#pragma once
#include <uxtheme.h>
#include <windows.h>

namespace uxtheme
{
    class CRenderObj;

    void ScaleLongValues(long scale, long* numbers, int count);
    int ScaleThemeSize(HDC hdc, CRenderObj const* pRender, int iValue);
    void ScaleThemeFont(HDC hdc, CRenderObj const* pRender, LOGFONTW* plf);
    void ScaleFontForScreenDpi(LOGFONTW* plf);
    void ScaleMargins(MARGINS* pMargins, UINT targetDpi);
} // namespace uxtheme
