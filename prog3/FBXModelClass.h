#pragma once
#include "GraphicClass.h"



class ModelClass : public GraphicClass
{
public:
	ModelClass() = default;

	Vertex* InitModel();
	int GetSize() { return size; };
	void SetDeviceAndContext(ID3D11Device*& m_pd3dDevice,
		ID3D11DeviceContext*& m_pImmediateContext) {
		this->m_pd3dDevice = m_pd3dDevice;
		this->m_pImmediateContext = m_pImmediateContext;
	}

	void SetVertexBuffer();
	void SetIndexBuffer();
	void SetConstantBuffer();
	bool CompileShader();
	int GetIndexCount();
	int GetVertexCount();
	void DrawMesh();
	void LoadModel();

	void SetMesh(aiMesh * othermesh);

private:
	ID3D11Device* m_pd3dDevice;
	ID3D11DeviceContext* m_pImmediateContext;

	aiMesh* mesh;

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
};

