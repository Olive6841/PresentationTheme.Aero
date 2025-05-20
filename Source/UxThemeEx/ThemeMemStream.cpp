#include "ThemeMemStream.h"
#include "Utils.h"

namespace uxtheme
{
    CThemeMemStream::CThemeMemStream(size_t cbInitialSize, int)
        : m_fRead(FALSE)
        , m_cbInitialSize(0)
        , m_RefCnt(1)
    {
        Clear(FALSE);
    }

    HRESULT CThemeMemStream::QueryInterface(REFIID riid, _COM_Outptr_ void** ppvObject)
    {
        if (!ppvObject)
            return E_POINTER;

        if (riid == IID_IUnknown)
            *ppvObject = static_cast<IUnknown*>(this);
        else if (riid == IID_ISequentialStream)
            *ppvObject = static_cast<ISequentialStream*>(this);
        else if (riid == IID_IStream)
            *ppvObject = static_cast<IStream*>(this);
        else
        {
            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }

        if (*ppvObject)
            AddRef();
        return S_OK;
    }

    ULONG CThemeMemStream::AddRef()
    {
        return m_RefCnt += 1;
    }

    ULONG CThemeMemStream::Release()
    {
        return m_RefCnt -= 1;
    }

    HRESULT CThemeMemStream::Read(_Out_writes_bytes_to_(cb, *pcbRead) void* pv, _In_ ULONG cb,
                                  _Out_opt_ ULONG* pcbRead)
    {
        if (m_uCurr >= m_uSize)
        {
            *pcbRead = 0;
            return E_FAIL;
        }

        if (m_uCurr + cb > m_uSize)
            cb = m_uSize - m_uCurr;

        if (cb)
        {
            memcpy(pv, &m_lpb[m_uCurr], cb);
            this->m_uCurr += cb;
        }

        *pcbRead = cb;
        return cb == 0 ? E_FAIL : S_OK;
    }

    HRESULT CThemeMemStream::Write(_In_reads_bytes_(cb) void const* pv, _In_ ULONG cb,
                                   _Out_opt_ ULONG* pcbWritten)
    {
        if (!pv)
            return E_INVALIDARG;

        size_t cbWritten = cb;
        if (cb)
        {
            uint64_t newSize = m_cbInitialSize;
            if (m_uCurr + cb > newSize)
                newSize = m_uCurr + cb;

            if (newSize > m_cbMax)
            {
                size_t v11 = ((newSize / 8192) + 1) * 8192;
                auto newBuf = make_unique_nothrow<uint8_t[]>(v11);
                if (!newBuf)
                    return E_OUTOFMEMORY;
                if (m_lpb)
                {
                    memcpy(newBuf.get(), m_lpb.get(), m_cbMax);
                    m_lpb.reset();
                }

                m_cbMax = v11;
                m_lpb = std::move(newBuf);
            }

            if (!m_lpb && !m_uCurr || m_lpb.get() + m_uCurr < m_lpb.get() ||
                (uint64_t)(m_lpb.get() + m_uCurr) < m_uCurr)
                return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);

            memcpy(&m_lpb[m_uCurr], pv, cbWritten);

            m_uCurr += cbWritten;
            if (m_uCurr > m_uSize)
                m_uSize = m_uCurr;
        }

        if (pcbWritten)
            *pcbWritten = cbWritten;

        return S_OK;
    }

    HRESULT CThemeMemStream::Seek(LARGE_INTEGER liMove, DWORD dwOrigin,
                                  _Out_opt_ ULARGE_INTEGER* pliNewPos)
    {
        if (dwOrigin == 0)
            m_uCurr = liMove.LowPart;
        else if (dwOrigin == 1)
            m_uCurr = m_uCurr + liMove.LowPart;
        else if (dwOrigin == 2)
            m_uCurr = m_uSize - liMove.LowPart;
        else
            m_uCurr = 0;

        if (pliNewPos)
            pliNewPos->QuadPart = m_uCurr;
        return S_OK;
    }

    HRESULT CThemeMemStream::SetSize(ULARGE_INTEGER uli)
    {
        m_uSize = m_cbInitialSize;
        if (uli.QuadPart > m_uSize)
            m_uSize = uli.QuadPart;

        if (m_uCurr > m_uSize)
            m_uCurr = m_uSize;

        if (m_uSize > m_cbMax)
        {
            auto newBuf = make_unique_nothrow<uint8_t[]>(m_uSize + 0x2000);
            if (m_lpb)
            {
                memcpy(newBuf.get(), m_lpb.get(), m_cbMax);
                m_lpb.reset();
            }

            m_lpb = std::move(newBuf);
            m_cbMax = m_uSize + 0x2000;
        }

        return S_OK;
    }

    HRESULT CThemeMemStream::CopyTo(_In_ IStream* pstm, ULARGE_INTEGER cb,
                                    _Out_opt_ ULARGE_INTEGER* pcbRead,
                                    _Out_opt_ ULARGE_INTEGER* pcbWritten)
    {
        return STG_E_UNIMPLEMENTEDFUNCTION;
    }

    HRESULT CThemeMemStream::Commit(DWORD grfCommitFlags)
    {
        return S_OK;
    }

    HRESULT CThemeMemStream::Revert()
    {
        return STG_E_UNIMPLEMENTEDFUNCTION;
    }

    HRESULT CThemeMemStream::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb,
                                        DWORD dwLockType)
    {
        return STG_E_UNIMPLEMENTEDFUNCTION;
    }

    HRESULT CThemeMemStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb,
                                          DWORD dwLockType)
    {
        return STG_E_UNIMPLEMENTEDFUNCTION;
    }

    HRESULT CThemeMemStream::Stat(_Out_ STATSTG* pStg, DWORD dwStatFlag)
    {
        memset(pStg, 0, sizeof(STATSTG));
        pStg->type = STGTY_STREAM;
        pStg->pwcsName = (LPWSTR)L""; // (wchar_t *)&pszAppName;
        pStg->cbSize.QuadPart = m_uSize;
        SYSTEMTIME st;
        GetSystemTime(&st);
        SystemTimeToFileTime(&st, &pStg->mtime);
        pStg->ctime = pStg->mtime;
        pStg->atime = pStg->mtime;
        pStg->grfMode = m_fRead == FALSE;
        return S_OK;
    }

    HRESULT CThemeMemStream::Clone(_COM_Outptr_opt_ IStream** ppstm)
    {
        return STG_E_UNIMPLEMENTEDFUNCTION;
    }

    uint8_t* CThemeMemStream::GetBuffer(uint64_t* cbSize)
    {
        if (cbSize)
            *cbSize = m_uSize;
        return m_lpb.get();
    }

    void CThemeMemStream::Clear(BOOL fFree)
    {
        if (fFree && !m_fRead)
            m_lpb.reset();

        m_lpb = nullptr;
        m_uCurr = 0;
        m_uSize = 0;
        m_cbMax = 0;
    }

    HRESULT CThemeMemStream::SetMaxSize(uint64_t cbSize)
    {
        if (cbSize < m_uSize)
        {
            m_uCurr = 0;
            return S_OK;
        }

        Clear(TRUE);
        ULARGE_INTEGER size;
        size.QuadPart = cbSize;
        return SetSize(size);
    }
} // namespace uxtheme
