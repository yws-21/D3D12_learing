#include <iostream>

#include <Support/WinInclude.h>
#include <Support/ComPointer.h>
#include <Support/DXWindow.h>

#include <Debug/DXDebugLayer.h>

#include <D3D/DXContext.h>

int main()
{

	DXDebugLayer::Get().Init(); //调试层初始化

	if (DXContext::Get().Init() && DXWindow::Get().Init()) { //DXContext和窗口初始化

		while (!DXWindow::Get().ShouldClose())
		{
			DXWindow::Get().Update();
			auto* cmdList = DXContext::Get().InitCommandList(); //初始化命令列表并储存

			DXContext::Get().ExecuteCommandList(); //提交执行并等待
		}

		DXWindow::Get().Shutdown();
		DXContext::Get().Shutdown();
	}

	DXDebugLayer::Get().Shutdown();
}