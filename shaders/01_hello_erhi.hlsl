struct Payload
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

Payload VSMain(
    [[vk::location(0)]] float4 position : POSITION,
    [[vk::location(1)]] float4 color : COLOR) {
    
    Payload result;

    result.position = position;
    result.color = color;

    return result;
}

float4 PSMain(Payload input) : SV_TARGET
{
    return input.color;
}
