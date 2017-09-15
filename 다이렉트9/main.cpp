/**-----------------------------------------------------------------------------
 * \brief 키프레임 방식의 애니메이션
 * 파일: Animate.cpp
 *
 * 설명: 애니메이션의 가장 대표적인 기법은 키프레임 애니메이션이다.
 *       우리는 Position과 Rotation의 키값을 만들고, 이들 키를 보간(interpoate)
 *       하는 애니메이션을 제작해 볼 것이다.
 *------------------------------------------------------------------------------
 */

//#include "ConstHeader.h"

#include "ASELoader.h"
#include "Device.h"

/**-----------------------------------------------------------------------------
 * 카메라 행렬 설정
 *------------------------------------------------------------------------------
 */
void SetupCamera()
{
	/// 월드행렬 설정
    D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity( &matWorld );
	g_Device.pd3dDevice_->SetTransform( D3DTS_WORLD, &matWorld );

    /// 뷰행렬을 설정
    D3DXVECTOR3 vEyePt( 0.0f, 10.0f,-20.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    g_Device.pd3dDevice_->SetTransform( D3DTS_VIEW, &matView );

    /// 프로젝션 행렬 설정
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    g_Device.pd3dDevice_->SetTransform( D3DTS_PROJECTION, &matProj );
}


/**-----------------------------------------------------------------------------
 * 화면 그리기
 *------------------------------------------------------------------------------
 */
VOID Render()
{
	D3DXMATRIXA16	matWorld;

    /// 후면버퍼와 Z버퍼 초기화
    g_Device.pd3dDevice_->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );


    /// 렌더링 시작
    if( SUCCEEDED( g_Device.pd3dDevice_->BeginScene() ) )
    {
		auto iter = g_aseLoader.map_Models_.begin();

		for(; iter != g_aseLoader.map_Models_.end(); ++iter)
		{
			iter->second.Render();
		}

		g_Device.pd3dDevice_->EndScene();
    }

    /// 후면버퍼를 보이는 화면으로!
    g_Device.pd3dDevice_->Present( NULL, NULL, NULL, NULL );
}


/**-----------------------------------------------------------------------------
 * 윈도우 프로시져
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
 * 프로그램 시작점
 *------------------------------------------------------------------------------
 */
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    /// 윈도우 클래스 등록
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "BasicFrame", NULL };
    RegisterClassEx( &wc );

    /// 윈도우 생성
    HWND hWnd = CreateWindow( "BasicFrame", "Keyframe Animation",
                              WS_OVERLAPPEDWINDOW, 100, 100, 500, 500,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );


    /// Direct3D 초기화
	if( SUCCEEDED( g_Device.InitD3D( hWnd ) ) )
    {
		g_aseLoader.Initialize();

		SetupCamera();

        /// 윈도우 출력
		ShowWindow( hWnd, SW_SHOWDEFAULT );
		UpdateWindow( hWnd );

        /// 메시지 루프
		MSG msg;
		ZeroMemory( &msg, sizeof(msg) );
		while( msg.message!=WM_QUIT )
		{
            /// 메시지큐에 메시지가 있으면 메시지 처리
			if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			/// 처리할 메시지가 없으면 Render()함수 호출
				Render();
		}
    }

	/// 등록된 클래스 소거
    UnregisterClass( "D3D Tutorial", wc.hInstance );
    return 0;
}
