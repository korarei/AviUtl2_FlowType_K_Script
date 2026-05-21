Texture2D tex : register(t0);
cbuffer params : register(b0) {
    float2 origin;
}

float4
blit(float4 pos : SV_Position) : SV_Target {
    const int2 loc = int2(pos.xy + origin);
    const int2 mask = step(0, loc);
    return tex.Load(int3(max(loc, 0), 0)) * mask.x * mask.y;
}
