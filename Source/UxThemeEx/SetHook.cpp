#include "SetHook.h"
#include "Primitives.h"

namespace uxtheme
{
    enum THEMEHOOKSTATE
    {
        HS_INITIALIZED = 0,
        HS_UNHOOKING = 1,
        HS_UNINITIALIZED = 2,
    };

    struct UXTHEMEHOOKS
    {
        HINSTANCE hInst;
        USERAPIHOOK uahReal;
    };

    static THEMEHOOKSTATE g_eThemeHookState = HS_UNINITIALIZED;
    static UXTHEMEHOOKS hookinf = { nullptr };
    static CRITICAL_SECTION csSysMetCall = { nullptr };
    static BOOL fSysMetCall = FALSE;

    void ENTER_CLASSICSYSMETCALL()
    {
        if (g_eThemeHookState == HS_INITIALIZED && csSysMetCall.DebugInfo)
        {
            EnterCriticalSection(&csSysMetCall);
            fSysMetCall = TRUE;
        }
    }

    void LEAVE_CLASSICSYSMETCALL()
    {
        if (fSysMetCall)
        {
            fSysMetCall = FALSE;
            if (csSysMetCall.DebugInfo)
            {
                LeaveCriticalSection(&csSysMetCall);
            }
        }
    }

    int ClassicGetSystemMetrics(int iMetric)
    {
        if (g_eThemeHookState == HS_INITIALIZED && hookinf.uahReal.pfnGetSystemMetrics)
            return hookinf.uahReal.pfnGetSystemMetrics(iMetric);
        ENTER_CLASSICSYSMETCALL();
        int nRet = GetSystemMetrics(iMetric);
        LEAVE_CLASSICSYSMETCALL();
        return nRet;
    }
}
