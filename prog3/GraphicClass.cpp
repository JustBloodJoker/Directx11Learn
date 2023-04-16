#include "GraphicClass.h"
#include "Utilites.h"

#include <d2d1.h>
#include <dwrite.h>
#include "FBXModelClass.h"

using namespace DirectX;

ModelClass mesh;

struct SimpleVertex
{
	dx::XMFLOAT3 Pos;
	XMFLOAT2 texCoord;
};

struct ConstantBuffer
{
	dx::XMMATRIX mWorld;
	dx::XMMATRIX mView;
	dx::XMMATRIX mProjection;
};

bool GraphicClass::CreateDevice()
{
	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

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
			createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION,
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

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile("q1.obj", aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	if (!scene) {
		return false;
	}

	
	mesh.SetDeviceAndContext(m_pd3dDevice, m_pImmediateContext);
	mesh.SetMesh(scene->mMeshes[0]);
	

	mesh.LoadModel();
	
	/*ID3DBlob* pVSBlob = NULL;
	hr = m_compileshaderfromfile(L"shader.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		return false;
	}

	hr = m_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &m_pVertexShader);
	if (FAILED(hr))
	{
		RELEASE(pVSBlob);
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	hr = m_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pVertexLayout);
	RELEASE(pVSBlob);
	if (FAILED(hr))
		return false;

	m_pImmediateContext->IASetInputLayout(m_pVertexLayout);

	ID3DBlob* pPSBlob = NULL;
	hr = m_compileshaderfromfile(L"shader.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		
		return false;
	}

	hr = m_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &m_pPixelShader);
	RELEASE(pPSBlob);
	if (FAILED(hr))
		return false;

	ModelClass fbx;
	Vertex* vert = nullptr;

	fbx.SetDeviceAndContext(m_pd3dDevice, m_pImmediateContext);

	vert = fbx.InitModel();
	
	

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = m_pd3dDevice->CreateBuffer(&bd, NULL, &m_pConstantBuffer);
	if (FAILED(hr))
		return false;

	m_World = XMMatrixIdentity();

	XMVECTOR Eye = XMVectorSet(0.0f, -1.0f, 10.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = XMMatrixLookAtLH(Eye, At, Up);

	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, WIDTH / HEIGHT, 0.01f, 1000.0f);
	*/
	

	return true;
}

bool GraphicClass::Draw()
{
	mesh.DrawMesh();
	/*static float t = 0.0f;
	static DWORD dwTimeStart = 0;
	DWORD dwTimeCur = GetTickCount();
	if (dwTimeStart == 0)
		dwTimeStart = dwTimeCur;
	t = (dwTimeCur - dwTimeStart) / 1000.0f;

	m_World = XMMatrixRotationY(t);

	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(m_World);
	cb.mView = XMMatrixTranspose(m_View);
	cb.mProjection = XMMatrixTranspose(m_Projection);
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, NULL, &cb, 0, 0);

	
	m_pImmediateContext->VSSetShader(m_pVertexShader, NULL, 0);
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->PSSetShader(m_pPixelShader, NULL, 0);
	m_pImmediateContext->DrawIndexed(36 * 3, 0, 0);*/

	return true;
}

void GraphicClass::Close()
{
	RELEASE(m_pVertexBuffer);
	RELEASE(m_pVertexLayout);
	RELEASE(m_pVertexShader);
	RELEASE(m_pPixelShader);
	RELEASE(m_pConstantBuffer);
	RELEASE(m_pIndexBuffer);

	RELEASE(TexSamplerState);
	RELEASE(Texture);
}

HRESULT GraphicClass::m_compileshaderfromfile(const wchar_t* FileName, LPCSTR EntryPoint, LPCSTR ShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	hr = D3DX11CompileFromFile(FileName, NULL, NULL, EntryPoint, ShaderModel, NULL, NULL,
		NULL, ppBlobOut, NULL, NULL);

	return hr;
}
