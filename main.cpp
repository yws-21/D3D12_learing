#include <iostream>

#include <Support/WinInclude.h>
#include <Support/ComPointer.h>
#include <Support/DXWindow.h>

#include <Debug/DXDebugLayer.h>

#include <D3D/DXContext.h>

int main()
{
	DXDebugLayer::Get().Init(); //调试层初始化
	if (DXContext)::Get().Init()) //初始化D3D12核心对象
	{
		while (true)
		{
			auto* cmdList = DXContext::Get().InitCommandList(); //初始化命令列表

			//先完成设置-->再绘制



			DXContext::Get().ExecuteCommandList(); //提交命令列表并等待GPU执行完成
			//-->显示结果
		}
		DXContext::Get().Shutdown(); //释放D3D12核心对象
	}
	else
	{
		
	}

	DXDebugLayer::Get().Shutdown();
