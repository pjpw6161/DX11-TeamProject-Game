#include "Engine_Shader_Define.hlsli"

matrix g_ViewMatrix, g_ProjMatrix;
texture2D g_MaskTexture;
vector g_vColor;

float2 g_CamNF;

bool g_isBlur;

struct VS_IN
{
    float3 vTopPos : TopPosition;
    float3 vBottomPos : BottomPosition;
    float fAlpha : Alpha;
    float fTexcoordX : TexcoordX;
};

struct VS_OUT
{
    vector vTopPos : TopPosition;
    vector vBottomPos : BottomPosition;
    float fAlpha : Alpha;
    float fTexcoordX : TexcoordX;
};

VS_OUT VS_Main(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
    
    Output.vTopPos = vector(Input.vTopPos, 1.f);
    Output.vBottomPos = vector(Input.vBottomPos, 1.f);
    Output.fAlpha = Input.fAlpha;
    Output.fTexcoordX = Input.fTexcoordX;
    
    return Output;
}

struct GS_IN
{
    vector vTopPos : TopPosition;
    vector vBottomPos : BottomPosition;
    float fAlpha : Alpha;
    float fTexcoordX : TexcoordX;
};

struct GS_OUT
{
    vector vPos : SV_Position;
    float2 vTex : Texcoord0;
    float fAlpha : Alpha;
    float LinearZ : LINEARZ;
};

[maxvertexcount(6)]
void GS_Main(line GS_IN Input[2], inout TriangleStream<GS_OUT> Triangles)
{
    GS_OUT Output[8];
    
    Output[0].vPos = Input[0].vTopPos;
    Output[0].vTex = float2(Input[0].fTexcoordX, 0.f);
    Output[0].fAlpha = Input[0].fAlpha;
    

    Output[1].vPos = Input[1].vTopPos;
    Output[1].vTex = float2(Input[1].fTexcoordX, 0.f);
    Output[1].fAlpha = Input[1].fAlpha;

    Output[2].vPos = Input[1].vBottomPos;
    Output[2].vTex = float2(Input[1].fTexcoordX, 1.f);
    Output[2].fAlpha = Input[1].fAlpha;

    Output[3].vPos = Input[0].vBottomPos;
    Output[3].vTex = float2(Input[0].fTexcoordX, 1.f);
    Output[3].fAlpha = Input[0].fAlpha;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    ///////////////
    Output[0].vPos = mul(Output[0].vPos, matVP);
    Output[0].LinearZ = Output[0].vPos.w;
    
    ///////////////
    Output[1].vPos = mul(Output[1].vPos, matVP);
    Output[1].LinearZ = Output[1].vPos.w;
    
    ///////////////
    Output[2].vPos = mul(Output[2].vPos, matVP);
    Output[2].LinearZ = Output[2].vPos.w;
    
    ///////////////
    Output[3].vPos = mul(Output[3].vPos, matVP);
    Output[3].LinearZ = Output[3].vPos.w;

    Triangles.Append(Output[0]);
    Triangles.Append(Output[1]);
    Triangles.Append(Output[2]);
    Triangles.RestartStrip();
    
    Triangles.Append(Output[0]);
    Triangles.Append(Output[2]);
    Triangles.Append(Output[3]);
    Triangles.RestartStrip();
}

struct PS_IN
{
    vector vPos : SV_Position;
    float2 vTex : Texcoord0;
    float fAlpha : Alpha;
    float LinearZ : LINEARZ;
};

struct PS_OUT
{
    vector vColor : SV_Target0;
    vector vAlpha : SV_Target1;
    vector vBlur : SV_Target2;
};

struct PS_OUT_DISTORTION
{
    vector vDistortion : SV_Target0;
};

PS_OUT PS_Main_Color(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    //Output.vColor = g_MaskTexture.Sample(PointSampler, Input.vTex);
    
    //vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex);
    
    float3 Color = g_vColor.rgb;
    float fAlpha = Input.fAlpha;
    
    float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 4.f)), 1e-2, 3e3);
    fWeight = max(min(1.f, max(max(Color.r, Color.g), Color.b) * fAlpha), fAlpha) * fWeight;
    
    Output.vColor = vector(Color * fAlpha, fAlpha) * fWeight;
    Output.vAlpha = vector(fAlpha, fAlpha, fAlpha, fAlpha);
    Output.vBlur = vector(Color, fAlpha) * g_isBlur;
    
    //Output.vColor = g_vColor;
    //Output.vColor.a = Input.fAlpha;
    //Output.vBlurColor = g_vColor;
    //Output.vBlurColor.a = Input.fAlpha;
    
    return Output;
}

PS_OUT PS_Main_Mask(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex);
    
    float3 Color = g_vColor.rgb;
    float fAlpha = vMask.r * Input.fAlpha;
    
    float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 4.f)), 1e-2, 3e3);
    fWeight = max(min(1.f, max(max(Color.r, Color.g), Color.b) * fAlpha), fAlpha) * fWeight;
    
    Output.vColor = vector(Color * fAlpha, fAlpha) * fWeight;
    Output.vAlpha = vector(fAlpha, fAlpha, fAlpha, fAlpha);
    Output.vBlur = vector(Color, fAlpha) * g_isBlur;
    
    
    
    //Output.vColor = g_vColor;
    //Output.vColor.a = vMask.r * Input.fAlpha;
    
    //Output.vBlurColor = g_vColor;
        
    //Output.vBlurColor.a = vMask.r * Input.fAlpha;
    
    
    
    return Output;
}

PS_OUT_DISTORTION PS_Distortion(PS_IN Input)
{
    PS_OUT_DISTORTION Output = (PS_OUT_DISTORTION) 0;
    
    Output.vDistortion = normalize(g_MaskTexture.Sample(LinearSampler, Input.vTex)) * 0.03f * Input.fAlpha;
    
    return Output;
}

technique11 DefaultTechnique
{
    pass SingleColorSurface
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_Main();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Color();
    }

    pass MaskSurface
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_Main();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Mask();
    }

    pass Distortion
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_OnebyOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_Main();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Distortion();
    }
};