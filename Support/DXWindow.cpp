#include "DXWindow.h"

bool DXWindow::Init()
{
    // Window class 窗口类描述结构
    WNDCLASSEXW wcex{};
    wcex.cbSize = sizeof(wcex); //结构的大小
	wcex.style = CS_OWNDC; //窗口样式：用一组位标识符来指定窗口的样式，CS_OWNDC表示为每个窗口分配一个私有的设备上下文
    wcex.lpfnWndProc = &DXWindow::OnWindowMessage;//注册窗口过程
    wcex.cbClsExtra = 0;//
    wcex.cbWndExtra = 0;//额外预留
	wcex.hInstance = GetModuleHandleW(nullptr);//注册这个窗口类的应用程序实例 传回句柄
	wcex.hIcon = LoadIconW(nullptr, IDI_APPLICATION);//指定图标：（指定图标资源所在的模块实例句柄，IDI_APPLICATION表示使用默认应用程序图标）
    wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);//鼠标指针：类似同上
    wcex.hbrBackground = nullptr;//背景画刷：定义背景颜色
    wcex.lpszMenuName = nullptr;//菜单名称（暂不需要
    wcex.lpszClassName = L"D3D12ExWndCls";//指定窗口类名
    wcex.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);//小图标
   
    m_wndClass = RegisterClassExW(&wcex);//注册函数
    if (m_wndClass == 0)
    {
        return false;
    }

    // Place window on current screen
    POINT pos{ 0,0 };
    GetCursorPos(&pos);
    HMONITOR monitor = MonitorFromPoint(pos, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfoW(monitor, &monitorInfo);

    // Window
    m_window = CreateWindowExW(    //创造实际窗口
		WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW,
        //扩展（额外）窗口样式：WS_EX_OVERLAPPEDWINDOW表示创建一个重叠窗口，WS_EX_APPWINDOW表示创建一个应用程序窗口
        (LPCWSTR)m_wndClass, //指定窗口类
        L"D3D12Ez",//名字
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, //窗口样式：WS_OVERLAPPEDWINDOW也表示创建一个重叠窗口，WS_VISIBLE表示窗口直接可见
		monitorInfo.rcWork.left + 100,   //窗口位置：坐标
		monitorInfo.rcWork.top + 100, //同上
        1920,//大小尺寸
        1080,
		nullptr,//父窗口句柄，若为顶级窗口则为NULL
        nullptr,//菜单句柄
        wcex.hInstance,//模块实例句柄
        nullptr//用户自定义参数
    );
    if (m_window == nullptr)
        return false;

    // Describe swap chain
    DXGI_SWAP_CHAIN_DESC1 swd{};
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC sfd{};

    swd.Width = 1920;  
    swd.Height = 1080;
    swd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swd.Stereo = false;
    swd.SampleDesc.Count = 1;
    swd.SampleDesc.Quality = 0;
    swd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swd.BufferCount = GetFrameCount();
    swd.Scaling = DXGI_SCALING_STRETCH;
    swd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    swd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    sfd.Windowed = true;

    // Create Swap chain
    auto& factory = DXContext::Get().GetFactory();
    ComPointer<IDXGISwapChain1> sc1;
    factory->CreateSwapChainForHwnd(DXContext::Get().GetCommandQueue(), m_window, &swd, &sfd, nullptr, &sc1);
    if (!sc1.QueryInterface(m_swapChain))
    {
        return false;
    }

    // Create RTV Heap
    D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
    descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    descHeapDesc.NumDescriptors = FrameCount;
    descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    descHeapDesc.NodeMask = 0;
    if (FAILED(DXContext::Get().GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&m_rtvDescHeap))))
    {
        return false;
    }

    // Create handles to view
    auto firstHandle = m_rtvDescHeap->GetCPUDescriptorHandleForHeapStart();
    auto handleIncrement = DXContext::Get().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    for (size_t i = 0; i < FrameCount; ++i)
    {
        m_rtvHandles[i] = firstHandle;
        m_rtvHandles[i].ptr += handleIncrement * i;
    }

    // Get buffers
    if (!GetBuffers())
    {
        return false;
    }

    return true;
}

