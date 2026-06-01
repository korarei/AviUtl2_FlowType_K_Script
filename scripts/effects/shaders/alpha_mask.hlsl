Texture2D tex : register(t0);
cbuffer params : register(b0) {
    float should_invert;
}

float4
alpha_mask(float4 pos : SV_Position) : SV_Target {
    float a = tex.Load(int3(pos.xy, 0)).a;
    a = lerp(a, 1.0 - a, should_invert);
    return a;
}
