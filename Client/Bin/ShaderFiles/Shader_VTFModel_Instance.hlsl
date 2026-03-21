#include "Engine_Shader_Define.hlsli"

#define MAX_INSTANCE 300

matrix g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_MaskTexture;

float2 g_CamNF;

bool g_HasNorTex;
bool g_HasMaskTex;

matrix g_OldWorldMatrix, g_OldViewMatrix;

struct tagAnimTimeDesc
{
    int iAnimIndex;
    uint iCurrFrame;
    uint iNextFrame;
    float fRatio;
    float fTime;
    float fSpeed;
    float2 vPadding;
};

struct tagPlayAnimDesc
{
    float SwitchDuration;
    float SwitchRatio;
    float SwitchTime;
    float fPadding;
    tagAnimTimeDesc eCurrent;
    tagAnimTimeDesc eNext;
};

struct tagPlayAnimBuffer
{
    tagPlayAnimDesc PlayAnimFrames[MAX_INSTANCE];
};

tagAnimTimeDesc g_OldAnimDesc;

tagPlayAnimBuffer g_PlayAnimInstances;

Texture2DArray g_BoneTexture;

struct VS_IN
{
    float3 vPos : Position;
    float3 vNor : Normal;
    float2 vTex : Texcoord0;
    float3 vTan : Tangent;
    uint4 vBlendIndices : BlendIndex;
    float4 vBlendWeight : BlendWeight;
        
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
};

matrix Get_BoneMatrix(VS_IN Input)
{
    float fIndices[4] = { Input.vBlendIndices.x, Input.vBlendIndices.y, Input.vBlendIndices.z, Input.vBlendIndices.w };
   
    float fWeights[4] = { Input.vBlendWeight.x, Input.vBlendWeight.y, Input.vBlendWeight.z, Input.vBlendWeight.w };

    int iAnimIndex[2];
    int iCurrentFrame[2];
    int iNextFrame[2];
    float fRatio[2];
    
    iAnimIndex[0] = g_PlayAnimInstances.PlayAnimFrames[Input.iID].eCurrent.iAnimIndex;
    iCurrentFrame[0] = g_PlayAnimInstances.PlayAnimFrames[Input.iID].eCurrent.iCurrFrame;
    iNextFrame[0] = g_PlayAnimInstances.PlayAnimFrames[Input.iID].eCurrent.iNextFrame;
    fRatio[0] = g_PlayAnimInstances.PlayAnimFrames[Input.iID].eCurrent.fRatio;
    
    iAnimIndex[1] = g_PlayAnimInstances.PlayAnimFrames[Input.iID].eNext.iAnimIndex;
    iCurrentFrame[1] = g_PlayAnimInstances.PlayAnimFrames[Input.iID].eNext.iCurrFrame;
    iNextFrame[1] = g_PlayAnimInstances.PlayAnimFrames[Input.iID].eNext.iNextFrame;
    fRatio[1] = g_PlayAnimInstances.PlayAnimFrames[Input.iID].eNext.fRatio;
    
    float4 CurrentBoneVec[4];
    float4 NextBoneVec[4];
    
    matrix CurrentBone = 0;
    matrix NextBone = 0;
    
    matrix BoneMatrix = 0;
    
    for (uint i = 0; i < 4; i++)
    {
        CurrentBoneVec[0] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 0, iCurrentFrame[0], iAnimIndex[0], 0));
        CurrentBoneVec[1] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 1, iCurrentFrame[0], iAnimIndex[0], 0));
        CurrentBoneVec[2] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 2, iCurrentFrame[0], iAnimIndex[0], 0));
        CurrentBoneVec[3] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 3, iCurrentFrame[0], iAnimIndex[0], 0));
        
        CurrentBone = matrix(CurrentBoneVec[0], CurrentBoneVec[1], CurrentBoneVec[2], CurrentBoneVec[3]);

        NextBoneVec[0] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 0, iNextFrame[0], iAnimIndex[0], 0));
        NextBoneVec[1] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 1, iNextFrame[0], iAnimIndex[0], 0));
        NextBoneVec[2] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 2, iNextFrame[0], iAnimIndex[0], 0));
        NextBoneVec[3] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 3, iNextFrame[0], iAnimIndex[0], 0));
        
        NextBone = matrix(NextBoneVec[0], NextBoneVec[1], NextBoneVec[2], NextBoneVec[3]);
        
        matrix LerpBone = lerp(CurrentBone, NextBone, fRatio[0]);
        
        if (0 <= iAnimIndex[1])
        {
            CurrentBoneVec[0] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 0, iCurrentFrame[1], iAnimIndex[1], 0));
            CurrentBoneVec[1] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 1, iCurrentFrame[1], iAnimIndex[1], 0));
            CurrentBoneVec[2] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 2, iCurrentFrame[1], iAnimIndex[1], 0));
            CurrentBoneVec[3] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 3, iCurrentFrame[1], iAnimIndex[1], 0));
        
            CurrentBone = matrix(CurrentBoneVec[0], CurrentBoneVec[1], CurrentBoneVec[2], CurrentBoneVec[3]);

            NextBoneVec[0] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 0, iNextFrame[1], iAnimIndex[1], 0));
            NextBoneVec[1] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 1, iNextFrame[1], iAnimIndex[1], 0));
            NextBoneVec[2] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 2, iNextFrame[1], iAnimIndex[1], 0));
            NextBoneVec[3] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 3, iNextFrame[1], iAnimIndex[1], 0));
        
            NextBone = matrix(NextBoneVec[0], NextBoneVec[1], NextBoneVec[2], NextBoneVec[3]);
            
            matrix nextLerpBone = lerp(CurrentBone, NextBone, fRatio[1]);
            
            LerpBone = lerp(LerpBone, nextLerpBone, g_PlayAnimInstances.PlayAnimFrames[Input.iID].SwitchRatio);

        }
        
        BoneMatrix += mul(LerpBone, fWeights[i]);
    }
    return BoneMatrix;
}

