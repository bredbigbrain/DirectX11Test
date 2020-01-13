
cbuffer LightBuffer
{
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
};

Texture2D mainTex;
Texture2D normalTex;
SamplerState mainSampler;

struct SPixelInput
{
	float4 position	: SV_POSITION;
	float2 uvMain	: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	float3 binormal	: BINORMAl;
	float4 color	: COLOR;
};


float4 psMain(SPixelInput input) : SV_TARGET
{
	float4 texColor = mainTex.Sample(mainSampler, input.uvMain);
	texColor = saturate(texColor * input.color * 2);
	//texColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float4 texNormal = normalTex.Sample(mainSampler, input.uvMain);
	texNormal = (texNormal * 2.0f) - 1.0f;
	//texNormal = normalize((texNormal * 2.0f) - 1.0f);

	float3 bumpNormal = (texNormal.x * input.tangent) + (texNormal.y * input.binormal) + (texNormal.z * input.normal);
	bumpNormal = normalize(bumpNormal);
	
	float lightIntensity = saturate(dot(bumpNormal, -lightDirection));
	float4 color = saturate(diffuseColor * lightIntensity) * texColor;
	return color;
}