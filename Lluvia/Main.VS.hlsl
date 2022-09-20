struct VSInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

struct VSOutput
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
};

VSOutput main(VSInput input)
{
    VSOutput output;

    output.position = input.position;
    output.texcoord = input.texcoord;

    return output;
}