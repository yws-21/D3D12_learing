#include "DXDebugLayer.h"

BOOL DXDebugLayer::Init()
{
#ifdef_DEBUG
	if(succeeded(D3D12GetDebugInterface(IID_PPV_ARGS(&m_d3d12Debug))))
	{
		m_d3d12Debug->EnableDebugLayer();
	}

#endif 
return false;
}

void DXDebugLayer::Shutdown()
{
	#ifdef_DEBUG


#endif 


}