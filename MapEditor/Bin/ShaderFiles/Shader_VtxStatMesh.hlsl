#include "Engine_Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_MaskTexture;
texture2D g_CloudTexture;
texture2D g_DissolveTexture;
texture2D g_GradationTexture;

vector g_vColor;
float g_fAlpha;
float g_fx, g_fy;
vector g_vCamPos;
float g_fCamFar;
float g_fLightFar;
float g_fDissolveRatio;
bool g_HasNorTex;
bool g_HasMaskTex;
bool g_bSelected = false;


vector g_vLightDiffuse;

float2 g_vUVTransform;

// ¿ø¸í
float4 g_vClipPlane;
float4 g_RimColor;
vector g_OldWorldMatrix, g_OldViewMatrix;

int g_iID = 0;

struct VS_IN
{
    float3 vPos : Position;
    float3 vNor : Normal;
    float2 vTex : Texcoord0;
    float3 vTan : Tangent;
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
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    //matrix matOldWV, matOldWVP;
    
    //matOldWV = mul(g_OldWorldMatrix, g_OldViewMatrix);
    //matOldWVP = mul(matOldWV, g_ProjMatrix);
    
    vector vNewPos = mul(vector(Input.vPos, 1.f), matWVP);
    //vector vOldPos = mul(vector(Input.vPos, 1.f), matOldWVP);
    
    //vector vCalNor = mul(vector(Input.vNor, 0.f), matWV);
    
    //vector vDir = vNewPos - vOldPos;
    
    //float a = dot(normalize(vDir), normalize(vCalNor));
    
    //vector vPos;
    //if(a < 0.f)
    //    vPos = vOldPos;
    //else
    //    vPos = vNewPos;
    
    //float2 velocity = (vNewPos.xy / vNewPos.w) - (vOldPos.xy / vOldPos.w);
    
    //vector vCalDir;
    //vCalDir.xy = velocity * 0.5f;
    //vCalDir.y *= -1.f;
	
    Output.vPos = vNewPos;
    Output.vNor = normalize(mul(vector(Input.vNor, 0.f), g_WorldMatrix));
    Output.vTex = Input.vTex;
    Output.vWorldPos = mul(vector(Input.vPos, 1.f), g_WorldMatrix);
    Output.vProjPos = Output.vPos;
    Output.vTangent = normalize(mul(vector(Input.vTan, 0.f), g_WorldMatrix)).xyz;
    Output.vBinormal = normalize(cross(Output.vNor.xyz, Output.vTangent));
    Output.vDir = 0.f;
    
    return Output;
}

VS_OUT VS_OutLine(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
	
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    vector vPos = vector(Input.vPos, 1.f);
    vector vNor = vector(Input.vNor, 0.f);
    
    float fDist = length(g_vCamPos - mul(vPos, g_WorldMatrix));
    
    float fThickness = clamp(fDist / 300.f, 0.03f, 0.2f);
    
    vPos += normalize(vNor) * (fThickness + 0.5 * g_bSelected);
    
    Output.vPos = mul(vPos, matWVP);
    Output.vNor = normalize(mul(vNor, g_WorldMatrix));
    Output.vTex = Input.vTex;
    Output.vWorldPos = mul(vector(Input.vPos, 1.f), g_WorldMatrix);
    Output.vProjPos = Output.vPos;
	
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
    float2 vDir : DIRECTION;
    
    float fClip : SV_ClipDistance0;
};

VS_WATER_OUT VS_Main_Water(VS_IN Input)
{
    VS_WATER_OUT Output = (VS_WATER_OUT) 0;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Output.vPos = mul(vector(Input.vPos, 1.f), matWVP);
    Output.vNor = normalize(mul(vector(Input.vNor, 0.f), g_WorldMatrix));
    Output.vTex = Input.vTex;
    Output.vWorldPos = mul(vector(Input.vPos, 1.f), g_WorldMatrix);
    Output.vProjPos = Output.vPos;
    Output.vTangent = normalize(mul(vector(Input.vTan, 0.f), g_WorldMatrix)).xyz;
    Output.vBinormal = normalize(cross(Output.vNor.xyz, Output.vTangent));
    
    Output.fClip = dot(mul(vector(Input.vPos, 1.f), g_WorldMatrix), g_vClipPlane);
    
    return Output;

}

struct VS_SHADOW_OUT
{
    vector vPos : SV_Position; // == float4
    float2 vTex : Texcoord0;
};

