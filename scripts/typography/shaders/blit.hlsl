Texture2D tex : register(t0);
cbuffer params : register(b0) {
    float2 origin;
}

float4
blit(float4 pos : SV_Position) : SV_Target {
    return tex.Load(int3(int2(pos.xy + origin), 0));
}
