cbuffer MatrixBuffer
{
	matrix matrWorld;
	matrix matrView;
	matrix matrProjection;
};

struct SVertexInput
{
	float4 position : POSITION;
	float2 uvMain	: TEXCOORD0;
	float3 normal	: NORMAL;
};

struct SVertexOutput
{
	float4 position	: SV_POSITION;
	float2 uvMain	: TEXCOORD0;
	float3 normal	: NORMAL;
};


SVertexOutput vsMain(SVertexInput input)
{
	SVertexOutput output;

	input.position.w = 1.0f;

	output.position = mul(input.position, matrWorld);
	output.position = mul(output.position, matrView);
	output.position = mul(output.position, matrProjection);

	output.uvMain = input.uvMain;
	output.normal = input.normal;

	return output;
}