VS_SHADOW_OUT VS_Shadow(VS_IN Input)
{
    VS_SHADOW_OUT Output = (VS_SHADOW_OUT) 0;

    Output.vPos = mul(vector(Input.vPos, 1.f), g_WorldMatrix);
    Output.vTex = Input.vTex;
    
    return Output;
}

struct GS_SHADOW_IN
{
    vector vPos : SV_Position; // == float4
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
    float2 vDir : DIRECTION;
    
};

struct PS_OUT_DEFERRED
{
    vector vDiffuse : SV_Target0;
    vector vNormal : SV_Target1;
    vector vDepth : SV_Target2;
    vector vRimMask : SV_Target3;
    int ID : SV_Target4;

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
    
    Output.vDiffuse = vector(vMtrlDiffuse.xyz, 1.f);
    Output.vNormal = vector(vNormal * 0.5f + 0.5f, vMask.b);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_fCamFar, Input.vDir.x, Input.vDir.y);
    Output.ID = g_iID;

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
    
    Output.vDiffuse = vMtrlDiffuse;
    Output.vNormal = vector(vNormal * 0.5f + 0.5f, vMask.b);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_fCamFar, Input.vDir.x, Input.vDir.y);
    Output.ID = g_iID;

    return Output;
}

PS_OUT_DEFERRED PS_OutLine(PS_IN Input)
{
    PS_OUT_DEFERRED Output = (PS_OUT_DEFERRED) 0;
    
    vector vLook = g_vCamPos - Input.vWorldPos;
    float DotProduct = dot(normalize(vLook), normalize(Input.vNor));
    if (DotProduct > 0.f)
    {
        discard;
    }
    
    Output.vDiffuse = g_vColor;
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Output.ID = g_iID;

    return Output;
}

PS_OUT PS_Main_Sky(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_DiffuseTexture.Sample(LinearSampler, Input.vTex);
    Output.vColor = 0.7f * Output.vColor + (1.f - 0.7f) * g_vLightDiffuse;

    return Output;
}

PS_OUT PS_Main_COL(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    return Output;
}

PS_OUT PS_Main_Effect(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;

    Output.vColor = g_vColor;
    
    return Output;
}

PS_OUT PS_Main_Effect_Dissolve(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;

    float fDissolve = g_DissolveTexture.Sample(LinearSampler, Input.vTex).r;
    
    if (g_fDissolveRatio > fDissolve)
    {
        discard;
    }
    
    Output.vColor = g_vColor;
    
    return Output;
}

PS_OUT PS_Main_Fireball(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;

    float fUV = 1.f - g_MaskTexture.Sample(LinearSampler, Input.vTex).x;
    Output.vColor = g_GradationTexture.Sample(LinearSampler, float2(fUV, 0.4f));
    
    return Output;
}

PS_OUT PS_Main_MaskEffect(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;

    vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex + g_vUVTransform);
    if (vMask.r < 0.1f)
    {
        discard;
    }
    
    Output.vColor = g_vColor;
    Output.vColor.a *= vMask.r;
    
    return Output;
}

PS_OUT PS_Main_MaskEffect_Dissolve(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;

    float fDissolve = g_DissolveTexture.Sample(LinearSampler, Input.vTex).r;
    
    if (g_fDissolveRatio > fDissolve)
    {
        discard;
    }
    
    vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex + g_vUVTransform);
    if (vMask.r < 0.1f)
    {
        discard;
    }
    
    Output.vColor = g_vColor;
    Output.vColor.a = vMask.r;
    
    return Output;
}

PS_OUT PS_Main_MaskEffect_Clamp(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;

    vector vMask = g_MaskTexture.Sample(LinearClampSampler, Input.vTex + g_vUVTransform);
    if (vMask.r < 0.1f)
    {
        discard;
    }
    
    Output.vColor = g_vColor;
    Output.vColor.a = vMask.r;
    
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
    
    if(0.5f > vDiffuse.a)
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
    float2 vDir : DIRECTION;
    
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
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex);
    if(vMtrlDiffuse.a < 0.3f)
        discard;
    
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
    
    Output.vDiffuse = vMtrlDiffuse;
    Output.vNormal = vector(vNormal * 0.5f + 0.5f, vMask.b);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_fCamFar, Input.vDir.x, Input.vDir.y);
    
    return Output;
}

