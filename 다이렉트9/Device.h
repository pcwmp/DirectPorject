#pragma once

#include "ConstHeader.h"

class Device
{
public:
	LPDIRECT3D9         pD3D_; /// D3D 디바이스를 생성할 D3D객체변수
	LPDIRECT3DDEVICE9   pd3dDevice_; /// 렌더링에 사용될 D3D디바이스
public:
	Device();
	HRESULT InitD3D( HWND hWnd );
};


extern Device g_Device;