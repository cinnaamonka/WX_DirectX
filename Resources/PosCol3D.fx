float4x4 worldViewProjection : WorldViewProjection;
float4x4 worldMatrix : WORLD;

Texture2D gDiffuseMap : DiffuseMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;
Texture2D gNormalMap : NormalMap;

float3 gLightDirection : LightDirection = { 0.577f, -0.577f, 0.577f };

float3 gCameraPos : CAMERA;

float gPI = 3.14;
float gLightIntensity = 7.0f;
float gShininess = 25.0f;
bool gNormalMapEnabled = false;

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
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};


struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 ViewDirection : VERTEX_TO_CAMERA;
};


VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Position = mul(float4(input.Position, 1.f), worldMatrix);
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.tangent = mul(input.tangent, (float3x3) worldMatrix);
    
    output.ViewDirection = normalize(gCameraPos - output.Position.xyz);
   
    output.Position = mul(float4(input.Position, 1.f), worldViewProjection);
    output.uv = input.uv;
    output.Color = input.Color;
       
    return output;
}

float4 PS_Point(VS_OUTPUT input) : SV_TARGET
{
    float4 ambientOcclusion = float4(0.05f, 0.05f, 0.05f, 1.0f);
    float sampledSpecular = 0.5f;
    float sampledPhongExponent = 20.0f;
    float diffuseStrengthKd = 7.0f;
    
  
	//normal mapping
    float3 binormal = cross(input.normal, input.tangent);
    float3x3 tangentSpaceAxis = float3x3(input.tangent, binormal, input.normal);
    float3 sampledNormalColor = gNormalMap.Sample(samPoint, input.uv).rgb;

    sampledNormalColor = 2 * sampledNormalColor - 1;

    float3 resultNormal;
    
    if (gNormalMapEnabled)
    {
        resultNormal = normalize(mul(sampledNormalColor, tangentSpaceAxis));

    }
    else
    {
        resultNormal = input.normal;

    }
   
    float cosAngle = dot(resultNormal, normalize(-gLightDirection));
    
 	// lambert diffuse
    float4 sampleDiffuse = gDiffuseMap.Sample(samPoint, input.uv);
    float4 lambertDiffuse = sampleDiffuse * diffuseStrengthKd / gPI;

  
	// phong 
    float3 reflection = reflect(normalize(gLightDirection), resultNormal);
    float cosAlpha = max(dot(reflection, input.ViewDirection), 0.0f);
    sampledSpecular = gSpecularMap.Sample(samPoint, input.uv);
    sampledPhongExponent = gGlossinessMap.Sample(samPoint, input.uv);

    float4 specularColor = sampledSpecular * pow(cosAlpha, sampledPhongExponent * gShininess);
  
   
    return (lambertDiffuse + specularColor + ambientOcclusion) * cosAngle;
};
float4 PS_Linear(VS_OUTPUT input) : SV_TARGET
{
    float4 ambientOcclusion = float4(0.05f, 0.05f, 0.05f, 1.0f);
    float sampledSpecular = 0.5f;
    float sampledPhongExponent = 20.0f;
    float diffuseStrengthKd = 7.0f;
    
  
	//normal mapping
    float3 binormal = cross(input.normal, input.tangent);
    float3x3 tangentSpaceAxis = float3x3(input.tangent, binormal, input.normal);
    float3 sampledNormalColor = gNormalMap.Sample(samLinear, input.uv).rgb;

    sampledNormalColor = 2 * sampledNormalColor - 1;

    float3 resultNormal;
    
    if (gNormalMapEnabled)
    {
        resultNormal = normalize(mul(sampledNormalColor, tangentSpaceAxis));
    }
    else
    {
        resultNormal = input.normal;

    }

    float cosAngle = dot(resultNormal, normalize(-gLightDirection));
 	// lambert diffuse

    float4 sampleDiffuse = gDiffuseMap.Sample(samLinear, input.uv);
    float4 lambertDiffuse = sampleDiffuse * diffuseStrengthKd / gPI;

  
	// phong 
    float3 reflection = reflect(normalize(gLightDirection), resultNormal);
    float cosAlpha = max(dot(reflection, input.ViewDirection), 0.0f);
    sampledSpecular = gSpecularMap.Sample(samLinear, input.uv);
    sampledPhongExponent = gGlossinessMap.Sample(samLinear, input.uv);

    float4 specularColor = sampledSpecular * pow(cosAlpha, sampledPhongExponent * gShininess);
  
   
    return (lambertDiffuse + specularColor + ambientOcclusion) * cosAngle;
}


float4 PS_Anisotropic(VS_OUTPUT input) : SV_TARGET
{
    float4 ambientOcclusion = float4(0.05f, 0.05f, 0.05f, 1.0f);
    float sampledSpecular = 0.5f;
    float sampledPhongExponent = 20.0f;
    float diffuseStrengthKd = 7.0f;
    
  
	//normal mapping
    float3 binormal = cross(input.normal, input.tangent);
    float3x3 tangentSpaceAxis = float3x3(input.tangent, binormal, input.normal);
    float3 sampledNormalColor = gNormalMap.Sample(samAnisotropic, input.uv).rgb;

    sampledNormalColor = 2 * sampledNormalColor - 1;

    float3 resultNormal;
    
    if (gNormalMapEnabled)
    {
        resultNormal = normalize(mul(sampledNormalColor, tangentSpaceAxis));
    }
    else
    {
        resultNormal = input.normal;

    }

    float cosAngle = dot(resultNormal, normalize(-gLightDirection));
 	// lambert diffuse

    float4 sampleDiffuse = gDiffuseMap.Sample(samAnisotropic, input.uv);
    float4 lambertDiffuse = sampleDiffuse * diffuseStrengthKd / gPI;

  
	// phong 
    float3 reflection = reflect(normalize(gLightDirection), resultNormal);
    float cosAlpha = max(dot(reflection, input.ViewDirection), 0.0f);
    sampledSpecular = gSpecularMap.Sample(samAnisotropic, input.uv);
    sampledPhongExponent = gGlossinessMap.Sample(samAnisotropic, input.uv);

    float4 specularColor = sampledSpecular * pow(cosAlpha, sampledPhongExponent * gShininess);
  
   
    return (lambertDiffuse + specularColor + ambientOcclusion) * cosAngle;
}

BlendState gBlendState
{
    BlendEnable[0] = false; 
    SrcBlend = one; 
    DestBlend = zero; 
    BlendOp = add; 
    SrcBlendAlpha = one; 
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
    DepthWriteMask = ALL;
    DepthFunc = less;
    StencilEnable = false;

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
technique11 DefaultTechnique
{
    pass P0
    {
        SetRasterizerState(gRasterizerState);
        SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Point()));
    }
    
    pass P1
    {
        SetRasterizerState(gRasterizerState);
        SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Linear()));
    }
    
    pass P2
    {
        SetRasterizerState(gRasterizerState);
        SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Anisotropic()));
    }
}

