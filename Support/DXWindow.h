#pragma once

#include <Support/WinInclude.h>
#include <Support/ComPointer.h>
#include <D3D/DXContext.h>

class DXWindow
{
public:
    bool Init();
    void Update();
    void Preset();
    void Shutdown();
    void Resize();
    void SetFullscreen(bool enabled);

    void BeginFrame(ID3D12GraphicsCommandList7* cmdlist);
    void EndFrame(ID3D12GraphicsCommandList7* cmdlist);

    inline bool ShouldClose() const
    {
        return m_shouldClose;
    }

    inline bool ShouldResize() const
    {
        return m_shouldResize;
    }

    inline bool IsFullscreen() const
    {
        return m_isFullscreen;
    }

    inline UINT GetWidth() const
    {
        return m_width;
    }

    inline UINT GetHeight() const
    {
        return m_height;
    }

    static constexpr size_t FrameCount = 2;
    static constexpr size_t GetFrameCount()
    {
        return FrameCount;
    }

private:
    bool GetBuffers();
    void ReleaseBuffers();

    static LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
	//负责处理窗口消息：窗口句柄/消息编号/消息（附加）参数1/消息参数2

private:
    ATOM m_wndClass = 0;
    HWND m_window = nullptr;
    bool m_shouldClose = false;

    bool m_shouldResize = false;
    UINT m_width = 1920;
    UINT m_height = 1080;

    bool m_isFullscreen = false;

    ComPointer<IDXGISwapChain3> m_swapChain;
    ComPointer<ID3D12Resource2> m_buffers[FrameCount];
    size_t m_currentBufferIndex = 0;

    ComPointer<ID3D12DescriptorHeap> m_rtvDescHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandles[FrameCount];

	// 单例类(Singelton) 只能有一个实例，禁止拷贝和赋值   
public:
    DXWindow(const DXWindow&) = delete;
    DXWindow& operator=(const DXWindow&) = delete;

    inline static DXWindow& Get()
    {
        static DXWindow instance;
        return instance;
    }
private:
    DXWindow() = default;
};