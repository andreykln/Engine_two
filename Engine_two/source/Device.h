#pragma once
#include "comdef.h"
#include "Graphics.h"
#include <string>

struct DxException
{
    DxException() = default;
    DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber)
        : ErrorCode(hr), FunctionName(functionName), Filename(filename), LineNumber(lineNumber) {}

    std::wstring ToString() const
    {
        // Get the string description of the error code.
        _com_error err(ErrorCode);
        std::wstring msg = err.ErrorMessage();

        return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
    }

    HRESULT ErrorCode = S_OK;
    std::wstring FunctionName;
    std::wstring Filename;
    int LineNumber = -1;
};

inline std::wstring AnsiToWString(const std::string& str)
{
    WCHAR buffer[512];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
    return std::wstring(buffer);
}

#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}

class Device
{
public:
    void SetWindowParams(HWND handle, const int w, const int h);
    virtual ~Device(){};

public:

    //two in one because Debug needs to be enabled before creation of factory
    virtual bool DeviceInitialized() = 0;
    virtual void D3DInitialized() = 0;
	virtual void CreateDebugAndFactory() = 0;
    virtual void CreateCommandObjects() = 0;
    virtual void CreateSwapChain() = 0;
    virtual void CreateDescriptorHeap(DescriptorHeap type, UINT numDesc, DescriptorHeapFlag flag, std::string name) = 0;
    virtual void PrepareCommandList() = 0;
    virtual void CloseCommandList() = 0;
    virtual void FlushCommandQueue() = 0;
    virtual void ReleaseAndResizeSwapChain() = 0;
    virtual void CreateRenderTargetView() = 0;
    virtual void PrepareRenderTarget() = 0;
    virtual void PrepareRTVtoPresent() = 0;
    virtual void CreateDepthStencilView() = 0;
    virtual void SetDepthStencilView() = 0;
    virtual void UpdateViewport() = 0;
    virtual void SetViewportScissorRect() = 0;
    virtual void ClearRTVAndStencil() = 0;
    virtual void SetRenderTarget() = 0;
    virtual void Present() = 0;
    virtual void PrepareCommandListAndAllocator() = 0;

    void GetNewWindowSize(int w, int h);

    HWND hWnd;
    int mClientWidth;
    int mClientHeight;
};