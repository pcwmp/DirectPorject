#pragma once

#include "ConstHeader.h"

class CRenderMgr
{

public:
	CRenderMgr();
	~CRenderMgr();

public:
	HRESULT SetIndices(IDirect3DIndexBuffer9* pindexBuffer);
	void SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9 *pStreamData, UINT OffsetInBytes, UINT Stride);
	void SetFVF(DWORD fvf);
	void SetTexture(DWORD sampler, IDirect3DBaseTexture9* pTexture);

	HRESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, 
		UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount);
};

extern CRenderMgr g_renderMgr;