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


struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : TEXCOORD1;
    float3 Color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 ViewDirection : TEXCOORD2;
    float3 LightDirection : TEXCOORD3;
};


VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Position = mul(float4(input.Position, 1.f), worldViewProjection);
    
    // Transform position to world space
    output.WorldPosition = mul(float4(input.Position, 1.f), worldMatrix);
    
    // Calculate view direction in world space
    output.ViewDirection = normalize(gCameraPos - output.WorldPosition.xyz);
    
    output.Color = input.Color;
    output.uv = input.uv;
    output.normal = mul(normalize(input.normal), (float3x3) worldMatrix);
    output.tangent = mul(normalize(input.tangent), (float3x3) worldMatrix);
    return output;
}

float4 PS_Point(VS_OUTPUT input) : SV_TARGET
{
    float4 sampleDiffuseColor = float4(1, 1, 1, 1);
    float4 ambientColor = float4(0.05f, 0.05f, 0.05f, 1.0f);
    float sampledSpecular = 0.5f;
    float sampledPhongExponent = 20.0f;
    float diffuseStrengthKd = 3.0f;
    
    sampledPhongExponent *= gGlossinessMap.Sample(samPoint, input.uv).r;
    sampledSpecular *= gSpecularMap.Sample(samPoint, input.uv).r;
    sampleDiffuseColor *= gDiffuseMap.Sample(samPoint, input.uv);
   
    float3 sampledNormalColor = gNormalMap.Sample(samPoint, input.uv).xyz;
    float3x3 tbnMatrix = float3x3(input.tangent, cross(input.normal, input.tangent), input.normal);
    float3 sampledNormal = mul(sampledNormalColor * 2.0f - 1.0f, tbnMatrix);
   
    float4 lambertDiffuse = sampleDiffuseColor * diffuseStrengthKd / gPI;
    
    float observedArea = max(0.0f, dot(sampledNormal, -gLightDirection));
    
    float3 reflection = reflect(gLightDirection, sampledNormal);
    float cosAlpha = max(0.0f, dot(reflection, input.ViewDirection));
    float specularIntensity = sampledSpecular * pow(cosAlpha, sampledPhongExponent);
    float4 specularColor = specularIntensity * float4(1, 1, 1, 1);
    
    
    return saturate((specularColor + lambertDiffuse) * observedArea + ambientColor);
}


float4 PS_Linear(VS_OUTPUT input) : SV_TARGET
{
    float4 baseColor = float4(1, 1, 1, 1);
    float4 ambient = float4(0.05f, 0.05f, 0.05f, 1.0f);
    float specularIntensity = 0.5f;
    float phongExponent = 20.0f;
    float diffuseStrength = 3.0f;

    // Glossiness and specular maps
    float glossiness = gGlossinessMap.Sample(samLinear, input.uv).r;
    float specularMapSample = gSpecularMap.Sample(samLinear, input.uv).r;

    // Diffuse map
    baseColor *= gDiffuseMap.Sample(samLinear, input.uv);

    // Normal map transformation
    float3 sampledNormalColor = gNormalMap.Sample(samLinear, input.uv).xyz;
    float3x3 tangentToWorldMatrix = float3x3(input.tangent, cross(input.normal, input.tangent), input.normal);
    float3 worldNormal = mul(sampledNormalColor * 2.0f - 1.0f, tangentToWorldMatrix);

    // Lambertian diffuse reflection
    float4 lambertDiffuse = baseColor * (diffuseStrength / 3.16f);

    // Cosine law for observed area
    float observedArea = saturate(dot(worldNormal, gLightDirection));

    // Specular reflection calculation
    float3 reflectedRay = reflect(-gLightDirection, worldNormal);
    float cosAlpha = saturate(dot(reflectedRay, normalize(input.WorldPosition.xyz - gCameraPos)));
    float4 specularColor = specularIntensity * pow(cosAlpha, phongExponent) * float4(1, 1, 1, 1) * specularMapSample;

    // Final color calculation
    float4 finalColor = saturate((specularColor + lambertDiffuse) * observedArea + ambient);

    return finalColor;
}


float4 PS_Anisotropic(VS_OUTPUT input) : SV_TARGET
{
    float4 baseColor = float4(1, 1, 1, 1);
    float4 ambient = float4(0.05f, 0.05f, 0.05f, 1.0f);
    float specularIntensity = 0.5f;
    float phongExponent = 20.0f;
    float diffuseStrength = 3.0f;

    // Glossiness and specular maps
    float glossiness = gGlossinessMap.Sample(samAnisotropic, input.uv).r;
    float specularMapSample = gSpecularMap.Sample(samAnisotropic, input.uv).r;

    // Diffuse map
    baseColor *= gDiffuseMap.Sample(samAnisotropic, input.uv);

    // Normal map transformation
    float3 sampledNormalColor = gNormalMap.Sample(samAnisotropic, input.uv).xyz;
    float3x3 tangentToWorldMatrix = float3x3(input.tangent, cross(input.normal, input.tangent), input.normal);
    float3 worldNormal = mul(sampledNormalColor * 2.0f - 1.0f, tangentToWorldMatrix);

    // Lambertian diffuse reflection
    float4 lambertDiffuse = baseColor * (diffuseStrength / 3.16f);

    // Cosine law for observed area
    float observedArea = saturate(dot(worldNormal, gLightDirection));

    // Specular reflection calculation
    float3 reflectedRay = reflect(-gLightDirection, worldNormal);
    float cosAlpha = saturate(dot(reflectedRay, normalize(input.WorldPosition.xyz - gCameraPos)));
    float4 specularColor = specularIntensity * pow(cosAlpha, phongExponent) * float4(1, 1, 1, 1) * specularMapSample;

    // Final color calculation
    float4 finalColor = saturate((specularColor + lambertDiffuse) * observedArea + ambient);

    return finalColor;
}


technique11 DefaultTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Point()));
    }
    
    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Linear()));
    }
    
    pass P2
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Anisotropic()));
    }
}
