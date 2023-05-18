#pragma once
#include "stdafx.h"
#include "ModelClass.h"

class GraphicClass
{
private:
	friend class ModelClass;

public:
	GraphicClass()
	{
		m_driverType = D3D_DRIVER_TYPE_NULL;
		m_featureLevel = D3D_FEATURE_LEVEL_11_0;
		m_pd3dDevice = nullptr;
		m_pImmediateContext = nullptr;
		m_pSwapChain = nullptr;
		m_pRenderTargetView = nullptr;

		TexSamplerState = nullptr;
		Texture = nullptr;
	}
	virtual ~GraphicClass() { ShutDown(); };

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

	void InputScene(const char* sceneName,const wchar_t* vsFilename, const wchar_t* psFilename);

	

private:
	
	ID3D11Device* m_pd3dDevice;
	ID3D11DeviceContext* m_pImmediateContext;
	

	dx::XMMATRIX m_View;
	dx::XMMATRIX m_Projection;
	

	HWND hwnd;

	D3D_DRIVER_TYPE m_driverType;
	D3D_FEATURE_LEVEL m_featureLevel;
	
	IDXGISwapChain* m_pSwapChain;
	ID3D11RenderTargetView* m_pRenderTargetView;

	ID3D11ShaderResourceView* Texture;
	ID3D11SamplerState* TexSamplerState;

	std::vector<ModelClass> scenes;

};

