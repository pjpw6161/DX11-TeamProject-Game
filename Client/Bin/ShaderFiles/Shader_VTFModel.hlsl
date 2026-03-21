#include "Engine_Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
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

tagAnimTimeDesc g_OldAnimDesc;

tagPlayAnimDesc g_PlayAnimDesc;


Texture2DArray g_BoneTexture;

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
    float4 vDir : DIRECTION;
};

matrix Get_BoneMatrix(VS_IN Input)
{
    float fIndices[4] = { Input.vBlendIndices.x, Input.vBlendIndices.y, Input.vBlendIndices.z, Input.vBlendIndices.w };
   
    float fWeights[4] = { Input.vBlendWeight.x, Input.vBlendWeight.y, Input.vBlendWeight.z, Input.vBlendWeight.w };

    int iAnimIndex[2];
    int iCurrentFrame[2];
    int iNextFrame[2];
    float fRatio[2];
    
    iAnimIndex[0] = g_PlayAnimDesc.eCurrent.iAnimIndex;
    iCurrentFrame[0] = g_PlayAnimDesc.eCurrent.iCurrFrame;
    iNextFrame[0] = g_PlayAnimDesc.eCurrent.iNextFrame;
    fRatio[0] = g_PlayAnimDesc.eCurrent.fRatio;
    
    iAnimIndex[1] = g_PlayAnimDesc.eNext.iAnimIndex;
    iCurrentFrame[1] = g_PlayAnimDesc.eNext.iCurrFrame;
    iNextFrame[1] = g_PlayAnimDesc.eNext.iNextFrame;
    fRatio[1] = g_PlayAnimDesc.eNext.fRatio;
    
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
            
            LerpBone = lerp(LerpBone, nextLerpBone, g_PlayAnimDesc.SwitchRatio);

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
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
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
    
    Output.vPos = vNewPos;
    Output.vNor = normalize(mul(vNormal, g_WorldMatrix));
    Output.vTex = Input.vTex;
    Output.vWorldPos = mul(vector(Input.vPos, 1.f), g_WorldMatrix);
    Output.vProjPos = Output.vPos;
    Output.vTangent = normalize(mul(vector(Input.vTan, 0.f), g_WorldMatrix)).xyz;
    Output.vBinormal = normalize(cross(Output.vNor.xyz, Output.vTangent));
    Output.vDir = vCalDir;
    
    return Output;
}

struct PS_IN
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

struct PS_OUT
{
    vector vDiffuse : SV_Target0;
    vector vNormal : SV_Target1;
    vector vDepth : SV_Target2;
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
    
    vector vMask = vector(1.f, 0.1f, 0.1f, 0.f);
    if (g_HasMaskTex)
    {
        vMask = g_MaskTexture.Sample(PointSampler, Input.vTex);
    }
    
    Output.vDiffuse = vMtrlDiffuse;
    Output.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, vMask.b);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_CamNF.y, Input.vDir.x, Input.vDir.y);
    
    return Output;
}

technique11 DefaultTechniqueShader_VTF
{
    pass Default
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main();
    }
};