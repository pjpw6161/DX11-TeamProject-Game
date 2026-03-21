#include "Engine_Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_MaskTexture;
texture2D g_DissolveTexture;

matrix g_OldViewMatrix;

vector g_vColor;
float g_fAlpha;

vector g_vCamPos;
float2 g_CamNF;
float g_fLightFar;
float g_fDissolveRatio;

bool g_HasNorTex;
bool g_HasMaskTex;
bool g_bSelected = false;

float4 g_vClipPlane;

struct VS_IN
{
    float3 vPos : Position;
    float3 vNor : Normal;
    float2 vTex : Texcoord0;
    float3 vTan : Tangent;
    row_major matrix mWorld : World;
    
    int iID : ID;
};

struct VS_OUT
{
    vector vPos : SV_Position; // == float4
    vector vNor : Normal;
    float2 vTex : Texcoord0;
    vector vWorldPos : Texcoord1;
    vector vProjPos : Texcoord2;
    float3 vTangent : Tangent;
    float3 vBinormal : Binormal;
    float2 vDir : DIRECTION;
};

VS_OUT VS_Main(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
	
    matrix matWV, matWVP;
    
    matWV = mul(Input.mWorld, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    vector vNewPos = mul(vector(Input.vPos, 1.f), matWVP);
    
    matrix matOldWV, matOldWVP;
    
    matOldWV = mul(Input.mWorld, g_OldViewMatrix);
    matOldWVP = mul(matOldWV, g_ProjMatrix);
    
    vector vOldPos = mul(vector(Input.vPos, 1.f), matOldWVP);
    
    float2 Velocity = (vNewPos.xy / vNewPos.w) - (vOldPos.xy / vOldPos.w);
    
    float2 vCalDir;
    vCalDir = Velocity * 0.5f;
    vCalDir.y *= -1.f;
    
    Output.vPos = vNewPos;
    Output.vNor = normalize(mul(vector(Input.vNor, 0.f), Input.mWorld));
    Output.vTex = Input.vTex;
    Output.vWorldPos = mul(vector(Input.vPos, 1.f), Input.mWorld);
    Output.vProjPos = Output.vPos;
    Output.vTangent = normalize(mul(vector(Input.vTan, 0.f), Input.mWorld)).xyz;
    Output.vBinormal = normalize(cross(Output.vNor.xyz, Output.vTangent));
    Output.vDir = vCalDir;
    
    return Output;
}

struct VS_WATER_OUT
{
    vector vPos : SV_Position; // == float4
    vector vNor : Normal;
    float2 vTex : Texcoord0;
    vector vWorldPos : Texcoord1;
    vector vProjPos : Texcoord2;
    float3 vTangent : Tangent;
    float3 vBinormal : Binormal;
    
    float fClip : SV_ClipDistance0;
};

VS_WATER_OUT VS_Main_Water(VS_IN Input)
{
    VS_WATER_OUT Output = (VS_WATER_OUT) 0;
    
    matrix matWV, matWVP;
    
    matWV = mul(Input.mWorld, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Output.vPos = mul(vector(Input.vPos, 1.f), matWVP);
    Output.vNor = normalize(mul(vector(Input.vNor, 0.f), Input.mWorld));
    Output.vTex = Input.vTex;
    Output.vWorldPos = mul(vector(Input.vPos, 1.f), Input.mWorld);
    Output.vProjPos = Output.vPos;
    Output.vTangent = normalize(mul(vector(Input.vTan, 0.f), Input.mWorld)).xyz;
    Output.vBinormal = normalize(cross(Output.vNor.xyz, Output.vTangent));
    
    Output.fClip = dot(mul(vector(Input.vPos, 1.f), Input.mWorld), g_vClipPlane);
    
    return Output;

}

struct VS_SHADOW_OUT
{
    vector vPos : Position; // == float4
    float2 vTex : Texcoord0;
};

VS_SHADOW_OUT VS_Shadow(VS_IN Input)
{
    VS_SHADOW_OUT Output = (VS_SHADOW_OUT) 0;
    
    Output.vPos = mul(vector(Input.vPos, 1.f), Input.mWorld);
    Output.vTex = Input.vTex;
    
    return Output;
}


struct GS_SHADOW_IN
{
    vector vPos : Position; // == float4
    float2 vTex : Texcoord0;
};

struct GS_SHADOW_OUT
{
    vector vPos : SV_Position;
    float2 vTex : Texcoord0;
    uint RTIndex : SV_RenderTargetArrayIndex;
};

matrix g_CascadeView[3];
matrix g_CascadeProj[3];

[maxvertexcount(9)]
void GS_Shadow(triangle GS_SHADOW_IN Input[3], inout TriangleStream<GS_SHADOW_OUT> Output)
{
    
    for (uint Face = 0; Face < 3; ++Face)
    {
        GS_SHADOW_OUT Elements = (GS_SHADOW_OUT) 0;
        
        Elements.RTIndex = Face;
        matrix matVP = mul(g_CascadeView[Face], g_CascadeProj[Face]);
        
        for (uint i = 0; i < 3; ++i)
        {
            Elements.vPos = mul(Input[i].vPos, matVP);

            Elements.vTex = Input[i].vTex;
            Output.Append(Elements);
        }
        Output.RestartStrip();
    }
}


struct PS_IN
{
    vector vPos : SV_Position;
    vector vNor : Normal;
    float2 vTex : Texcoord0;
    vector vWorldPos : Texcoord1;
    vector vProjPos : Texcoord2;
    float3 vTangent : Tangent;
    float3 vBinormal : Binormal;
    float2 vDir : DIRECTION;
    
};

struct PS_OUT_DEFERRED
{
    vector vDiffuse : SV_Target0;
    vector vNormal : SV_Target1;
    vector vDepth : SV_Target2;
    vector vRimMask : SV_Target3;
};

struct PS_OUT
{
    vector vColor : SV_Target0;

};

PS_OUT_DEFERRED PS_Main(PS_IN Input)
{
    PS_OUT_DEFERRED Output = (PS_OUT_DEFERRED) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex) + 0.3f * g_bSelected;
    
    float3 vNormal;
    if (g_HasNorTex)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, Input.vTex);
    
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
        float3x3 WorldMatrix = float3x3(Input.vTangent, Input.vBinormal, Input.vNor.xyz);
    
        vNormal = mul(normalize(vNormal), WorldMatrix) * -1.f;
    }
    else
    {
        vNormal = Input.vNor.xyz;
    }
    
    vector vMask = vector(1.f, 0.1f, 0.1f, 0.1f);
    if (g_HasMaskTex)
    {
        vMask = g_MaskTexture.Sample(PointSampler, Input.vTex);
    }
    
    Output.vDiffuse = vMtrlDiffuse;
    Output.vNormal = vector(vNormal * 0.5f + 0.5f, vMask.b);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_CamNF.y, Input.vDir.x, Input.vDir.y);

    return Output;
}

