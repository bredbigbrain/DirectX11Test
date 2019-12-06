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
	float3 color	: COLOR;
};

struct SVertexOutput
{
	float4 position	: SV_POSITION;
	float2 uvMain	: TEXCOORD0;
	float3 normal	: NORMAL;
	float4 color	: COLOR;
};


SVertexOutput vsMain(SVertexInput input)
{
	SVertexOutput output;

	input.position.w = 1.0f;

	output.position = mul(input.position, matrWorld);
	output.position = mul(output.position, matrView);
	output.position = mul(output.position, matrProjection);

	output.uvMain = input.uvMain;
	//output.normal = normalize(mul(input.normal, (float3x3)matrWorld));
	output.normal = input.normal;
	output.color = float4(input.color, 1.0f);

	return output;
}