PS_OUT_DEFERRED PS_Main_Rim(PS_IN Input)
{
    PS_OUT_DEFERRED Output = (PS_OUT_DEFERRED) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex);
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
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
    
    Output.vDiffuse = vector(vMtrlDiffuse.xyz, 1.f);
    Output.vNormal = vector(vNormal * 0.5f + 0.5f, vMask.b);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_fCamFar, Input.vDir.x, Input.vDir.y);
    Output.vRimMask = vRimColor;
    Output.ID = g_iID;

    return Output;
}

PS_OUT_DEFERRED PS_Main_WorldMap_Water(PS_IN Input)
{
    PS_OUT_DEFERRED Output = (PS_OUT_DEFERRED) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, float2(Input.vTex.x, (Input.vTex.y + g_fy*0.5f))) + 0.3f * g_bSelected;
    
    float3 vNormal;
    if (g_HasNorTex)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, float2(Input.vTex.x, Input.vTex.y + g_fy));
    
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
    
    Output.vDiffuse = vector(vMtrlDiffuse.xyz, 1.f);
    Output.vNormal = vector(vNormal * 0.5f + 0.5f, vMask.b);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_fCamFar, Input.vDir.x, Input.vDir.y);
    
    return Output;
}

PS_OUT_DEFERRED PS_Main_WorldMap_Cloud(PS_IN Input)
{
    PS_OUT_DEFERRED Output = (PS_OUT_DEFERRED) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler,Input.vTex);
    //vector vMask = g_MaskTexture.Sample(LinearClampSampler, (Input.vTex + g_vUVTransform));
    
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
    
    vector vCloud = g_CloudTexture.Sample(LinearSampler, float2(Input.vTex.x - g_fx, Input.vTex.y + g_fy));

    Output.vDiffuse = vector(vMtrlDiffuse.xyz, 1.f);
    Output.vDiffuse.r -= (vCloud.r * 0.5f);
    Output.vDiffuse.b -= (vCloud.r * 0.5f);
    Output.vDiffuse.g -= (vCloud.r * 0.5f);

    Output.vNormal = vector(vNormal * 0.5f + 0.5f, vMask.b);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_fCamFar, Input.vDir.x, Input.vDir.y);
    
    return Output;
}

PS_OUT PS_Main_DiffEffect(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;

    Output.vColor = g_DiffuseTexture.Sample(LinearSampler, Input.vTex + g_vUVTransform);
    
    return Output;
}

PS_OUT PS_Main_Effect_Alpha(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;

    Output.vColor.rgb = g_vColor.rgb;
    Output.vColor.a = g_fAlpha;
    
    return Output;
}

PS_OUT PS_Main_MaskEffect_Alpha(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;

    vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex + g_vUVTransform);
    if (vMask.r < 0.1f)
    {
        discard;
    }
    
    Output.vColor = g_vColor;
    Output.vColor.a *= vMask.r * g_fAlpha;
    
    return Output;
}

PS_OUT PS_Main_DiffEffect_Alpha(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;

    Output.vColor.rgb = g_DiffuseTexture.Sample(LinearSampler, Input.vTex + g_vUVTransform).rgb;
    Output.vColor.a = g_fAlpha;
    
    return Output;
}

technique11 DefaultTechnique_Shader_StatMesh
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
        SetRasterizerState(RS_CCW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_OutLine();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_OutLine();
    }

    pass AlphaTestMeshes
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_AlphaTest();
    }

    pass Sky
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sky();
    }

    pass COLMesh
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_COL();
    }

    pass SingleColoredEffect
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Effect();
    }

    pass SingleColoredEffectDissolve
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Effect_Dissolve();
    }

    pass Fireball
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Fireball();
    }

    pass MaskEffect
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_MaskEffect();
    }

    pass MaskEffectDissolve
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_MaskEffect_Dissolve();
    }

    pass MaskEffectClamp
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_MaskEffect_Clamp();
    }

    pass SingleColoredEffectFrontCull
    {
        SetRasterizerState(RS_CCW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Effect();
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

    pass WorldMap_Water //15
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_WorldMap_Water();
    }

    pass WorldMap_Cloud //16
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_WorldMap_Cloud();
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

    pass DiffEffect
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_DiffEffect();
    }

    pass SingleColoredAlpha
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Effect_Alpha();
    }

    pass MaskAlpha
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_MaskEffect_Alpha();
    }

    pass DiffEffectAlpha
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_DiffEffect_Alpha();
    }
};