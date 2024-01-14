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
VS_OUTPUT VS_FireFX(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    output.Position = mul(float4(input.Position, 1.0f), worldViewProjection);
    
    output.uv = input.uv;

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

    float3 resultNormal = normalize(mul(sampledNormalColor, tangentSpaceAxis));


    float cosAngle = dot(resultNormal, normalize(-gLightDirection));
 	// lambert diffuse

    float4 sampleDiffuse = gDiffuseMap.Sample(samPoint, input.uv);
    float4 lambertDiffuse = sampleDiffuse * diffuseStrengthKd / gPI;

  
	// phong 
    float3 reflection = reflect(-normalize(gLightDirection), resultNormal);
    float cosAlpha = max(dot(reflection, input.ViewDirection), 0.0f);
    sampledSpecular = gSpecularMap.Sample(samPoint, input.uv);
    sampledPhongExponent = gGlossinessMap.Sample(samPoint, input.uv);

    float4 specularColor = sampledSpecular * pow(cosAlpha, sampledPhongExponent * gShininess);
   
    if (cosAngle < 0) return 0;
   
    return (lambertDiffuse + specularColor + ambientOcclusion) * cosAngle; 
};
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
    float4 lambertDiffuse = baseColor * (diffuseStrength / gPI);

    // Cosine law for observed area
    float observedArea = saturate(dot(worldNormal, gLightDirection));

    // Specular reflection calculation
    float3 reflectedRay = reflect(-gLightDirection, worldNormal);
    float3 viewDirection = input.ViewDirection;
    float cosAlpha = saturate(dot(reflectedRay, viewDirection));
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
    float4 lambertDiffuse = baseColor * (diffuseStrength / gPI);

    // Cosine law for observed area
    float observedArea = saturate(dot(worldNormal, gLightDirection));

    // Specular reflection calculation
    float3 reflectedRay = reflect(-gLightDirection, worldNormal);
    float3 viewDirection = input.ViewDirection;
    float cosAlpha = saturate(dot(reflectedRay, normalize(viewDirection)));
    float4 specularColor = specularIntensity * pow(cosAlpha, phongExponent) * float4(1, 1, 1, 1) * specularMapSample;

    // Final color calculation
    float4 finalColor = saturate((specularColor + lambertDiffuse) * observedArea + ambient);

    return finalColor;
}
float4 PS_FireFX(VS_OUTPUT input) : SV_TARGET
{
    float4 color = gDiffuseMap.Sample(samPoint, input.uv);
    return color;
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

