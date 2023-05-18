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


	dx::XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -10.0f, 0.0f);
	dx::XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	dx::XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = XMMatrixLookAtLH(Eye, At, Up);

	m_Projection = dx::XMMatrixPerspectiveFovLH(dx::XM_PIDIV2, WIDTH / HEIGHT, 0.01f, 1000.0f);

	InputScene("q1.obj", L"mesh.vs", L"mesh.ps");

	for (size_t iter = 0; iter < scenes.size(); iter++) {
		scenes[iter].InitScene();
	}
/*
//	Assimp::Importer importer;

//	const aiScene* scene = importer.ReadFile("q1.obj", aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);


//	if (!scene) {
//		return false;
//	}
	
//	meshes = new ModelClass[scene->mNumMeshes];



//	for (size_t i = 0; i < scene->mNumMeshes; i++) {
//		meshes[i].SetDevice(m_pd3dDevice);
//		meshes[i].SetContext(m_pImmediateContext);
//		meshes[i].SetMesh(scene->mMeshes[i]);
//		meshes[i].LoadModel();
//	}
	
//	numMeshes = scene->mNumMeshes;
	*/

	return true;
}

bool GraphicClass::Draw()
{
	
	for (size_t iter = 0; iter < scenes.size(); iter++) {
		scenes[iter].DrawScene();
	}

	return true;
}

void GraphicClass::Close()
{
	RELEASE(TexSamplerState);
	RELEASE(Texture);
}

void GraphicClass::InputScene(const char* sceneName, const wchar_t* vsFilename, const wchar_t* psFilename)
{
	scenes.push_back(ModelClass(this->m_pd3dDevice, this->m_pImmediateContext,this->m_View, m_Projection, sceneName, vsFilename, psFilename));
}



