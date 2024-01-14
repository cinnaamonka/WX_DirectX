float4x4 worldViewProjection : WorldViewProjection;
float4x4 worldMatrix : WORLD;

Texture2D gDiffuseMap : DiffuseMap;

float3 gLightDirection : LightDirection = { 0.577f, -0.577f, 0.577f };

float3 gCameraPos : CAMERA;

float gPI = 3.14;
float gLightIntensity = 7.0f;
float gShininess = 25.0f;

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};
SamplerState samAnisotropic
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};
BlendState gBlendState
{
    BlendEnable[0] = true;
    SrcBlend = src_alpha;
    DestBlend = inv_src_alpha;
    BlendOp = add;
    SrcBlendAlpha = zero;
    DestBlendAlpha = zero; 
    BlendOpAlpha = add; 
    RenderTargetWriteMask[0] = 0x0F; 
};
RasterizerState gRasterizerState
{
    CullMode = none;
    FrontCounterClockwise = false;
};
DepthStencilState gDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = less;
    StencilEnable = false;

    //others are redundant because
    // StencilEnable is FALSE
    // (for demo purposes only)
    StencilReadMask = 0xFF;
    StencilWriteMask = 0xFF;

    FrontFaceStencilFunc = always;
    BackFaceStencilFunc = always;

    FrontFaceStencilDepthFail = keep;
    BackFaceStencilDepthFail = keep;

    FrontFaceStencilPass = keep;
    BackFaceStencilPass = keep;

    FrontFaceStencilFail = keep;
    BackFaceStencilFail = keep;
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float4 Color : COLOR;
    float2 Uv : TEXCOORD;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 Uv : TEXCOORD;
};
// vertex shader
PS_INPUT VS_FireFX(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Position = mul(float4(input.Position, 1.0f), worldViewProjection);
    output.Uv = input.Uv; 
    output.Color = input.Color;
    
    return output;
}
float4 PS_FireFX(PS_INPUT input) : SV_TARGET
{
    float4 color = gDiffuseMap.Sample(samPoint, input.Uv);
    
    return color;
}
technique11 FireEffectTechnique
{
    pass P0
    {
        SetRasterizerState(gRasterizerState);
        SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS_FireFX())); 
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_FireFX())); 
    }
}