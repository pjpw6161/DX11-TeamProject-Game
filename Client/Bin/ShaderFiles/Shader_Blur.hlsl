#define GAUSSIAN_RADIUS 7

Texture2D<float4> inputTexture : register(t0);
RWTexture2D<float4> outputTexture : register(u0);


static float Gaussian[15] =
{
    0.00006103515625f,
    0.0008544921875f,
    0.00555419921875f,
    0.022216796875f,
    0.06109619140625f,
    0.1221923828125f,
    0.18328857421875f,
    0.20947265625f,
    0.18328857421875f,
    0.1221923828125f,
    0.06109619140625f,
    0.022216796875f,
    0.00555419921875f,
    0.0008544921875f,
    0.00006103515625f
};

cbuffer BlurParams : register(b0)
{
    // = float coefficients[GAUSSIAN_RADIUS + 1]
    // radius <= GAUSSIAN_RADIUS, direction 0 = horizontal, 1 = vertical
    int2 radiusAndDirection;
    uint2 iWinSize;
    
    float fBlurPower;
    uint3 Padding;
}

[numthreads(8, 8, 1)]
void Blur(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchID : SV_DispatchThreadID)
{
    int2 pixel = int2(dispatchID.x, dispatchID.y);

    if (pixel.x >= iWinSize.x || pixel.y >= iWinSize.y)
        return;
    
    int radius = radiusAndDirection.x;
    int2 dir = int2(1 - radiusAndDirection.y, radiusAndDirection.y);

    float4 accumulatedValue = float4(0.0, 0.0, 0.0, 0.0);
        
        //float Gaussian[15] = { 0.000067f, 0.000437f, 0.002216f, 0.008765f, 0.027001f, 0.064764f, 0.121012f, 0.199500f, 0.121012f, 0.064764f, 0.027001f, 0.008765f, 0.002216f, 0.000437f, 0.000067f };
    for (int i = -radius; i <= radius; ++i)
    {
        uint cIndex = (uint) (i + radius);
            //accumulatedValue += coefficients[cIndex >> 2][cIndex & 3] * inputTexture[mad(i, dir, pixel)];
        int2 iIndex = mad(i, dir, pixel);
        int3 i3Index = int3(iIndex, 0);
        accumulatedValue += inputTexture.Load(i3Index) * Gaussian[cIndex] * fBlurPower;
    }
        
    outputTexture[pixel] = accumulatedValue;
}

cbuffer RadialParams : register(b1)
{
    float2 g_vCenterTexPos;
    float g_fRadialBlur_Power;
    uint g_isTex;
    
    vector g_vCenterPos;
}

static float fSize[15] = { -0.07f, -0.06f, -0.05f, -0.04f, -0.03f, -0.02f, -0.01f, 0.f, 0.01f, 0.02f, 0.03f, 0.04f, 0.05f, 0.06f, 0.07f };

[numthreads(8,8,1)]
void Radial_Blur(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchID : SV_DispatchThreadID)
{
    int2 pixel = int2(dispatchID.x, dispatchID.y);

    if (pixel.x >= 1280 || pixel.y >= 720)
        return;
    
    float2 UV;
    UV.x = pixel.x / (float) 1280;
    UV.y = pixel.y / (float) 720;
    
    float2 Dir;
    if (1 == g_isTex)
    {
        Dir = g_vCenterTexPos - UV;
    }
    else
    {
        float2 vPos = g_vCenterPos.xy;
        
        vPos.x = (vPos.x + 1.f) * 0.5f;
        vPos.y = (vPos.y - 1.f) * -0.5f;
        
        Dir = vPos - UV;
    }
    
    float2 DirMulti = Dir * g_fRadialBlur_Power;
    
    float2 vTex;
    vector vColor = 0.f;
    for (uint i = 0; i < 15; ++i)
    {
        vTex = (DirMulti * fSize[i]) + UV;
        vTex.x = vTex.x * (float) 1280;
        vTex.y = vTex.y * (float) 720;
        
        int3 iIndex;
        iIndex.x = (int) vTex.x;
        iIndex.y = (int) vTex.y;
        iIndex.z = 0;
        vColor += inputTexture.Load(iIndex) * Gaussian[i];
    }

    outputTexture[pixel] = vColor;

}

Texture2D<float4> Depth_Velocity_Texture : register(t1);


[numthreads(8, 8, 1)]
void Motion_Blur(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchID : SV_DispatchThreadID)
{
    int2 pixel = int2(dispatchID.x, dispatchID.y);

    if (pixel.x >= 1280 || pixel.y >= 720)
        return;
    
    uint iNumBlurSample = 10;
    
    int3 iCurIndex = int3(pixel, 0);
    vector vDepth_Velocity_Desc = Depth_Velocity_Texture.Load(iCurIndex);
    
    float2 Velocity = vDepth_Velocity_Desc.zw / (float) iNumBlurSample;
    
    float Depth = vDepth_Velocity_Desc.x;
    
    float2 vTex;
    vTex.x = pixel.x / (float) 1280;
    vTex.y = pixel.y / (float) 720;
    
    uint iCnt = 1;
    
    vector BColor = 0.f;
    float BDepth = 0.f;
    
    vector vColor = inputTexture.Load(iCurIndex);
    
    for (uint i = iCnt; i < iNumBlurSample; ++i)
    {
        float2 UV = vTex + Velocity * (float) i;
        UV.x = UV.x * (float) 1280;
        UV.y = UV.y * (float) 720;
        
        int3 iIndex;
        iIndex.x = (int) UV.x;
        iIndex.y = (int) UV.y;
        iIndex.z = 0;
        BColor = inputTexture.Load(iIndex);
        BDepth = Depth_Velocity_Texture.Load(iIndex).x;
        
        if(Depth < BDepth + 0.04f)
        {
            iCnt++;
            vColor += BColor;
        }
    }

    vColor /= (float) iCnt;
    
    outputTexture[pixel] = vColor;
}