#include "Engine_Shader_Define.hlsli"

matrix g_BoneMatrices[512];

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_MaskTexture;
texture2D g_DissolveTexture;

vector g_vColor = {1.f, 1.f, 1.f, 0.f};
vector g_vCamPos;
float2 g_CamNF;
float g_fLightFar;
float g_fDissolveRatio;

bool g_HasNorTex;
bool g_HasMaskTex;
bool g_bSelected = false;

vector g_RimColor;

matrix g_OldWorldMatrix, g_OldViewMatrix;

struct VS_IN
{
    float3 vPos : Position;
    float3 vNor : Normal;
    float2 vTex : Texcoord0;
    float3 vTan : Tangent;
    uint4 vBlendIndices : BlendIndex;
    float4 vBlendWeight : BlendWeight;
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
    vector vOldPos : Texcoord3;
};

VS_OUT VS_Main(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
	
    matrix matWV, matWVP;
    float fW = 1.f - (Input.vBlendWeight.x + Input.vBlendWeight.y + Input.vBlendWeight.z);
    matrix Bone = g_BoneMatrices[Input.vBlendIndices.x] * Input.vBlendWeight.x +
    g_BoneMatrices[Input.vBlendIndices.y] * Input.vBlendWeight.y +
    g_BoneMatrices[Input.vBlendIndices.z] * Input.vBlendWeight.z +
    g_BoneMatrices[Input.vBlendIndices.w] * fW;
    
    vector vPos = mul(vector(Input.vPos, 1.f), Bone);
    vector vNor = mul(vector(Input.vNor, 0.f), Bone);
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    vector vNewPos = mul(vPos, matWVP);
    
    matrix matOldWV, matOldWVP;
    
    matOldWV = mul(g_OldWorldMatrix, g_OldViewMatrix);
    matOldWVP = mul(matOldWV, g_ProjMatrix);
    
    vector vOldPos = mul(vPos, matOldWVP);
	
    Output.vPos = vNewPos;
    Output.vNor = normalize(mul(vNor, g_WorldMatrix));
    Output.vTex = Input.vTex;
    Output.vWorldPos = mul(vector(Input.vPos, 1.f), g_WorldMatrix);
    Output.vProjPos = Output.vPos;
    Output.vTangent = normalize(mul(vector(Input.vTan, 0.f), g_WorldMatrix)).xyz;
    Output.vBinormal = normalize(cross(Output.vNor.xyz, Output.vTangent));
    Output.vOldPos = vOldPos;
    
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
	
    float fW = 1.f - (Input.vBlendWeight.x + Input.vBlendWeight.y + Input.vBlendWeight.z);
    matrix Bone = g_BoneMatrices[Input.vBlendIndices.x] * Input.vBlendWeight.x +
    g_BoneMatrices[Input.vBlendIndices.y] * Input.vBlendWeight.y +
    g_BoneMatrices[Input.vBlendIndices.z] * Input.vBlendWeight.z +
    g_BoneMatrices[Input.vBlendIndices.w] * fW;
    
    vector vPos = mul(vector(Input.vPos, 1.f), Bone);
    
    Output.vPos = mul(vPos, g_WorldMatrix);
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
void GS_Main_Shadow(triangle GS_SHADOW_IN Input[3], inout TriangleStream<GS_SHADOW_OUT> Output)
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
    vector vOldPos : Texcoord3;
};

struct PS_OUT
{
    vector vColor : SV_Target0;
};

struct PS_OUT_DEFERRED
{
    vector vDiffuse : SV_Target0;
    vector vNormal : SV_Target1;
    vector vDepth : SV_Target2;
    vector vRimMask : SV_Target3;
};

PS_OUT_DEFERRED PS_Main(PS_IN Input)
{
    PS_OUT_DEFERRED Output = (PS_OUT_DEFERRED) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex) + vector(0.5f, 0.f, 0.f, 0.f) * g_bSelected;
    if (vMtrlDiffuse.a < 0.3f)
    {
        discard;
    }
    float3 vNormal;
    if (g_HasNorTex)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, Input.vTex);
    
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
        float3x3 WorldMatrix = float3x3(Input.vTangent, Input.vBinormal, Input.vNor.xyz);
    
        vNormal = normalize(mul(normalize(vNormal), WorldMatrix) * -1.f);
    }
    else
    {
        vNormal = normalize(Input.vNor.xyz);
    }
    
    vector vMask = vector(1.f, 0.1f, 0.1f, 0.1f);
    if (g_HasMaskTex)
    {
        vMask = g_MaskTexture.Sample(PointSampler, Input.vTex);
    }
    
    float2 Velocity = (Input.vProjPos.xy / Input.vProjPos.w) - (Input.vOldPos.xy / Input.vOldPos.w);
    
    float2 vCalDir;
    vCalDir = Velocity * 0.5f;
    vCalDir.y *= -1.f;
    
    Output.vDiffuse = vMtrlDiffuse;
    Output.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, vMask.b);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_CamNF.y, vCalDir.x, vCalDir.y);
    
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
    
    if (0.1f > vDiffuse.a)
        discard;

}

