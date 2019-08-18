Texture2D shaderTexture;
SamplerState SampleType;

cbuffer PixelBuffer
{
	float4 pixelColor;
};

struct PixelInput
{
	float4 position	: SV_POSITION;
	float2 uvMain	: TEXCOORD0;
};

float4 psMain(PixelInput input) : SV_TARGET
{
	float4 color = shaderTexture.Sample(SampleType, input.uvMain);
	
	// If the color is black on the texture then treat this pixel as transparent.
	if(color.r == 0.0f)
	{
		color.a = 0.0f;
	}
	
	// If the color is other than black on the texture then this is a pixel in the font so draw it using the font pixel color.
	else
	{
		color.a = 1.0f;
		color = color * pixelColor;
	}
	
	return color;
}