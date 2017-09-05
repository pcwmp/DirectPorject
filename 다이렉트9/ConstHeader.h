#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <string>

#include <d3d9.h>
#include <d3dx9.h>

#include <fstream>

struct UV
{
	float u;
	float v;
};

struct MYINDEX
{
	int	index[3];		/// 일반적으로 인덱스는 16비트의 크기를 갖는다.
	int subMaterialRef_;
	UV texUV[3];
	D3DXVECTOR3 _faceNormal;
	D3DXVECTOR3 _vertexNormal[3];

	int vertexBufferIdx[3];
};

struct TFACEINDEX
{
	int	_0, _1, _2;		/// 일반적으로 인덱스는 16비트의 크기를 갖는다.
};

struct INDEX
{
	WORD i[3];
};

struct RigidVertex
{
	enum { FVF = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1) };
	D3DXVECTOR3	p;
	D3DXVECTOR3	n;
	D3DXVECTOR2	t;
};

struct SkinnedVertex
{
	enum { FVF = (D3DFVF_XYZB4 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1) };
	D3DXVECTOR3	p;
	float		b[3];
	DWORD		i;
	D3DXVECTOR3	n;
	D3DXVECTOR2	t;
};

struct ZSWSkinnedVertex
{
	D3DXVECTOR3	p;
	float		b[4];
	DWORD		i[4];
};

struct BOXVERTEX
{
	enum { FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE };

	float x, y, z;	/// 정점의 변환된 좌표
	DWORD color;	/// 정점의 색깔
	BOXVERTEX() { x = 0; y = 0; z = 0; color = 0; }
	BOXVERTEX( float f1, float f2, float f3, DWORD c ) { x = f1; y = f2; z = f3; color = c; }
};

