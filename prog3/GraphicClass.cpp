#include "GraphicClass.h"
#include "Utilites.h"

#include <d2d1.h>
#include <dwrite.h>
#include "ModelClass.h"

using namespace DirectX;


bool GraphicClass::CreateDevice()
{
	HRESULT hr = S_OK;



	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,


	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = WIDTH;
	sd.BufferDesc.Height = HEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = !_WINDOWED;
	

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++) {
		m_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, m_driverType, NULL,
			NULL, featureLevels, numFeatureLevels, D3D11_SDK_VERSION,
			&sd, &m_pSwapChain, &m_pd3dDevice, &m_featureLevel, &m_pImmediateContext);
		if (SUCCEEDED(hr)) {
			break;
		}
	}
	if (FAILED(hr))
		return false;

	ID3D11Texture2D* pBackBuffer = NULL;
	hr = m_pSwapChain->GetBuffer(NULL, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return false;

	hr = m_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
	RELEASE(pBackBuffer);
	if (FAILED(hr))
		return false;

	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

	D3D11_VIEWPORT vp;
	vp.Width = WIDTH;
	vp.Height = HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	m_pImmediateContext->RSSetViewports(1, &vp);


	return true;
}

void GraphicClass::BeginFrame()
{
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, ClearColor);
}

void GraphicClass::EndFrame()
{
	m_pSwapChain->Present(0, 0);
}

void GraphicClass::ShutDown()
{
	Close();
	if (m_pImmediateContext)
		m_pImmediateContext->ClearState();

	RELEASE(m_pRenderTargetView);
	RELEASE(m_pSwapChain);
	RELEASE(m_pImmediateContext);
	RELEASE(m_pd3dDevice);
}

bool GraphicClass::Init()
{
	HRESULT hr = S_OK;
	if (!CreateDevice())
	{
		MessageBox(0, L"Direct3D Creation Device and Swap chain - Failed",
			L"Error", MB_OK);
		return false;
	}


	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = TRUE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	
	hr = m_pd3dDevice->CreateDepthStencilState(&depthStencilDesc, &pDepthStencilState);
	



	Eye = XMVectorSet(0.0f, 0.0f, 10.0f, 1.0f);
	At = XMVectorSet(0.0f, 10.0f, 0.0f, 0.0f);
	Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = XMMatrixLookAtLH(Eye, At, Up);

	m_Projection = dx::XMMatrixPerspectiveFovLH(dx::XM_PIDIV2, WIDTH / HEIGHT, 0.01f, 1000.0f);

	InputScene("Grass.obj", "Grass.mtl", L"mesh.vs", L"mesh.ps");
	

	for (size_t iter = 0; iter < scenes.size(); iter++) {
		scenes[iter].InitScene();
	}
	

	return true;
}

bool GraphicClass::Draw()
{
	m_pImmediateContext->OMSetDepthStencilState(pDepthStencilState, 1);

	for (float x = -15.0f; x < 15.0f; x += 15.0f) {
		for (float z = -15.0f; z < 15.0f; z += 15.0f) {
			scenes[0].ChangePlaceXYZ(x, 0.0f, z);
			scenes[0].DrawScene();

		}
	}
	
	if (GetAsyncKeyState('W') & 0x8000) 
	{
		Eye += (0.1f * (At - Eye));
	}
	if (GetAsyncKeyState('S') & 0x8000) 
	{
		Eye -= (0.1f * (At - Eye));
	}
	if (GetAsyncKeyState('A') & 0x8000)  
	{
		Eye -= (0.1f * XMVector3Normalize(XMVector3Cross(At - Eye, Up)));
	}
	if (GetAsyncKeyState('D') & 0x8000)  
	{
		Eye += (0.1f * XMVector3Normalize(XMVector3Cross(At - Eye, Up)));
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)  
	{
		Eye += (0.1f * Up);
	}
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)  
	{
		Eye -= (0.1f * Up);
	}

	m_View = XMMatrixLookAtLH(Eye, At, Up);

	return true;
}

void GraphicClass::Close()
{
	RELEASE(TexSamplerState);
	RELEASE(Texture);

	RELEASE(pDepthStencilState);

}

void GraphicClass::InputScene(const char* sceneName, const char* materialName, const wchar_t* vsFilename, const wchar_t* psFilename)
{
	scenes.push_back(ModelClass(this->m_pd3dDevice, this->m_pImmediateContext,this->m_View, m_Projection, sceneName, materialName, vsFilename, psFilename));
}