PS_OUT_DEFERRED PS_Main_Dissolve(PS_IN Input)
{
    PS_OUT_DEFERRED Output = (PS_OUT_DEFERRED) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex) + vector(0.5f, 0.f, 0.f, 0.f) * g_bSelected;
    float fDissolve = g_DissolveTexture.Sample(LinearSampler, Input.vTex).r;
    
    if (g_fDissolveRatio > fDissolve)
    {
        discard;
    }
  
    float3 vNormal;
    if (g_HasNorTex)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, Input.vTex);
    
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
        float3x3 WorldMatrix = float3x3(Input.vTangent, Input.vBinormal, Input.vNor.xyz);
    
        vNormal = normalize(mul(normalize(vNormal), WorldMatrix) * -1.f);
    }
    else
    {
        vNormal = normalize(Input.vNor.xyz);
    }
    
    vector vMask = vector(1.f, 0.1f, 0.1f, 0.1f);
    if (g_HasMaskTex)
    {
        vMask = g_MaskTexture.Sample(PointSampler, Input.vTex);
    }
    
    float2 Velocity = (Input.vProjPos.xy / Input.vProjPos.w) - (Input.vOldPos.xy / Input.vOldPos.w);
    
    float2 vCalDir;
    vCalDir = Velocity * 0.5f;
    vCalDir.y *= -1.f;
    
    Output.vDiffuse = vMtrlDiffuse;
    Output.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, vMask.b);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_CamNF.y, vCalDir.x, vCalDir.y);
    
    return Output;
}

PS_OUT_DEFERRED PS_Main_Rim(PS_IN Input)
{
    PS_OUT_DEFERRED Output = (PS_OUT_DEFERRED) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex) + vector(0.5f, 0.f, 0.f, 0.f) * g_bSelected;
    if (vMtrlDiffuse.a < 0.3f)
    {
        discard;
    }

    
    float3 vNormal;
    if (g_HasNorTex)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, Input.vTex);
    
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
        float3x3 WorldMatrix = float3x3(Input.vTangent, Input.vBinormal, Input.vNor.xyz);
    
        vNormal = normalize(mul(normalize(vNormal), WorldMatrix) * -1.f);
    }
    else
    {
        vNormal = normalize(Input.vNor.xyz);
    }
    
    vector vMask = vector(1.f, 0.1f, 0.1f, 0.1f);
    if (g_HasMaskTex)
    {
        vMask = g_MaskTexture.Sample(PointSampler, Input.vTex);
    }
    
    float3 vToCamera = normalize(g_vCamPos - Input.vWorldPos).xyz;
    
    float fRim = smoothstep(0.5f, 1.f, 1.f - max(0.f, dot(vNormal, vToCamera)));
    
    vector vRimColor = g_RimColor * fRim;
    
    float2 Velocity = (Input.vProjPos.xy / Input.vProjPos.w) - (Input.vOldPos.xy / Input.vOldPos.w);
    
    float2 vCalDir;
    vCalDir = Velocity * 0.5f;
    vCalDir.y *= -1.f;
    
    Output.vDiffuse = vMtrlDiffuse;
    Output.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, vMask.b);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_CamNF.y, vCalDir.x, vCalDir.y);
    
    Output.vRimMask = vRimColor;
    
    return Output;
}
PS_OUT_DEFERRED PS_Color(PS_IN Input)
{
    PS_OUT_DEFERRED Output = (PS_OUT_DEFERRED) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex) + vector(0.5f, 0.f, 0.f, 0.f) * g_bSelected;
    if (vMtrlDiffuse.a < 0.3f)
    {
        discard;
    }
    float3 vNormal;
    if (g_HasNorTex)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, Input.vTex);
    
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
        float3x3 WorldMatrix = float3x3(Input.vTangent, Input.vBinormal, Input.vNor.xyz);
    
        vNormal = normalize(mul(normalize(vNormal), WorldMatrix) * -1.f);
    }
    else
    {
        vNormal = normalize(Input.vNor.xyz);
    }
    
    vector vMask = vector(1.f, 0.1f, 0.1f, 0.1f);
    if (g_HasMaskTex)
    {
        vMask = g_MaskTexture.Sample(PointSampler, Input.vTex);
    }
    
    
    Output.vDiffuse = g_vColor;
    Output.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, vMask.b);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_CamNF.y, 0.f, 0.f);
    
    return Output;
}


technique11 DefaultTechnique_Shader_AnimMesh
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

    pass Shadow
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Shadow();
        GeometryShader = compile gs_5_0 GS_Main_Shadow();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Shadow();
    }

    pass Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Dissolve();
    }

    pass RimLight
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Rim();
    }

    pass DissolveNoCull
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Dissolve();
    }

    pass MeshColor
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Color();
    }
};