#include "Device.h"

Device g_Device;

Device::Device():
pD3D_(NULL),
pd3dDevice_(NULL)
{

}

HRESULT Device::InitD3D( HWND hWnd )
{
	 /// 디바이스를 생성하기위한 D3D객체 생성
    if( NULL == ( pD3D_ = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    /// 디바이스를 생성할 구조체
    /// 복잡한 오브젝트를 그릴것이기때문에, 이번에는 Z버퍼가 필요하다.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    /// 디바이스 생성
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &pd3dDevice_ ) ) )
    {
        return E_FAIL;
    }

    /// 컬링기능을 끈다.
    pd3dDevice_->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

    /// Z버퍼기능을 켠다.
    pd3dDevice_->SetRenderState( D3DRS_ZENABLE, TRUE );

    /// 정점에 색깔값이 있으므로, 광원기능을 끈다.
    pd3dDevice_->SetRenderState( D3DRS_LIGHTING, FALSE );

    return S_OK;
}
 