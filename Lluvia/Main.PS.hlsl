Texture2D tex : register(t0);
SamplerState sampler_state_ : register(s0);

struct PSInput
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
};

struct PSOutput
{
    float4 color: SV_Target0;
};

PSOutput main(PSInput input)
{
    PSOutput output;

    output.color = tex.Sample(sampler_state_, input.texcoord);
    //output.color = float4(input.texcoord, 0.3, 1.0);

    return output;
}