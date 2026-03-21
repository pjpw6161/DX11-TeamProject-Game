#include "Engine_Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_DeferredTexture;
Texture2D g_Luminance;

Texture2D g_HalfTex;
Texture2D g_QuarterTex;
Texture2D g_EightTex;

float Get_Luminance(float3 vRGB)
{
    return dot(vRGB, g_fLuminace);
}

struct VS_IN
{
    float3 vPos : Position;
    float2 vTexcoord : Texcoord0;
};

struct VS_OUT
{
    vector vPos : SV_Position;
    float2 vTexcoord : Texcoord0;
};

VS_OUT VS_Main(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
	
    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);

    Output.vPos = mul(vector(Input.vPos, 1.f), matWVP);
    Output.vTexcoord = Input.vTexcoord;
    
    return Output;
}

struct PS_IN
{
    vector vPos : SV_Position;
    float2 vTexcoord : Texcoord0;
};

struct PS_OUT
{
    vector vColor : SV_Target0;
};

PS_OUT PS_Main(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vHalfColor = g_HalfTex.Sample(LinearSampler, Input.vTexcoord);
    vector vQuarterColor = g_QuarterTex.Sample(LinearSampler, Input.vTexcoord);
    vector vEightColor = g_EightTex.Sample(LinearSampler, Input.vTexcoord);
    
    vector vColor = lerp(vHalfColor, vQuarterColor, 0.5f);
    vColor = lerp(vColor, vEightColor, 0.5f);
    
    Output.vColor = vColor;
    
    return Output;
}

float Get_BloomCurve(float x)
{
    float fResult = x;
    x *= 2.f;
    
    fResult = x * 0.05f + max(0.f, x - 1.26f) * 0.5f;
    
    return fResult * 0.5f;
}

PS_OUT PS_Main_Lum(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vHDRColor = g_DeferredTexture.Sample(PointSampler, Input.vTexcoord);
    
    float fLum = Get_Luminance(vHDRColor.rgb);
    
    float fAvgLum = g_Luminance.Sample(PointSampler, float2(0.f, 0.f)).r;
    
    float fColorScale = saturate(fLum - fAvgLum * Get_BloomCurve(fLum));
    
    //float fIntensity = dot(vHDRColor.rgb, g_fLuminace);
    //float fBloomIntensity = Get_BloomCurve(fIntensity);
    //float3 BloomColor = vHDRColor.rgb * fBloomIntensity / fIntensity;
    
    Output.vColor.rgb = vHDRColor.rgb * fColorScale;
    
    return Output;
}

technique11 DefaultTechnique
{
    pass BlurTex
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main();
    }

    pass BloomGetLum
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Lum();
    }
};