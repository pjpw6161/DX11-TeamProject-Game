#include "Engine_Shader_Define.hlsli"

matrix g_ViewMatrix, g_ProjMatrix;
texture2D g_MaskTexture;
vector g_vCamPos;

bool g_isBlur;


struct VS_IN
{
    float3 vPos : Position;
    float2 vPSize : PSize;
    vector vColor : Color;
};

struct VS_OUT
{
    vector vPos : Position;
    float2 vPSize : PSize;
    vector vColor : Color;
};

VS_OUT VS_Main(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
    
    Output.vPos = vector(Input.vPos, 1.f);
    Output.vPSize = Input.vPSize;
    Output.vColor = Input.vColor;
    
    return Output;
}

struct GS_IN
{
    vector vPos : Position;
    float2 vPSize : PSize;
    vector vColor : Color;
};

struct GS_OUT
{
    vector vPos : SV_Position;
    float2 vTex : Texcoord0;
    vector vColor : Color;
    float LinearZ : LINEARZ;
};

[maxvertexcount(12)]
void GS_Main(line GS_IN Input[2], inout TriangleStream<GS_OUT> Triangles)
{
    GS_OUT Output[8];
    
    float3 vLook = (g_vCamPos - Input[0].vPos).xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * Input[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * Input[0].vPSize.y * 0.5f;
    
    float3 vNextLook = (g_vCamPos - Input[1].vPos).xyz;
    float3 vNextRight = normalize(cross(float3(0.f, 1.f, 0.f), vNextLook)) * Input[1].vPSize.x * 0.5f;
    float3 vNextUp = normalize(cross(vNextLook, vNextRight)) * Input[1].vPSize.y * 0.5f;
    
    Output[0].vPos = vector(Input[0].vPos.xyz + vRight + vUp, 1.f);
    Output[0].vTex = float2(0.f, 0.f);
    Output[0].vColor = Input[0].vColor;

    Output[1].vPos = vector(Input[0].vPos.xyz - vRight + vUp, 1.f);
    Output[1].vTex = float2(1.f, 0.f);
    Output[1].vColor = Input[0].vColor;

    Output[2].vPos = vector(Input[0].vPos.xyz - vRight - vUp, 1.f);
    Output[2].vTex = float2(1.f, 1.f);
    Output[2].vColor = Input[0].vColor;

    Output[3].vPos = vector(Input[0].vPos.xyz + vRight - vUp, 1.f);
    Output[3].vTex = float2(0.f, 1.f);
    Output[3].vColor = Input[0].vColor;
    
    Output[4].vPos = vector(Input[1].vPos.xyz + vNextRight + vNextUp, 1.f);
    Output[4].vTex = float2(1.f, 0.f);
    Output[4].vColor = Input[1].vColor;

    Output[5].vPos = vector(Input[1].vPos.xyz - vNextRight + vNextUp, 1.f);
    Output[5].vTex = float2(1.f, 1.f);
    Output[5].vColor = Input[1].vColor;

    Output[6].vPos = vector(Input[1].vPos.xyz - vNextRight - vNextUp, 1.f);
    Output[6].vTex = float2(1.f, 0.f);
    Output[6].vColor = Input[1].vColor;

    Output[7].vPos = vector(Input[1].vPos.xyz + vNextRight - vNextUp, 1.f);
    Output[7].vTex = float2(1.f, 1.f);
    Output[7].vColor = Input[1].vColor;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    Output[0].vPos = mul(Output[0].vPos, matVP);
    Output[0].LinearZ = Output[0].vPos.w;
    
    Output[1].vPos = mul(Output[1].vPos, matVP);
    Output[1].LinearZ = Output[1].vPos.w;
    
    Output[2].vPos = mul(Output[2].vPos, matVP);
    Output[2].LinearZ = Output[2].vPos.w;
    
    Output[3].vPos = mul(Output[3].vPos, matVP);
    Output[3].LinearZ = Output[3].vPos.w;
    
    Output[4].vPos = mul(Output[4].vPos, matVP);
    Output[4].LinearZ = Output[4].vPos.w;
    
    Output[5].vPos = mul(Output[5].vPos, matVP);
    Output[5].LinearZ = Output[5].vPos.w;
    
    Output[6].vPos = mul(Output[6].vPos, matVP);
    Output[6].LinearZ = Output[6].vPos.w;
    
    Output[7].vPos = mul(Output[7].vPos, matVP);
    Output[7].LinearZ = Output[7].vPos.w;

    Triangles.Append(Output[0]);
    Triangles.Append(Output[4]);
    Triangles.Append(Output[6]);
    Triangles.RestartStrip();
    
    Triangles.Append(Output[0]);
    Triangles.Append(Output[6]);
    Triangles.Append(Output[2]);
    Triangles.RestartStrip();
    
    Triangles.Append(Output[1]);
    Triangles.Append(Output[5]);
    Triangles.Append(Output[7]);
    Triangles.RestartStrip();
    
    Triangles.Append(Output[1]);
    Triangles.Append(Output[7]);
    Triangles.Append(Output[3]);
    Triangles.RestartStrip();
}

struct PS_IN
{
    vector vPos : SV_Position;
    float2 vTex : Texcoord0;
    vector vColor : Color;
    float LinearZ : LINEARZ;
};

struct PS_OUT
{
    vector vColor : SV_Target0;
    vector vAlpha : SV_Target1;
    vector vBlur : SV_Target2;
};

PS_OUT PS_Main(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    //Output.vColor = g_MaskTexture.Sample(PointSampler, Input.vTex);
    
    //vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex);
    
    float3 Color = Input.vColor.rgb;
    float fAlpha = Input.vColor.a;
    
    float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 4.f)), 1e-2, 3e3);
    fWeight = max(min(1.f, max(max(Color.r, Color.g), Color.b) * fAlpha), fAlpha) * fWeight;
    
    Output.vColor = vector(Color * fAlpha, fAlpha) * fWeight;
    Output.vAlpha = vector(fAlpha, fAlpha, fAlpha, fAlpha);
    Output.vBlur = vector(Color, fAlpha) * g_isBlur;
    
    return Output;
}

technique11 DefaultTechnique
{
    pass SingleColorTrail
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_Main();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main();
    }
};