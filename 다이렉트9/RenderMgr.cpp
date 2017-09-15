#include "RenderMgr.h"
#include "Device.h"

CRenderMgr g_renderMgr;


CRenderMgr::CRenderMgr()
{

}

CRenderMgr::~CRenderMgr()
{

}

HRESULT CRenderMgr::SetIndices(IDirect3DIndexBuffer9* pindexBuffer)
{
	return g_Device.pd3dDevice_->SetIndices(pindexBuffer);
}

void CRenderMgr::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9 *pStreamData, UINT OffsetInBytes, UINT Stride)
{
	g_Device.pd3dDevice_->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
}

void CRenderMgr::SetFVF(DWORD fvf)
{
	g_Device.pd3dDevice_->SetFVF(fvf);
}

void CRenderMgr::SetTexture(DWORD sampler, IDirect3DBaseTexture9* pTexture)
{
	g_Device.pd3dDevice_->SetTexture(sampler, pTexture);
}

HRESULT CRenderMgr::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, 
										 UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount)
{
	return g_Device.pd3dDevice_->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinIndex, 
		NumVertices, StartIndex, PrimitiveCount);
}