/**-----------------------------------------------------------------------------
 * \brief Ű������ ����� �ִϸ��̼�
 * ����: Animate.cpp
 *
 * ����: �ִϸ��̼��� ���� ��ǥ���� ����� Ű������ �ִϸ��̼��̴�.
 *       �츮�� Position�� Rotation�� Ű���� �����, �̵� Ű�� ����(interpoate)
 *       �ϴ� �ִϸ��̼��� ������ �� ���̴�.
 *------------------------------------------------------------------------------
 */

//#include "ConstHeader.h"

#include "ASELoader.h"
#include "Device.h"

/**-----------------------------------------------------------------------------
 * ī�޶� ��� ����
 *------------------------------------------------------------------------------
 */
void SetupCamera()
{
	/// ������� ����
    D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity( &matWorld );
	g_Device.pd3dDevice_->SetTransform( D3DTS_WORLD, &matWorld );

    /// ������� ����
    D3DXVECTOR3 vEyePt( 0.0f, 10.0f,-20.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    g_Device.pd3dDevice_->SetTransform( D3DTS_VIEW, &matView );

    /// �������� ��� ����
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    g_Device.pd3dDevice_->SetTransform( D3DTS_PROJECTION, &matProj );
}


/**-----------------------------------------------------------------------------
 * ȭ�� �׸���
 *------------------------------------------------------------------------------
 */
VOID Render()
{
	D3DXMATRIXA16	matWorld;

    /// �ĸ���ۿ� Z���� �ʱ�ȭ
    g_Device.pd3dDevice_->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );


    /// ������ ����
    if( SUCCEEDED( g_Device.pd3dDevice_->BeginScene() ) )
    {
		auto iter = g_aseLoader.map_Models_.begin();

		for(; iter != g_aseLoader.map_Models_.end(); ++iter)
		{
			iter->second.Render();
		}

		g_Device.pd3dDevice_->EndScene();
    }

    /// �ĸ���۸� ���̴� ȭ������!
    g_Device.pd3dDevice_->Present( NULL, NULL, NULL, NULL );
}


/**-----------------------------------------------------------------------------
 * ������ ���ν���
 *------------------------------------------------------------------------------
 */

#define ROT_DELTA	0.1f
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;
		case WM_KEYDOWN :
			break;
			
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}




/**-----------------------------------------------------------------------------
 * ���α׷� ������
 *------------------------------------------------------------------------------
 */
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    /// ������ Ŭ���� ���
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "BasicFrame", NULL };
    RegisterClassEx( &wc );

    /// ������ ����
    HWND hWnd = CreateWindow( "BasicFrame", "Keyframe Animation",
                              WS_OVERLAPPEDWINDOW, 100, 100, 500, 500,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );


    /// Direct3D �ʱ�ȭ
	if( SUCCEEDED( g_Device.InitD3D( hWnd ) ) )
    {
		g_aseLoader.Initialize();

		SetupCamera();

        /// ������ ���
		ShowWindow( hWnd, SW_SHOWDEFAULT );
		UpdateWindow( hWnd );

        /// �޽��� ����
		MSG msg;
		ZeroMemory( &msg, sizeof(msg) );
		while( msg.message!=WM_QUIT )
		{
            /// �޽���ť�� �޽����� ������ �޽��� ó��
			if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			/// ó���� �޽����� ������ Render()�Լ� ȣ��
				Render();
		}
    }

	/// ��ϵ� Ŭ���� �Ұ�
    UnregisterClass( "D3D Tutorial", wc.hInstance );
    return 0;
}
