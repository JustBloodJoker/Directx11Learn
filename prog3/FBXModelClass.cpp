#include "FBXModelClass.h"
#include "Utilites.h"

struct ConstantBuffer
{
    dx::XMMATRIX mWorld;
    dx::XMMATRIX mView;
    dx::XMMATRIX mProjection;
};


void ModelClass::SetVertexBuffer()
{
    Vertex* vertices = new Vertex[mesh->mNumVertices];
    int ind = 0;
    for (UINT i = 0; i < mesh->mNumVertices; i++) {
        const aiVector3D& position = mesh->mVertices[i];
        vertices[ind++].Pos = dx::XMFLOAT3(position.x, position.y, position.z);
    }

    HRESULT hr;

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(Vertex) * mesh->mNumVertices;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;
    hr = m_pd3dDevice->CreateBuffer(&bd, &InitData, &this->m_pVertexBuffer);

    delete[] vertices;
}

void ModelClass::SetIndexBuffer()
{   
    WORD *indices = new WORD[mesh->mNumFaces * 3];

    int ind = 0;
   
    aiFace face;
   for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
        face = mesh->mFaces[j];
        for (unsigned int k = 0; k < face.mNumIndices; k++) {
            indices[ind] = face.mIndices[k];
            ind++;
        }
   }
   
   HRESULT hr;

   D3D11_BUFFER_DESC bd;
   ZeroMemory(&bd, sizeof(bd));
   bd.Usage = D3D11_USAGE_DEFAULT;
   bd.ByteWidth = sizeof(WORD) * mesh->mNumFaces * 3;
   bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
   bd.CPUAccessFlags = 0;
   D3D11_SUBRESOURCE_DATA InitData;
   ZeroMemory(&InitData, sizeof(InitData));
   InitData.pSysMem = indices;
   hr = m_pd3dDevice->CreateBuffer(&bd, &InitData, &this->m_pIndexBuffer);

   delete[] indices;
}

void ModelClass::SetConstantBuffer()
{
    HRESULT hr;
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = m_pd3dDevice->CreateBuffer(&bd, NULL, &m_pConstantBuffer);
   
    m_World = dx::XMMatrixIdentity();

    dx::XMVECTOR Eye = dx::XMVectorSet(0.0f, -1.0f, 5.0f, 0.0f);
    dx::XMVECTOR At = dx::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
    dx::XMVECTOR Up = dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    m_View = dx::XMMatrixLookAtLH(Eye, At, Up);

    m_Projection = dx::XMMatrixPerspectiveFovLH(dx::XM_PIDIV2, WIDTH / HEIGHT, 0.01f, 1000.0f);


}

bool ModelClass::CompileShader()
{
    HRESULT hr;

    ID3DBlob* pVSBlob = NULL;
    hr = m_compileshaderfromfile(L"shader.fx", "VS", "vs_4_0", &pVSBlob);
    if (FAILED(hr))
    {
        return false;
    }

    hr = m_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &this->m_pVertexShader);
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

    hr = m_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &this->m_pVertexLayout);
    RELEASE(pVSBlob);
    if (FAILED(hr))
        return false;

   

    ID3DBlob* pPSBlob = NULL;
    hr = m_compileshaderfromfile(L"shader.fx", "PS", "ps_4_0", &pPSBlob);
    if (FAILED(hr))
    {

        return false;
    }

    hr = m_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &this->m_pPixelShader);
    RELEASE(pPSBlob);
    if (FAILED(hr))
        return false;
}

int ModelClass::GetIndexCount()
{
    return mesh->mNumFaces * 3;
}

int ModelClass::GetVertexCount()
{
    return mesh->mNumVertices;
}

void ModelClass::DrawMesh()
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_pImmediateContext->IASetVertexBuffers(0, 1, &this->m_pVertexBuffer, &stride, &offset);
    m_pImmediateContext->IASetIndexBuffer(this->m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    static float t = 0.0f;
    static DWORD dwTimeStart = 0;
    DWORD dwTimeCur = GetTickCount();
    if (dwTimeStart == 0)
    	dwTimeStart = dwTimeCur;
    t = (dwTimeCur - dwTimeStart) / 1000.0f;

    m_World = dx::XMMatrixRotationY(t);

    ConstantBuffer cb;
    cb.mProjection = dx::XMMatrixTranspose(m_Projection * m_World * m_View);
    m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, NULL, &cb, 0, 0);
    m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
   

  

    m_pImmediateContext->IASetInputLayout(this->m_pVertexLayout);
    m_pImmediateContext->VSSetShader(this->m_pVertexShader, NULL, 0);
    m_pImmediateContext->PSSetShader(this->m_pPixelShader, NULL, 0);
    
    m_pImmediateContext->DrawIndexed(12 * 3, 0, 0);
}

void ModelClass::LoadModel()
{
    if (mesh == nullptr) {
        return;
    }

    CompileShader();
    SetVertexBuffer();
    SetIndexBuffer();
    SetConstantBuffer();
}

void ModelClass::SetMesh(aiMesh* othermesh)
{
    this->mesh = othermesh;
}














Vertex* ModelClass::InitModel()
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile("q1.obj", aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

    if (!scene) {
        return nullptr;
    }

    const aiMesh* mesh[3];
    int VerticesSize = 0;
    int IndicesSize = 0;
    for (int i = 0; i < scene->mNumMeshes; i++) {
        mesh[i] = scene->mMeshes[i];
        VerticesSize += mesh[i]->mNumVertices;
        IndicesSize += mesh[i]->mNumFaces;
    }

    Vertex* vertices = new Vertex[VerticesSize];
    int ind = 0;
    for (int j = 0; j < scene->mNumMeshes; j++) {
        for (UINT i = 0; i < mesh[j]->mNumVertices; i++) {
            const aiVector3D& position = mesh[j]->mVertices[i];
            vertices[ind++].Pos = dx::XMFLOAT3(position.x, position.y, position.z);
        }
    }

    WORD* indices = new WORD[IndicesSize * 3];

    ind = 0;
    int faceIndeces = 0;
    for (int i = 0; i < scene->mNumMeshes; i++) {
        aiFace face = mesh[i]->mFaces[0];

        for (unsigned int j = 0; j < mesh[i]->mNumFaces; j++) {
            face = mesh[i]->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; k++) {
                indices[ind] = face.mIndices[k] + faceIndeces;
                ind++;
            }
        }
        faceIndeces += mesh[i]->mNumVertices;
    }


    HRESULT hr;

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(Vertex) * VerticesSize;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;
    hr = m_pd3dDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer);


    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * IndicesSize * 3;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    hr = m_pd3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer);

    m_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return vertices;
}