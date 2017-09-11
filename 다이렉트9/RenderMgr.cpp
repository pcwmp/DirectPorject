#include "RenderMgr.h"
#include "Device.h"

CRenderMgr g_renderMgr;


CRenderMgr::CRenderMgr()
{

}

CRenderMgr::~CRenderMgr()
{

}

HRESULT CRenderMgr::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, 
										 UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount)
{
	return g_Device.pd3dDevice_->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinIndex, 
		NumVertices, StartIndex, PrimitiveCount);
}