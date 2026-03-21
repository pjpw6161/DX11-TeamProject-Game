#include "Engine_Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture, g_MaskTexture, g_DissolveTexture;
vector g_vCamPos;
vector g_vColor;
int2 g_vNumSprite;
uint g_iIndex;
float g_fDissolveRatio;

struct VS_IN
{
    float3 vPos : Position;
    float2 vPSize : PSize;
    
    row_major matrix TransformMatrix : World;
};

struct VS_OUT
{
    vector vPos : Position;
    float2 vPSize : PSize;
};

VS_OUT VS_Main(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
    
    matrix matWV, matWVP;
    
    vector vPosition = mul(float4(Input.vPos, 1.f), Input.TransformMatrix);
    
    Output.vPos = mul(vPosition, g_WorldMatrix);
    Output.vPSize = float2(Input.vPSize.x * Input.TransformMatrix._11, Input.vPSize.y * Input.TransformMatrix._22);
    
    return Output;
}

struct GS_IN
{
    vector vPos : Position;
    float2 vPSize : PSize;
};

struct GS_OUT
{
    vector vPos : SV_Position;
    float2 vTex : Texcoord0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN Input[1], inout TriangleStream<GS_OUT> Triangles)
{
    GS_OUT Output[4];
    
    float3 vLook = (g_vCamPos - Input[0].vPos).xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * Input[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * Input[0].vPSize.y * 0.5f;
    
    Output[0].vPos = vector(Input[0].vPos.xyz + vRight + vUp, 1.f);
    Output[0].vTex = float2(0.f, 0.f);

    Output[1].vPos = vector(Input[0].vPos.xyz - vRight + vUp, 1.f);
    Output[1].vTex = float2(1.f, 0.f);

    Output[2].vPos = vector(Input[0].vPos.xyz - vRight - vUp, 1.f);
    Output[2].vTex = float2(1.f, 1.f);

    Output[3].vPos = vector(Input[0].vPos.xyz + vRight - vUp, 1.f);
    Output[3].vTex = float2(0.f, 1.f);
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    Output[0].vPos = mul(Output[0].vPos, matVP);
    Output[1].vPos = mul(Output[1].vPos, matVP);
    Output[2].vPos = mul(Output[2].vPos, matVP);
    Output[3].vPos = mul(Output[3].vPos, matVP);

    Triangles.Append(Output[0]);
    Triangles.Append(Output[1]);
    Triangles.Append(Output[2]);
    Triangles.RestartStrip();
    
    Triangles.Append(Output[0]);
    Triangles.Append(Output[2]);
    Triangles.Append(Output[3]);
}

struct PS_IN
{
    vector vPos : SV_Position;
    float2 vTex : Texcoord0;
};

struct PS_OUT
{
    vector vColor : SV_Target0;
};

PS_OUT PS_Main(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_Texture.Sample(LinearSampler, Input.vTex);
    vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex);
    
    Output.vColor.a = vMask.r;
    
    return Output;
}

PS_OUT PS_Main_Sprite(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float fDissolve = g_DissolveTexture.Sample(LinearSampler, Input.vTex).r;
    
    if (g_fDissolveRatio > fDissolve)
    {
        discard;
    }
    
    float2 vSpriteSize = float2(1.f, 1.f) / g_vNumSprite;
    int2 vSpriteCoord;
    vSpriteCoord.x = g_iIndex % g_vNumSprite.x;
    vSpriteCoord.y = g_iIndex / g_vNumSprite.x;
    float2 vUV = Input.vTex / g_vNumSprite + (vSpriteSize * vSpriteCoord);
    
    vector vMask = g_MaskTexture.Sample(LinearSampler, vUV);
    if (vMask.r < 0.1f)
    {
        discard;
    }

    Output.vColor = g_vColor;
    
    Output.vColor.a = vMask.r;
    
    return Output;
}

PS_OUT PS_Main_Color(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_vColor;
    vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex);
    if (vMask.r < 0.1f)
    {
        discard;
    }
    
    Output.vColor.a = vMask.r;
    
    return Output;
}

technique11 DefaultTechnique
{
    pass Particle_Texture_Mask
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main();
    }

    pass Particle_Sprite_Color
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite();
    }

    pass Particle_Color
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Color();
    }
};