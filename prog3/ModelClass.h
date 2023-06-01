#pragma once
//#include "GraphicClass.h"
#include "stdafx.h"

class ModelClass
{
public:
	ModelClass() = default;
	ModelClass(ID3D11Device* m_pd3dDevice,
	ID3D11DeviceContext* m_pImmediateContext, dx::XMMATRIX& m_View,
	dx::XMMATRIX& m_Projection, const char* sceneName,
		const wchar_t* vsFilename, const wchar_t* psFilename) :
									renderSceneName(sceneName),
									m_pd3dDevice(m_pd3dDevice),
									m_pImmediateContext(m_pImmediateContext),
									vsFilename(vsFilename),
									psFilename(psFilename),
									m_View(m_View), 
									m_Projection(m_Projection)
														{};

	//~ModelClass() { Close(); }

	bool InitScene();
	void DrawScene();

	void Close();

private:
	HRESULT m_compileshaderfromfile(const wchar_t* FileName, LPCSTR EntryPoint, LPCSTR ShaderModel, ID3DBlob** ppBlobOut);

	bool ConstantBufferCreated = false;
	bool ConstantBufferLoaded = false;

	const wchar_t* vsFilename,* psFilename;

	bool InitMesh(aiMesh* mesh);
	bool InitShader();

	void PushBackData();

	void RenderBuffers(size_t iter);
	void SetShaderParameters();
	void RenderShader();

	const char* renderSceneName;

	ID3D11Device* m_pd3dDevice;
	ID3D11DeviceContext* m_pImmediateContext;
	dx::XMMATRIX& m_View;
	dx::XMMATRIX& m_Projection;

	const aiScene* scene;

	std::vector<ID3D11Buffer*> m_vertexBuffer;
	std::vector<ID3D11Buffer*> m_indexBuffer;
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader*  m_pixelShader;
	ID3D11InputLayout*  m_layout;
	ID3D11Buffer* m_pConstantBuffer;
	ID3D11SamplerState* m_sampleState;
	std::vector<ID3D11ShaderResourceView*> m_texture;

	dx::XMMATRIX m_objMatrix;

	std::vector<unsigned short> m_indexCount;
	float m_rot;

	struct Materials {
		float Ns;
		dx::XMFLOAT3 Ka;
		dx::XMFLOAT3 Kd;
		dx::XMFLOAT3 Ks;
		dx::XMFLOAT3 Ke;
		float Ni;
		float d;
		int illum;
	};

	


	struct ConstantBuffer
	{
		dx::XMMATRIX WVP;
		Materials material;
	};

	std::map<std::string, Materials> materials;
};