void DXWindow::Update()
{
    MSG msg;
    while (PeekMessageW(&msg, m_window, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void DXWindow::Preset()
{
    m_swapChain->Present(1, 0);
}

void DXWindow::Shutdown()
{
    ReleaseBuffers();

    m_rtvDescHeap.Release();

    m_swapChain.Release();

    if (m_window)
    {
        DestroyWindow(m_window);
    }

    if (m_wndClass)//注销窗口
    {
        UnregisterClassW((LPCWSTR)m_wndClass, GetModuleHandleW(nullptr));
    }
}

void DXWindow::Resize()
{
    ReleaseBuffers();

    RECT cr;
    if (GetClientRect(m_window, &cr))
    {
        m_width = cr.right - cr.left;
        m_height = cr.bottom - cr.top;

        m_swapChain->ResizeBuffers(GetFrameCount(), m_width, m_height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
        m_shouldResize = false;
    }

    GetBuffers();
}

void DXWindow::SetFullscreen(bool enabled)
{
    // Update window styling
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    DWORD exStyle = WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW;
    if (enabled)
    {
        style = WS_POPUP | WS_VISIBLE;
        exStyle = WS_EX_APPWINDOW;
    }
    SetWindowLongW(m_window, GWL_STYLE, style);
    SetWindowLongW(m_window, GWL_EXSTYLE, exStyle);

    // Adjust window size
    if (enabled)
    {
        HMONITOR monitor = MonitorFromWindow(m_window, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo{};
        monitorInfo.cbSize = sizeof(monitorInfo);
        if (GetMonitorInfoW(monitor, &monitorInfo))
        {
            SetWindowPos(m_window, nullptr,
                monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.top,
                monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                SWP_NOZORDER
            );
        }
    }
    else
    {
        ShowWindow(m_window, SW_MAXIMIZE);
    }

    m_isFullscreen = enabled;
}

void DXWindow::BeginFrame(ID3D12GraphicsCommandList7* cmdlist)
{
    m_currentBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

    D3D12_RESOURCE_BARRIER barr;
    barr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barr.Transition.pResource = m_buffers[m_currentBufferIndex];
    barr.Transition.Subresource = 0;
    barr.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barr.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

    cmdlist->ResourceBarrier(1, &barr);

    float clearColor[] = { .4f, .4f, .8f, 1.f };
    cmdlist->ClearRenderTargetView(m_rtvHandles[m_currentBufferIndex], clearColor, 0, nullptr);

    cmdlist->OMSetRenderTargets(1, &m_rtvHandles[m_currentBufferIndex], false, nullptr);
}

void DXWindow::EndFrame(ID3D12GraphicsCommandList7* cmdlist)
{
    D3D12_RESOURCE_BARRIER barr;
    barr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barr.Transition.pResource = m_buffers[m_currentBufferIndex];
    barr.Transition.Subresource = 0;
    barr.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barr.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

    cmdlist->ResourceBarrier(1, &barr);
}

bool DXWindow::GetBuffers()
{
    for (size_t i = 0; i < FrameCount; ++i)
    {
        if (FAILED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_buffers[i]))))
        {
            return false;
        }

        D3D12_RENDER_TARGET_VIEW_DESC rtv{};
        rtv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        rtv.Texture2D.MipSlice = 0;
        rtv.Texture2D.PlaneSlice = 0;
        DXContext::Get().GetDevice()->CreateRenderTargetView(m_buffers[i], &rtv, m_rtvHandles[i]);
    }

    return true;
}

void DXWindow::ReleaseBuffers()
{
    for (size_t i = 0; i < FrameCount; ++i)
    {
        m_buffers[i].Release();
    }
}

LRESULT CALLBACK DXWindow::OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
// 回调函数：负责处理窗口消息：窗口句柄/消息编号/消息（附加）参数1/消息参数2（有信息的时候回调窗口过程）
{
    switch (msg)
    {
    case WM_KEYDOWN:
        if (wParam == VK_F11)//按F11
        {
            Get().SetFullscreen(!Get().IsFullscreen()); //切换全屏
        }
        break;
    case WM_SIZE://窗口尺寸变化
        if (lParam && (HIWORD(lParam) != Get().m_height || LOWORD(lParam) != Get().m_width))
        {
            Get().m_shouldResize = true;
        }
        break;
    case WM_CLOSE://关闭请求
        Get().m_shouldClose = true;
        return 0;
    }
    return DefWindowProcW(wnd, msg, wParam, lParam);//默认处理
}