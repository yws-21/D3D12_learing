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
		

		DXContext::Get().Shutdown(); //释放D3D12核心对象
	}
	else
	{
		
	}

	DXDebugLayer::Get().Shutdown();
