#pragma once
#include "stdafx.h"

struct Vertex {
	dx::XMFLOAT3 Pos;
	dx::XMFLOAT2 texCoord;
};

class GraphicClass
{
public:
	GraphicClass()
	{
		m_driverType = D3D_DRIVER_TYPE_NULL;
		m_featureLevel = D3D_FEATURE_LEVEL_11_0;
		m_pd3dDevice = nullptr;
		m_pImmediateContext = nullptr;
		m_pSwapChain = nullptr;
		m_pRenderTargetView = nullptr;

		m_pVertexShader = nullptr;
		m_pPixelShader = nullptr;
		m_pVertexLayout = nullptr;
		m_pVertexBuffer = nullptr;

		m_pIndexBuffer = nullptr;
		m_pConstantBuffer = nullptr;

		TexSamplerState = nullptr;
		Texture = nullptr;
	}
	virtual ~GraphicClass() = default;

	bool CreateDevice();
	void BeginFrame();
	void EndFrame();
	void ShutDown();

	bool Init();
	bool Draw();
	void Close();
	
	void SetHWND(HWND &hwnd) { this->hwnd = hwnd; }

	void* operator new(size_t i)
	{
		return _aligned_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_aligned_free(p);
	}

private:
	friend class ModelClass;

	HWND hwnd;

	D3D_DRIVER_TYPE m_driverType;
	D3D_FEATURE_LEVEL m_featureLevel;
	ID3D11Device* m_pd3dDevice;
	ID3D11DeviceContext* m_pImmediateContext;
	IDXGISwapChain* m_pSwapChain;
	ID3D11RenderTargetView* m_pRenderTargetView;

	HRESULT m_compileshaderfromfile(const wchar_t* FileName, LPCSTR EntryPoint, LPCSTR ShaderModel, ID3DBlob** ppBlobOut);

	ID3D11Buffer* m_pVertexBuffer;
	ID3D11InputLayout* m_pVertexLayout;
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;

	dx::XMMATRIX m_World;
	dx::XMMATRIX m_View;
	dx::XMMATRIX m_Projection;

	ID3D11Buffer* m_pIndexBuffer;
	ID3D11Buffer* m_pConstantBuffer;

	ID3D11ShaderResourceView* Texture;
	ID3D11SamplerState* TexSamplerState;

};

