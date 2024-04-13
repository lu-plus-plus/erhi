struct VSInput
{
    [[vk::location(0)]] float4 position : POSITION;
    [[vk::location(1)]] float4 color : COLOR;
    uint vertexIndex : SV_VertexID;
};

struct Payload
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

Payload VSMain(VSInput input) {
    Payload output;

    output.position = input.position;
    output.color = input.color;

    return output;
}

float4 PSMain(Payload input) : SV_TARGET
{
    return input.color;
}
