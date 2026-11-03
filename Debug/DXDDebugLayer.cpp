#include "DXDebugLayer.h"

bool DXDebugLayer::Init()
{
#ifdef _DEBUG//只在Debug模式下启用Debug Layer
    //初始化D3D12 调试接口
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_d3d12Debug))))//如果成功获取到调试接口
		//D3D12GetDebugInterface:Windows API，用于获取 D3D12 调试接口对象
    {
        m_d3d12Debug->EnableDebugLayer();//启用调试层

        //初始化 DXGI 调试接口
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_dxgiDebug))))
			//DXGIGetDebugInterface1:Windows API，用于获取 DXGI 调试接口对象
        {
            m_dxgiDebug->EnableLeakTrackingForThread();
            return true;
        }
    }
#endif // _DEBUG

    return false;  // Debug 模式下初始化失败，或 Release 模式下直接返回 false
}

void DXDebugLayer::Shutdown()
{
#ifdef _DEBUG
    if (m_dxgiDebug)
    {
        OutputDebugStringW(L"DXGI Report live object : \n");
        m_dxgiDebug->ReportLiveObjects(
            DXGI_DEBUG_ALL,
            DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL)
        );
        /*
        DXGI_DEBUG_ALL	报告所有 DXGI 组件（D3D12、D3D11、DXGI 本身等）的存活对象
        DXGI_DEBUG_RLO_DETAIL	输出详细信息，包括对象名称、引用计数等
        DXGI_DEBUG_RLO_IGNORE_INTERNAL	忽略 DXGI 内部持有的对象，避免误报，只显示用户代码造成的泄漏
         */
    }

    m_d3d12Debug.Release(); // 释放 D3D12 调试接口的 COM 引用计数
    m_dxgiDebug.Release(); // 释放 DXGI 调试接口的 COM 引用计数
#endif // _DEBUG

}