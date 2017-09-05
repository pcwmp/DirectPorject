#pragma once

#include "ConstHeader.h"

class Device
{
public:
	LPDIRECT3D9         pD3D_; /// D3D ����̽��� ������ D3D��ü����
	LPDIRECT3DDEVICE9   pd3dDevice_; /// �������� ���� D3D����̽�
public:
	Device();
	HRESULT InitD3D( HWND hWnd );
};


extern Device g_Device;