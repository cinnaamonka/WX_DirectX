float4x4 worldViewProjection : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD0;  // Add texture coordinates
    float3 Color : COLOR;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;  // Pass texture coordinates to the pixel shader
    float3 Color : COLOR;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Position = mul(float4(input.Position, 1.f), worldViewProjection);
    output.TexCoord = input.TexCoord;  // Pass texture coordinates
    output.Color = input.Color;
    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    // Use the texture coordinates for sampling a texture
    float4 textureColor = input.TexCoord;
    
    // Multiply the texture color with the input color
    return float4(input.Color * textureColor.rgb, textureColor.a);
}

technique11 DefaultTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AdressU = Wrap;// Or Mirror,Clamp,Border
    AdressV = Wrap;

};
