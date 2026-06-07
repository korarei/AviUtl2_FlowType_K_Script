Texture2D tex[2] : register(t0);
cbuffer params : register(b0) {
    float2 origin;
    float idx;
}

float4
color_mask(float4 pos : SV_Position) : SV_Target {
    const int3 loc = int3(origin + pos.xy, 0);

    const float3 color = tex[1].Load(loc).rgb * 63.75;
    const float mask = color.r + color.g * 64.0 + color.b * 4096.0;

    return tex[0].Load(loc) * step(abs(mask - idx), 0.5);
}
