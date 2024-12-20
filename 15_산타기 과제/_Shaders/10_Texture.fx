matrix World, View, Projection;
Texture2D Map;

struct VertexInput
{
    float4 Position : Position;
	float2 Uv : Uv;
};

struct VertexOutput
{
    float4 Position : SV_Position;
	float2 Uv : Uv;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World); 
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

	output.Uv = input.Uv;
    
    return output;
}

SamplerState Samp;
float4 PS(VertexOutput input) : SV_Target
{
	return Map.Sample(Samp, input.Uv);
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}

//MIN_MAG_MIP
//min : 텍스쳐의 축소작업
//MAG : 텍스쳐의 확대작업
//MIP : 텍스쳐의 거리에 따른 작업