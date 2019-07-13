struct PixelInput
{
	float4 position	: SV_POSITION;
	float2 uvMain	: TEXCOORD0;
};

Texture2D shaderTexture;
SamplerState SamplerType;

float4 psMain(PixelInput input)	: SV_TARGET
{
	return shaderTexture.Sample(SamplerType, input.uvMain);
}