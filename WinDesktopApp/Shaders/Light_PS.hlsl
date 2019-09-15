
cbuffer LightBuffer
{
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
};

Texture2D mainTex;
SamplerState mainSampler;

struct SPixelInput
{
	float4 position	: SV_POSITION;
	float2 uvMain	: TEXCOORD0;
	float3 normal	: NORMAL;
};


float4 psMain(SPixelInput input) : SV_TARGET
{
	float4 texColor = mainTex.Sample(mainSampler, input.uvMain);
	float lightIntensity = saturate(dot(input.normal, -lightDirection));
	float4 color = saturate(diffuseColor * lightIntensity) * texColor;
	return color;
}