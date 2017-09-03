#include "Device.h"

Device g_Device;

Device::Device():
pD3D_(NULL),
pd3dDevice_(NULL)
{

}

HRESULT Device::InitD3D( HWND hWnd )
{
	 /// ����̽��� �����ϱ����� D3D��ü ����
    if( NULL == ( pD3D_ = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    /// ����̽��� ������ ����ü
    /// ������ ������Ʈ�� �׸����̱⶧����, �̹����� Z���۰� �ʿ��ϴ�.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    /// ����̽� ����
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &pd3dDevice_ ) ) )
    {
        return E_FAIL;
    }

    /// �ø������ ����.
    pd3dDevice_->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

    /// Z���۱���� �Ҵ�.
    pd3dDevice_->SetRenderState( D3DRS_ZENABLE, TRUE );

    /// ������ ������ �����Ƿ�, ��������� ����.
    pd3dDevice_->SetRenderState( D3DRS_LIGHTING, FALSE );

    return S_OK;
}
 