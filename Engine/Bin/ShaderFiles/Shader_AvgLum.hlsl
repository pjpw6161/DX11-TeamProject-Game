
Texture2D<vector> InputTexture : register(t0); // 1280 720
RWTexture2D<float> Tex4x3 : register(u0); // 320 240

[numthreads(8, 6, 1)]
void DownScaleFirst(uint3 DTID : SV_DispatchThreadID)
{
    uint2 vPixel = uint2(DTID.x, DTID.y);
    
    float avgLum = 0.f;
    
    uint2 inPixel = uint2(vPixel.x * 4, vPixel.y * 3);
    
    vector vDownScaled = 0;
    
    float3 LUM_FACTOR = float3(0.2125f, 0.7154f, 0.0721f);
    
    [unroll]
    for (uint i = 0; i < 3; ++i)
    {
        [unroll]
        for (uint j = 0; j < 4; ++j)
        {
            vDownScaled += dot(InputTexture[inPixel.xy + uint2(j, i)].rgb, LUM_FACTOR);
        }
    }
    
    vDownScaled /= 12.f;
    
    Tex4x3[vPixel] = avgLum;
}

Texture2D<float> Input4x3Tex : register(t1); // 320 240
RWTexture2D<float> Tex4x4 : register(u1); // 80 60

[numthreads(4, 4, 1)]
void DownScaleSecond(uint3 DTID : SV_DispatchThreadID)
{
    uint2 vPixel = uint2(DTID.x, DTID.y);
    
    uint2 inPixel = uint2(vPixel * 4);
    
    float DownScaled = 0.f;
    
    [unroll]
    for (uint i = 0; i < 4; ++i)
    {
        [unroll]
        for (uint j = 0; j < 4; ++j)
        {
            DownScaled += Input4x3Tex[inPixel.xy + uint2(j, i)];
        }
    }
    DownScaled /= 16.f;
    
    Tex4x4[DTID.xy] = DownScaled;
}

Texture2D<float> Input80x60Tex : register(t2); // 80 60
RWTexture2D<float> Tex20x20 : register(u2); // 20 20

[numthreads(4, 4, 1)]
void DownScaleThird(uint3 DTID : SV_DispatchThreadID)
{
    uint2 vPixel = uint2(DTID.x, DTID.y);
    
    uint2 inPixel = uint2(vPixel.x * 4, vPixel.y * 3);
    
    float DownScaled = 0.f;
    
    [unroll]
    for (uint i = 0; i < 3; ++i)
    {
        [unroll]
        for (uint j = 0; j < 4; ++j)
        {
            DownScaled += Input80x60Tex[inPixel.xy + uint2(j, i)];
        }
    }
    
    DownScaled /= 12.f;
    
    Tex20x20[DTID.xy] = DownScaled;

}

Texture2D<float> Input20x20Tex : register(t3); // 20 20
RWTexture2D<vector> Tex2x2 : register(u3); // 4 4

[numthreads(2, 2, 1)]
void DownScaleFourth(uint3 DTID : SV_DispatchThreadID)
{
    uint2 vPixel = uint2(DTID.x, DTID.y);
    
    uint2 inPixel = uint2(vPixel.x * 10, vPixel.y * 10);
    
    float DownScaled = 0.f;
    
    [unroll]
    for (uint i = 0; i < 5; ++i)
    {
        [unroll]
        for (uint j = 0; j < 5; ++j)
        {
            DownScaled += Input20x20Tex[inPixel.xy + uint2(j, i)];
        }
    }
    
    DownScaled /= 25.f;
    
    Tex2x2[DTID.xy] = vector(DownScaled, DownScaled, DownScaled, 1.f);

}

Texture2D<float> Input2x2Tex : register(t4); // 4 4
RWTexture2D<float> AvgLum : register(u4); // 1 1

[numthreads(1, 1, 1)]
void LastDownScaled(uint3 DTID : SV_DispatchThreadID)
{
    float avgLum = 0.f;
    
    for (uint i = 0; i < 4; ++i)
    {
        for (uint j = 0; j < 4; ++j)
        {
            avgLum += Input2x2Tex[uint2(j, i)];
        }
    }
    
    avgLum /= 16.f;
    
    AvgLum[DTID.xy] = max(avgLum, 0.0001f);
}