matrix Get_OldBoneMatrix(VS_IN Input)
{
    float fIndices[4] = { Input.vBlendIndices.x, Input.vBlendIndices.y, Input.vBlendIndices.z, Input.vBlendIndices.w };
   
    float fWeights[4] = { Input.vBlendWeight.x, Input.vBlendWeight.y, Input.vBlendWeight.z, Input.vBlendWeight.w };

    int iAnimIndex;
    int iCurrentFrame;
    int iNextFrame;
    float fRatio;
    
    iAnimIndex = g_OldAnimDesc.iAnimIndex;
    iCurrentFrame = g_OldAnimDesc.iCurrFrame;
    iNextFrame = g_OldAnimDesc.iNextFrame;
    fRatio = g_OldAnimDesc.fRatio;
        
    float4 CurrentBoneVec[4];
    float4 NextBoneVec[4];
    
    matrix CurrentBone = 0;
    matrix NextBone = 0;
    
    matrix BoneMatrix = 0;
    
    for (uint i = 0; i < 4; i++)
    {
        CurrentBoneVec[0] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 0, iCurrentFrame, iAnimIndex, 0));
        CurrentBoneVec[1] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 1, iCurrentFrame, iAnimIndex, 0));
        CurrentBoneVec[2] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 2, iCurrentFrame, iAnimIndex, 0));
        CurrentBoneVec[3] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 3, iCurrentFrame, iAnimIndex, 0));
        
        CurrentBone = matrix(CurrentBoneVec[0], CurrentBoneVec[1], CurrentBoneVec[2], CurrentBoneVec[3]);

        NextBoneVec[0] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 0, iNextFrame, iAnimIndex, 0));
        NextBoneVec[1] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 1, iNextFrame, iAnimIndex, 0));
        NextBoneVec[2] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 2, iNextFrame, iAnimIndex, 0));
        NextBoneVec[3] = g_BoneTexture.Load(int4(fIndices[i] * 4 + 3, iNextFrame, iAnimIndex, 0));
        
        NextBone = matrix(NextBoneVec[0], NextBoneVec[1], NextBoneVec[2], NextBoneVec[3]);
        
        matrix LerpBone = lerp(CurrentBone, NextBone, fRatio);
        
        BoneMatrix += mul(LerpBone, fWeights[i]);
    }
    return BoneMatrix;
}

