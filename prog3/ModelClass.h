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

	~ModelClass() { Close(); }

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
	void DrawMesh();
	
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

};


/*

ModelClass()
	{
		this->m_pd3dDevice = nullptr;
		this->m_pImmediateContext = nullptr;
	};

	ModelClass(ID3D11Device* m_pd3dDevice, ID3D11DeviceContext* m_pImmediateContext)
	{
		this->m_pd3dDevice = m_pd3dDevice;
		this->m_pImmediateContext = m_pImmediateContext;

	};

	void SetDevice(ID3D11Device* m_pd3dDevice) {
		this->m_pd3dDevice = m_pd3dDevice;
	}

	void SetContext(ID3D11DeviceContext* m_pImmediateContext) {
		this->m_pImmediateContext = m_pImmediateContext;
	}

	Vertex* InitModel();


	void SetVertexBuffer();
	void SetIndexBuffer();
	void SetConstantBuffer();
	bool CompileShader();
	int GetIndexCount();
	int GetVertexCount();
	void DrawMesh();
	void LoadModel();

	void SetMesh(aiMesh* othermesh);





*/




/*

HRESULT m_compileshaderfromfile(const wchar_t* FileName, LPCSTR EntryPoint, LPCSTR ShaderModel, ID3DBlob** ppBlobOut);

	aiMesh* mesh;

	ID3D11Device* m_pd3dDevice;
	ID3D11DeviceContext* m_pImmediateContext;


	ID3D11Buffer* m_pVertexBuffer;
	ID3D11InputLayout* m_pVertexLayout;
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11Buffer* m_pIndexBuffer;
	ID3D11Buffer* m_pConstantBuffer;

	dx::XMMATRIX m_World;
	dx::XMMATRIX m_View;
	dx::XMMATRIX m_Projection;

	int size = 0;

*/