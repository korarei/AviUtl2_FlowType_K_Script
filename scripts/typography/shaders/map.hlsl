Texture2D tex[2] : register(t0);
SamplerState smp : register(s0);
cbuffer params : register(b0) {
    float2 uv;
}

float4
map(float4 pos : SV_Position) : SV_Target {
    float2 size;
    tex[1].GetDimensions(size.x, size.y);

    const float4 base = tex[0].Load(int3(pos.xy, 0));
    const float4 src = tex[1].Sample(smp, mad(uv, size - 1.0, 0.5) * rcp(size));
    const float4 blended = mad(1.0 - src.a, base, src);
    return float4(blended.rgb * rcp(blended.a) * base.a, base.a);
}