PS_OUT PS_Main_NonLight(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex) + 0.3f * g_bSelected;
    
    Output.vColor = vector(vMtrlDiffuse.xyz, 1.f);
    
    return Output;
}

PS_OUT_DEFERRED PS_Main_AlphaTest(PS_IN Input)
{
    PS_OUT_DEFERRED Output = (PS_OUT_DEFERRED) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex) + 0.3f * g_bSelected;
    
    if (vMtrlDiffuse.a < 0.5f)
    {
        discard;
    }
        
    float3 vNormal;
    if (g_HasNorTex)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, Input.vTex);
    
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
        float3x3 WorldMatrix = float3x3(Input.vTangent, Input.vBinormal, Input.vNor.xyz);
    
        vNormal = mul(normalize(vNormal), WorldMatrix) * -1.f;
    }
    else
    {
        vNormal = Input.vNor.xyz;
    }
    
    vector vMask = vector(1.f, 0.1f, 0.1f, 0.1f);
    if (g_HasMaskTex)
    {
        vMask = g_MaskTexture.Sample(PointSampler, Input.vTex);
    }
    
    Output.vDiffuse = vMtrlDiffuse;
    Output.vNormal = vector(vNormal * 0.5f + 0.5f, vMask.b);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_CamNF.y, Input.vDir.x, Input.vDir.y);


    return Output;
}

struct PS_SHADOW_IN
{
    vector vPos : SV_Position;
    float2 vTex : Texcoord0;
};

void PS_Main_Shadow(PS_SHADOW_IN Input)
{
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex);
    
    if (0.3f > vDiffuse.a)
        discard;

}

struct PS_WATER_IN
{
    vector vPos : SV_Position; // == float4
    vector vNor : Normal;
    float2 vTex : Texcoord0;
    vector vWorldPos : Texcoord1;
    vector vProjPos : Texcoord2;
    float3 vTangent : Tangent;
    float3 vBinormal : Binormal;
    
    float fClip : SV_ClipDistance0;
};

struct PS_WATER_OUT
{
    vector vDiffuse : SV_Target0;
    vector vNormal : SV_Target1;
    vector vDepth : SV_Target2;
};

PS_WATER_OUT PS_Main_Water(PS_WATER_IN Input)
{
    PS_WATER_OUT Output = (PS_WATER_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex) + 0.3f * g_bSelected;
    
    if (vMtrlDiffuse.a < 0.5f)
    {
        discard;
    }
        
    float3 vNormal;
    if (g_HasNorTex)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, Input.vTex);
    
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
        float3x3 WorldMatrix = float3x3(Input.vTangent, Input.vBinormal, Input.vNor.xyz);
    
        vNormal = mul(normalize(vNormal), WorldMatrix) * -1.f;
    }
    else
    {
        vNormal = Input.vNor.xyz;
    }
    
    vector vMask = vector(1.f, 0.1f, 0.1f, 0.1f);
    if (g_HasMaskTex)
    {
        vMask = g_MaskTexture.Sample(PointSampler, Input.vTex);
    }
    
    Output.vDiffuse = vMtrlDiffuse;
    Output.vNormal = vector(vNormal * 0.5f + 0.5f, vMask.b);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_CamNF.y, 0.f, 0.f);

    return Output;
}


technique11 DefaultTechnique_Shader_StatInstance
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main();
    }

    pass NonLight
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_NonLight();
    }

    pass OutLine
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_DrawStencil, 1);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main();
    }

    pass AlphaTestMeshes
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_AlphaTest();
    }

    pass Shadow
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Shadow();
        GeometryShader = compile gs_5_0 GS_Shadow();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Shadow();
    }

    pass Water
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main_Water();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Water();
    }
};