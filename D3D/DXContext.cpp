

bool DXContext::Init() //初始化所有D3D12核心对象，包括设备、命令队列、围栏、事件、命令分配器和命令列表
{
    //创建设备
    if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device))))
    {
        return false;
    }

    //命令队列
    D3D12_COMMAND_QUEUE_DESC cmdQueueDesc;
    cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; //DIRECT type用来执行常规渲染
    cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
    cmdQueueDesc.NodeMask = 0; //默认设置，系统会自动推断主要的GPU
    cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    if (FAILED(m_device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_cmdQueue))))
    {
        return false;
    }

    //围栏
    if (FAILED(m_device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence))))
    {
        return false;
    }

    //事件
    m_fenceEvent = CreateEvent(nullptr, false, false, nullptr);
    if (!m_fenceEvent) {
        return false;
    }

    //命令分配器
    if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmdAllocator))))
    {
        return false;
    }

    //命令列表
    if (FAILED(m_device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_cmdList))))
    {
        return false;
    }

    return true;

}
void DXContext::Shutdown()//资源释放
{
    m_cmdAllocator.Release();
    m_cmdList.Release();
    if (m_fenceEvent)
    {
        CloseHandle(m_fenceEvent);//关闭事件句柄
    }

    m_cmdQueue.Release();
    m_device.Release();
}
void DXContext::SignalAndWait() //需要设置同步点是调用
{
    m_fenceValue++;//递增期望值
    m_cmdQueue->Signal(m_fence, m_fenceValue); // GPU 执行到此处时更新围栏

    if (SUCCEEDED(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent))) //注册事件
    {
        if (WaitForSingleObject(m_fenceEvent, 20000) != WAIT_OBJECT_0)//设定该线程睡眠（阻塞型），最大等待时间20s
        {
            std::exit(-1); //这里是如果WaitForSingleObject函数的返回值不为WAIT_OBJECT_0（指定对象的状态已发出信号），（超时）则退出程序。
        }
    }
    else {
        std::exit(-1); //退出程序
    }
}
ID3D12GraphicsCommandList7* DXContext::InitCommandList()//开启命令列表录制，返回命令列表指针
{

    m_cmdAllocator->Reset();
    m_cmdList->Reset(m_cmdAllocator, nullptr); //涉及到 ID3D12PiplineState，可设置初始管线状态，这里暂时不讨论。
    return m_cmdList;
}
void DXContext::ExecuteCommandList()
{
    if (SUCCEEDED(m_cmdList->Close()))//命令列表执行前需要关闭命令录制
    {
        ID3D12CommandList* lists[] = { m_cmdList };
        m_cmdQueue->ExecuteCommandLists(1, lists);
        SignalAndWait();
    }
}