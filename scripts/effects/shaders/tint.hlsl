Texture2D tex : register(t0);
cbuffer params : register(b0) {
    float4 col;
    float t;
}

float4
tint(float4 pos : SV_Position) : SV_Target {
    const float4 src = tex.Load(int3(pos.xy, 0));
    return lerp(src, col * src.a, t);
}
