
Texture2D<float4> InputTexture : register(t0);
RWTexture2D<float4> OutputTexture : register(u0);

cbuffer WinSize : register(b0)
{
    uint2 iWinSize;
    uint2 Padding;
}

[numthreads(8, 8, 1)]
void Downsample(uint3 GroupID : SV_GroupID, uint3 GroupThreadID : SV_GroupThreadID, 
uint GroupIndex : SV_GroupIndex, uint3 DispatchID : SV_DispatchThreadID)
{
    uint2 Pixel = uint2(DispatchID.x, DispatchID.y);
    
    if (Pixel.x >= iWinSize.x || Pixel.y >= iWinSize.y)
        return;
    
    uint2 inPixel = Pixel * 2;
    
    float4 hIntensity0 = lerp(InputTexture[inPixel], InputTexture[inPixel + uint2(1, 0)], 0.5f);
    float4 hIntensity1 = lerp(InputTexture[inPixel + uint2(0, 1)], InputTexture[inPixel + uint2(1, 1)], 0.5f);
    float4 Intensity = lerp(hIntensity0, hIntensity1, 0.5f);
    
    OutputTexture[Pixel] = Intensity;
}
