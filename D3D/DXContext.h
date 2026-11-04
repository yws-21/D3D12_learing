#pragma once

#include<Support/WinInclude.h>
#include<Support/ComPointer.h>

//负责管理DirectX 12的设备、命令队列、命令分配器、命令列表和同步机制（围栏和事件）。
// 提供了初始化和关闭 DirectX 12 上下文的功能，并允许用户获取设备和命令队列的引用。该类还实现了单例模式，确保在整个应用程序中只有一个 DXContext 实例。

class DXContext
{
public:
	bool Init();
	void Shutdown();

	void SignalAndWait();//同步机制

	ID3D12GraphicsCommandList7* InitCommandList();//初始化命令列表
	void ExecuteCommandList(); //提交执行并等待

	inline ComPointer<ID3D12Device10>& GetDevice() 
	{
		return m_device;
	}

	inline ComPointer<ID3D12CommandQueue>& GetCommandQueue()
	{
		return m_cmdQueue;
	}

private:
	ComPointer<ID3D12Device10> m_device; //设备
	ComPointer<ID3D12CommandQueue> m_cmdQueue; //命令队列

	ComPointer<ID3D12CommandAllocator> m_cmdAllocator; //命令分配器，实质上是一块内存区域，是为命令列表提供的所有命令提供内存支持。
	ComPointer<ID3D12GraphicsCommandList7> m_cmdList; //命令列表

	ComPointer<ID3D12Fence1> m_fence; // 围栏对象（Fence）
	// 一个可被 CPU 和 GPU 共同读写的计数器
	// GPU 执行到 Signal 指令时，会将围栏值更新为指定值

	UINT64 m_fenceValue = 0; // 围栏值（单调递增）
    // CPU 端维护的计数器，每次同步时递增
    // 通过比较 GPU 当前围栏值与此值，判断 GPU 是否追上了 CPU

	HANDLE m_fenceEvent = nullptr; // 围栏事件句柄（Windows 内核事件对象）
    // 当 GPU 完成命令并更新围栏值后，触发此事件
    // CPU 调用 WaitForSingleObject() 阻塞在此，被唤醒后继续执行
    // 避免了 CPU 忙等待（busy-wait）浪费资源



	//单例类(Singelton) 
public:
	DXContext(const DXContext&) = delete;
	DXContext& operator = (const DXContext&) = delete;

	inline static DXContext& Get()
	{
		static DXContext instance;
		return instance;
	}

private:
	DXContext() = default;
};
