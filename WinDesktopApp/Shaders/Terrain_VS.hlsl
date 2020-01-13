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
	float3 tangent	: TANGENT;
	float3 binormal	: BINORMAl;
	float3 color	: COLOR;
};

struct SVertexOutput
{
	float4 position	: SV_POSITION;
	float2 uvMain	: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	float3 binormal	: BINORMAl;
	float4 color	: COLOR;
};


SVertexOutput vsMain(SVertexInput input)
{
	SVertexOutput output;

	input.position.w = 1.0f;

	output.position = mul(input.position, matrWorld);
	output.position = mul(output.position, matrView);
	output.position = mul(output.position, matrProjection);

	output.normal = input.normal;
	output.tangent = input.tangent;
	output.binormal = input.binormal;

	output.uvMain = input.uvMain;
	/*output.normal = normalize(mul(input.normal, (float3x3)matrWorld));
	output.tangent = normalize(mul(input.tangent, (float3x3)matrWorld));
	output.binormal = normalize(mul(input.binormal, (float3x3)matrWorld));*/
	output.color = float4(input.color, 1.0f);

	return output;
}