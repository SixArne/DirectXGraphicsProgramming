float4x4 gWorldViewProj: WorldViewProjection;
Texture2D gDiffuseMap: DiffuseMap;

struct VS_INPUT
{
	float3 Position: POSITION;
	float3 Color: COLOR;
	float2 Uv: TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Position: SV_POSITION;
	float3 Color: COLOR0;
	float2 Uv: TEXCOORD0;
};

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;// or Mirror, Clamp, Border
	AddressV = Wrap;
};

/// Vertex shader
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = float4(input.Position, 1.f);
	output.Position = mul( output.Position, gWorldViewProj);
	output.Color = input.Color;
	output.Uv = input.Uv;

	return output;
}

// Pixel shader
float4 PS(VS_OUTPUT input): SV_TARGET
{
	float3 color = gDiffuseMap.Sample(samPoint, input.Uv) * input.Color;
	return float4(color, 1.f);
}

// Technique
technique11 DefaultTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}