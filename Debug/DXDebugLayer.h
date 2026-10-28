#pragma once

#include<Support/WinInclude.h>
#include<Support/ComPointer.h>


class DXDebugLayer
{
public:
	bool Init();
	void Shutdown();

private:

#ifdef _DEBUG
	ComPointer<ID3D12Debug6> m_d3d12Debug;
	ComPointer<IDXGIDebug1> m_dxgiDebug;
#endif // 只能在_DEBUG下使用DebugLayer


	//单例类(Singelton) 只能有一个实例，禁止拷贝和赋值
public:
	DXDebugLayer(const DXDebugLayer&) = delete;
	DXDebugLayer& operator = (const DXDebugLayer&) = delete;

	inline static DXDebugLayer& Get()
	{
		static DXDebugLayer instance;
		return instance;
	}

private:
	DXDebugLayer() = default;

};