#pragma once

#define NOMINMAX//避免使用Windows.h内的Min和Max宏，避免和Std冲突导致编译错误

#include <Windows.h>
#include <d3d12.h>
#include<dxgi1_6.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#include <d3d12sdklayers.h>
#endif
//测试