#pragma once

#include "ConstHeader.h"

class CRenderMgr
{

public:
	CRenderMgr();
	~CRenderMgr();

public:

	HRESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, 
		UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount);
};

extern CRenderMgr g_renderMgr;