VS_OUT VS_Main(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
	
    matrix matWV, matWVP;
    
    matWV = mul(Input.mWorld, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    matrix BoneMatrix = Get_BoneMatrix(Input);
    
    vector vPos = mul(vector(Input.vPos, 1.f), BoneMatrix);
    vector vNormal = mul(vector(Input.vNor, 0.f), BoneMatrix);
    
    Output.vPos = mul(vPos, matWVP);
    Output.vNor = normalize(mul(vNormal, Input.mWorld));
    Output.vTex = Input.vTex;
    Output.vWorldPos = mul(vector(Input.vPos, 1.f), Input.mWorld);
    Output.vProjPos = Output.vPos;
    Output.vTangent = normalize(mul(vector(Input.vTan, 0.f), Input.mWorld)).xyz;
    Output.vBinormal = normalize(cross(Output.vNor.xyz, Output.vTangent));
    
    return Output;
}

struct VS_Motion_Blur_Out
{
    vector vPos : SV_Position; // == float4
    vector vNor : Normal;
    float2 vTex : Texcoord0;
    vector vWorldPos : Texcoord1;
    vector vProjPos : Texcoord2;
    float3 vTangent : Tangent;
    float3 vBinormal : Binormal;
    float4 vDir : DIRECTION;
};

VS_Motion_Blur_Out VS_Motion_Blur(VS_IN Input)
{
    VS_Motion_Blur_Out Output = (VS_Motion_Blur_Out) 0;
	
    matrix matWV, matWVP;
    
    matWV = mul(Input.mWorld, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    matrix matOldWV, matOldWVP;
    
    matOldWV = mul(g_OldWorldMatrix, g_OldViewMatrix);
    matOldWVP = mul(matOldWV, g_ProjMatrix);
    
    matrix BoneMatrix = Get_BoneMatrix(Input);
    
    vector vNew = mul(vector(Input.vPos, 1.f), BoneMatrix);
    
    matrix OldBoneMatrix = Get_OldBoneMatrix(Input);
    
    vector vOld = mul(vector(Input.vPos, 1.f), OldBoneMatrix);
    
    
    vector vNormal = mul(vector(Input.vNor, 0.f), BoneMatrix);
    
    vector vOldPos = mul(vOld, matOldWVP);
    vector vNewPos = mul(vNew, matWVP);
    
    vector vDir = vNewPos - vOldPos;
    vector vCalNor = mul(vNormal, matWV);
    
    float a = dot(normalize(vDir), normalize(vCalNor));
    
    vector vPos;
    if(a<0.f)
        vPos = vOldPos;
    else
        vPos = vNewPos;
    
    float2 velocity = (vNewPos.xy / vNewPos.w) - (vOldPos.xy / vOldPos.w);
    
    vector vCalDir;
    vCalDir.xy = velocity * 0.5f;
    vCalDir.y *= -1.f;
    vCalDir.z = vPos.z;
    vCalDir.w = vPos.w;
    
    Output.vPos = vPos;
    Output.vNor = normalize(mul(vNormal, Input.mWorld));
    Output.vTex = Input.vTex;
    Output.vWorldPos = mul(vector(Input.vPos, 1.f), Input.mWorld);
    Output.vProjPos = Output.vPos;
    Output.vTangent = normalize(mul(vector(Input.vTan, 0.f), Input.mWorld)).xyz;
    Output.vBinormal = normalize(cross(Output.vNor.xyz, Output.vTangent));
    Output.vDir = vCalDir;
    
    return Output;
}

struct VS_SHADOW_OUT
{
    vector vPos : Position;
    float2 vTex : Texcoord0;
};

VS_SHADOW_OUT VS_Shadow(VS_IN Input)
{
    VS_SHADOW_OUT Output = (VS_SHADOW_OUT) 0;
    
    matrix matWV, matWVP;
    
    matWV = mul(Input.mWorld, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    matrix BoneMatrix = Get_BoneMatrix(Input);
    
    vector vPos = mul(vector(Input.vPos, 1.f), BoneMatrix);
    
    Output.vPos = mul(vPos, Input.mWorld);
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
};

struct PS_OUT
{
    vector vDiffuse : SV_Target0;
    vector vNormal : SV_Target1;
    vector vDepth : SV_Target2;
    vector vSpecular : SV_Target3;
};

PS_OUT PS_Main(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex);
    
    if (0.3f > vMtrlDiffuse.a)
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
    
    vector vSpecular = vector(0.f, 0.f, 0.f, 0.f);
    
    Output.vDiffuse = vMtrlDiffuse;
    Output.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_CamNF.y, 0.f, 0.f);
    Output.vSpecular = vSpecular;
    
    return Output;
}

struct PS_Blur_IN
{
    vector vPos : SV_Position; // == float4
    vector vNor : Normal;
    float2 vTex : Texcoord0;
    vector vWorldPos : Texcoord1;
    vector vProjPos : Texcoord2;
    float3 vTangent : Tangent;
    float3 vBinormal : Binormal;
    float4 vDir : DIRECTION;
};

struct PS_Blur_OUT
{
    vector vDiffuse : SV_Target0;
    vector vNormal : SV_Target1;
    vector vDepth : SV_Target2;
    vector vVelocity : SV_Target4;
};

PS_Blur_OUT PS_Motion_Blur(PS_Blur_IN Input)
{
    PS_Blur_OUT Output = (PS_Blur_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex);
    
    if (0.3f > vMtrlDiffuse.a)
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
    
    Output.vDiffuse = vMtrlDiffuse;
    Output.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_CamNF.y, 0.f, 0.f);
    Output.vVelocity = Input.vDir;
    
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

technique11 DefaultTechniqueShader_VTF
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

    pass Motion_Blur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Motion_Blur();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Motion_Blur();
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
};