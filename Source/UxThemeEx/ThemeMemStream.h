#pragma once
#include <cstdint>
#include <memory>
#include <objidl.h>

namespace UxTheme
{
    class CThemeMemStream : public IStream
    {
    public:
        CThemeMemStream(SIZE_T cbInitialSize, int);
        virtual ~CThemeMemStream() = default;

        void Clear(BOOL fFree);
        HRESULT SetMaxSize(uint64_t cbSize);
        uint8_t* GetBuffer(uint64_t* cbSize);

        // *** IUnknown ***
        STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) override;
        STDMETHODIMP_(ULONG) AddRef() override;
        STDMETHODIMP_(ULONG) Release() override;

        // *** ISequentialStream ***
        STDMETHODIMP Read(void* pv, ULONG cb, ULONG* pcbRead) override;
        STDMETHODIMP Write(void const* pv, ULONG cb, ULONG* pcbWritten) override;

        // *** IStream ***
        STDMETHODIMP Seek(LARGE_INTEGER liMove, DWORD dwOrigin, ULARGE_INTEGER* pliNewPos) override;
        STDMETHODIMP SetSize(ULARGE_INTEGER uli) override;
        STDMETHODIMP CopyTo(IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten) override;
        STDMETHODIMP Commit(DWORD grfCommitFlags) override;
        STDMETHODIMP Revert() override;
        STDMETHODIMP LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) override;
        STDMETHODIMP UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) override;
        STDMETHODIMP Stat(STATSTG* pStg, DWORD dwStatFlag) override;
        STDMETHODIMP Clone(IStream** ppstm) override;

    private:
        BOOL m_fRead;
        std::unique_ptr<uint8_t[]> m_lpb;
        SIZE_T m_uCurr;
        SIZE_T m_uSize;
        SIZE_T m_cbMax;
        SIZE_T m_cbInitialSize;
        UINT m_RefCnt;
    };
} // namespace UxTheme
