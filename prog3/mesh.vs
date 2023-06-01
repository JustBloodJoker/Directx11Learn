cbuffer cbPerObject
{
	float4x4 WVP;
    float Ns;
    float3 Ka;
    float3 Kd;
    float3 Ks;
    float3 Ke;
    float Ni;
    float d;
    int illum;
};

struct VertexInputType
{
    float4 pos : POSITION;
    float2 tex : TEXCOORD;
};

struct PixelInputType
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

PixelInputType VS(VertexInputType input)
{
    PixelInputType output;

    output.pos = mul(input.pos, WVP);
    output.tex = input.tex;

    return output;
}