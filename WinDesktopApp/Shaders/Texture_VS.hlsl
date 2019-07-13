cbuffer MatrixBuffer
{
	matrix matrWorld;
	matrix matrView;
	matrix matrProjection;
};

struct VertexInput
{
	float4 position	: POSITION;
	float2 uvMain	: TEXCOORD0;
};

struct VertexOutput
{
	float4 position	: SV_POSITION;
	float2 uvMain	: TEXCOORD0;
};

VertexOutput vsMain(VertexInput input)
{
	VertexOutput output;

	input.position.w = 1.f;
	output.position = mul(input.position, matrWorld);
	output.position = mul(output.position, matrView);
	output.position = mul(output.position, matrProjection);

	output.uvMain = input.uvMain;
	
	return output;
}