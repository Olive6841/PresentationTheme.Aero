#include "ScalingUtil.h"

#include "DpiInfo.h"
#include "RenderObj.h"

namespace uxtheme
{
    void ScaleLongValues(long scale, long* numbers, int count)
    {
        for (long i = 0; i < count; ++i)
            numbers[i] *= scale;
    }

    int ScaleThemeSize(HDC hdc, CRenderObj const* pRender, int iValue)
    {
        int iAssociatedDpi;
        if (hdc)
        {
            if (pRender->IsStronglyAssociatedDpi() && (UINT)IsScreenDC(hdc))
                iAssociatedDpi = pRender->GetAssociatedDpi();
            else
                iAssociatedDpi = GetDeviceCaps(hdc, LOGPIXELSX);
        }
        else
        {
            iAssociatedDpi = pRender->GetAssociatedDpi();
        }
        return MulDiv(iValue, iAssociatedDpi, 96);
    }

    void ScaleThemeFont(HDC hdc, CRenderObj const* pRender, LOGFONTW* plf)
    {
        if (plf->lfHeight < 0)
        {
            int iAssociatedDpi;
            if (hdc)
            {
                if (pRender->IsStronglyAssociatedDpi() && (UINT)IsScreenDC(hdc))
                    iAssociatedDpi = pRender->GetAssociatedDpi();
                else
                    iAssociatedDpi = GetDeviceCaps(hdc, LOGPIXELSX);
            }
            else
            {
                iAssociatedDpi = pRender->GetAssociatedDpi();
            }
            plf->lfHeight = MulDiv(plf->lfHeight, iAssociatedDpi, 96);
        }
    }

    void ScaleFontForScreenDpi(LOGFONTW* plf)
    {
        if (plf->lfHeight < 0)
            plf->lfHeight = MulDiv(plf->lfHeight, GetScreenDpi(), 96);
    }

    void ScaleMargins(MARGINS* pMargins, UINT targetDpi)
    {
        if (pMargins)
        {
            pMargins->cxLeftWidth = MulDiv(pMargins->cxLeftWidth, targetDpi, 96);
            pMargins->cxRightWidth = MulDiv(pMargins->cxRightWidth, targetDpi, 96);
            pMargins->cyTopHeight = MulDiv(pMargins->cyTopHeight, targetDpi, 96);
            pMargins->cyBottomHeight = MulDiv(pMargins->cyBottomHeight, targetDpi, 96);
        }
    }
} // namespace uxtheme
