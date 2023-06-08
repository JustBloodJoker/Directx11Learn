#include "ModelClass.h"
#include "Utilites.h"

struct Vertex
{
    dx::XMFLOAT3 Pos = { 0,0,0 };
    dx::XMFLOAT2 Tex = { 0,0 };
};

bool ModelClass::InitScene()
{
    m_objMatrix = dx::XMMatrixIdentity();

    Assimp::Importer importer;

    scene = importer.ReadFile(renderSceneName, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

    if (!scene)
        return false;

    std::fstream mtlFile;
    mtlFile.open(materialName, std::ofstream::in);
   

    std::string nameMtl;
    char temp;

    if (mtlFile)
    {
        while (mtlFile)
        {
            temp = mtlFile.get(); 

            switch (temp)
            {
            case '#':
                temp = mtlFile.get();
                while (temp != '\n')
                    temp = mtlFile.get();
                break;
            case 'n': {
                temp = mtlFile.get();
               if(temp == 'e') {
                   temp = mtlFile.get();
                   if (temp == 'w') {
                        temp = mtlFile.get();
                        if (temp == 'm') {
                            temp = mtlFile.get();
                            if (temp == 't') {
                                temp = mtlFile.get();
                                if (temp == 'l') {
                                    mtlFile >> nameMtl;
                                    materials[nameMtl] = Materials();
                                }
                            }
                        }
                    }
                }
            break;
            }
            case 'N': {
                temp = mtlFile.get();
                if (temp == 's') {
                    mtlFile >> materials[nameMtl].Ns;
                }
                else if (temp == 'i') {
                    mtlFile >> materials[nameMtl].Ni;
                }
            break;
            }
            case 'K': {
                temp = mtlFile.get();
                if (temp == 'a') {
                    mtlFile >> materials[nameMtl].Ka.x >> materials[nameMtl].Ka.y >> materials[nameMtl].Ka.z;
                }
                else if (temp == 'd') {
                    mtlFile >> materials[nameMtl].Kd.x >> materials[nameMtl].Kd.y >> materials[nameMtl].Kd.z;
                }
                else if (temp == 's') {
                    mtlFile >> materials[nameMtl].Ks.x >> materials[nameMtl].Ks.y >> materials[nameMtl].Ks.z;
                }
                else if (temp == 'e') {
                    mtlFile >> materials[nameMtl].Ke.x >> materials[nameMtl].Ke.y >> materials[nameMtl].Ke.z;
                }
            break;
            }
            case 'd':{
                mtlFile >> materials[nameMtl].d;
                break;
            }
            case 'i':
                temp = mtlFile.get();
                if (temp == 'l') {
                    temp = mtlFile.get();
                    if (temp == 'l') {
                        temp = mtlFile.get();
                        if (temp == 'u') {
                            temp = mtlFile.get();
                            if (temp == 'm') {
                                mtlFile >> materials[nameMtl].illum;
                            }
                        }
                    }
                }
                break;
            }
        }
    }

    for (size_t iter = 0; iter < static_cast<size_t>(scene->mNumMeshes); iter++) {
        if (!InitMesh(scene->mMeshes[iter]))
            return false;
    }

    if(!InitShader())
        return false;

    return true;
}

HRESULT ModelClass::m_compileshaderfromfile(const wchar_t* FileName, LPCSTR EntryPoint, LPCSTR ShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    hr = D3DX11CompileFromFile(FileName, NULL, NULL, EntryPoint, ShaderModel, NULL, NULL,
        NULL, ppBlobOut, NULL, NULL);

    return hr;
}

bool ModelClass::InitMesh(aiMesh* mesh)
{
    PushBackData();
    
    size_t VertexCount = static_cast<size_t>(mesh->mNumVertices);
    size_t TriangleCount = static_cast<size_t>(mesh->mNumFaces);

    m_indexCount[m_indexCount.size() - 1] += TriangleCount * 3;

    WORD* indices = new WORD[TriangleCount * 3];
    if (!indices)
        return false;

    Vertex* vertices = new Vertex[VertexCount];
    if (!vertices)
        return false;

    for (size_t iter = 0; iter < TriangleCount; iter++) {
        indices[iter*3    ] = mesh->mFaces[iter].mIndices[0];
        indices[iter*3 + 1] = mesh->mFaces[iter].mIndices[1];
        indices[iter*3 + 2] = mesh->mFaces[iter].mIndices[2];
    }
    
    for (size_t iter = 0; iter < VertexCount; iter++) {
        vertices[iter].Pos.x = mesh->mVertices[iter].x;
        vertices[iter].Pos.y = mesh->mVertices[iter].y;
        vertices[iter].Pos.z = mesh->mVertices[iter].z;

        if (mesh->HasTextureCoords(0)) {
            vertices[iter].Tex.x = mesh->mTextureCoords[0]->x;
            vertices[iter].Tex.y = mesh->mTextureCoords[0]->y;
        }

    }



    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(Vertex) * VertexCount;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA Data;
    ZeroMemory(&Data, sizeof(Data));
    Data.pSysMem = vertices;
    HRESULT hr = m_pd3dDevice->CreateBuffer(&bd, &Data, &m_vertexBuffer[m_vertexBuffer.size() - 1]);
    if (FAILED(hr))
        return false;

    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * m_indexCount[m_indexCount.size() - 1];
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    Data.pSysMem = indices;
    hr = m_pd3dDevice->CreateBuffer(&bd, &Data, &m_indexBuffer[m_indexBuffer.size() - 1]);
    if (FAILED(hr))
        return false;


   
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = m_pd3dDevice->CreateBuffer(&bd, NULL, &m_pConstantBuffer);
    if (FAILED(hr))
        return false;

    
    
    return true;
}

bool ModelClass::InitShader()
{
    m_texture.push_back({ 0 });
    m_texture.push_back({ 0 });

    HRESULT hr = S_OK;

    hr = D3DX11CreateShaderResourceViewFromFile(m_pd3dDevice, L"Grass1.jpg", NULL, NULL, &m_texture[0], NULL);
    if (FAILED(hr))
        return false;

    hr = D3DX11CreateShaderResourceViewFromFile(m_pd3dDevice, L"Grass2.jpg", NULL, NULL, &m_texture[1], NULL);
    if (FAILED(hr))
        return false;

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = m_pd3dDevice->CreateSamplerState(&sampDesc, &m_sampleState);
    if (FAILED(hr))
        return false;


    ID3DBlob* vertexShaderBuffer = nullptr;
    hr = m_compileshaderfromfile(vsFilename, "VS", "vs_4_0", &vertexShaderBuffer);
    if (FAILED(hr))
        return false;

    ID3DBlob* pixelShaderBuffer = nullptr;
    HRESULT result =  m_compileshaderfromfile(psFilename, "PS", "ps_4_0", &pixelShaderBuffer);
    if (FAILED(hr))
        return false;

    result =  m_pd3dDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
    if (FAILED(result))
        return false;

    result =  m_pd3dDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
    if (FAILED(result))
        return false;

    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;
    polygonLayout[1].SemanticName = "TEXCOORD";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    result =  m_pd3dDevice->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
    if (FAILED(result))
        return false;

    vertexShaderBuffer->Release();
    pixelShaderBuffer->Release();

    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    result =  m_pd3dDevice->CreateSamplerState(&samplerDesc, &m_sampleState);
    if (FAILED(result))
        return false;

    return true;
}

void ModelClass::PushBackData()
{
    m_vertexBuffer.push_back(nullptr);
    m_indexBuffer.push_back(nullptr);
   // m_texture.push_back(nullptr);
    m_indexCount.push_back(0);
}

void ModelClass::DrawScene()
{
    m_rot += 0.05f;
    if (m_rot > 6.26f)
        m_rot = 0.0f;

    SetShaderParameters();
    RenderShader();

    for (size_t i = 1; i < m_vertexBuffer.size(); i++) {
       RenderBuffers(i);
    }

}

void ModelClass::ChangePlaceXYZ(float x, float y, float z)
{
    m_objMatrix = dx::XMMatrixTranslation(x, y, z);
}

void ModelClass::Close()
{
    m_pd3dDevice = nullptr;
    m_pImmediateContext = nullptr;

    for (size_t iter = 0; iter < m_vertexBuffer.size(); iter++) {
        RELEASE(m_vertexBuffer[iter]);
        RELEASE(m_indexBuffer[iter]);
        RELEASE(m_texture[iter]);
    }
    
    RELEASE(m_vertexShader);
    
    m_texture.~vector();
    m_indexBuffer.~vector();
    m_vertexBuffer.~vector();
    m_indexCount.~vector();
}

void ModelClass::RenderBuffers(size_t iter)
{
    unsigned int stride = sizeof(Vertex);
    unsigned int offset = 0;

    m_pImmediateContext->IASetVertexBuffers(0, 1, &m_vertexBuffer[iter], &stride, &offset);
    m_pImmediateContext->IASetIndexBuffer(m_indexBuffer[iter], DXGI_FORMAT_R16_UINT, 0);
    
    if(iter % 2 == 0)
    m_pImmediateContext->PSSetShaderResources(0, 1, &m_texture[0]);
    else {
        m_pImmediateContext->PSSetShaderResources(0, 1, &m_texture[1]);
    }
    m_pImmediateContext->DrawIndexed(m_indexCount[iter], 0, 0);
}

void ModelClass::SetShaderParameters()
{    


     dx::XMMATRIX WVP = m_objMatrix * m_View * m_Projection;
     ConstantBuffer cb;

     cb.WVP = dx::XMMatrixTranspose(WVP);
     cb.material = materials["dirt"];
     

     m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, NULL, &cb, 0, 0);
     m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

     m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ModelClass::RenderShader()
{
     m_pImmediateContext->IASetInputLayout(m_layout);
     m_pImmediateContext->VSSetShader(m_vertexShader, NULL, 0);
     m_pImmediateContext->PSSetShader(m_pixelShader, NULL, 0);
     m_pImmediateContext->PSSetSamplers(0, 1, &m_sampleState);
}
