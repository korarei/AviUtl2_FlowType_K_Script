cbuffer params : register(b0) {
    column_major float3x3 xform;
    float2 pivot;
    float aspect;
}

float4
mask(float4 pos : SV_Position, float2 uv : TEXCOORD) : SV_Target {
    uv -= 0.5;
    uv.x *= aspect;
    uv = mul(xform, float3(uv + pivot, 1.0)).xy;
    uv.x *= rcp(aspect);
    uv += 0.5;
    const float2 aa = fwidth(uv);
    const float2 mask = smoothstep(-aa, aa, uv) * smoothstep(1.0 + aa, 1.0 - aa, uv);
    return mask.x * mask.y;
}
