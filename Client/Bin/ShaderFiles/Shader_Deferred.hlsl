#include "Engine_Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

vector g_vLightDir;
vector g_vLightPos;
vector g_vLightAtt;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vCamPosition;
float2 g_vCamNF;
float g_fLightFar;
float g_fHellStart;

float2 g_vFogNF;
vector g_vFogColor;

//Texture2DMS<vector> g_DiffuseTexture;
// MRT_Object
Texture2D g_DiffuseTexture;
Texture2D g_Normal_Spec_Texture;
Texture2D g_Depth_Velocity_Texture;
Texture2D g_RimMaskTexture;

// MRT_Light
Texture2D g_ShadeTexture;
Texture2D g_SpecularTexture;


Texture2D g_BlurTexture;

// Debugs
Texture2D g_DebugTexture;
Texture2DArray g_DebugArrayTexture;
uint g_DebugArrayIndex;


// Shadow
Texture2DArray g_LightDepthTexture;
matrix g_LightViewMatrix[3];
matrix g_LightProjMatrix[3];
vector g_ClipZ;
bool g_TurnOnShadow;


// SSAO
Texture2D g_SSAONoiseNormal;
Texture2D g_SSAOTexture;
matrix g_CamViewMatrix;
matrix g_CamProjMatrix;

// HDR
Texture2D g_Luminance;
Texture2D g_DeferredTexture;
Texture2D g_BloomTexture;

// Outline
Texture2D g_StencilTexture;
vector g_OutlineColor;
vector g_CheckColor;
uint g_OutlineColorIndex;

// Blurs
bool TurnOnSSAO;
bool TurnOnToneMap;
bool TurnOnBloom;
bool TurnOnRim;
SSAO_DESC g_SSAO;
HDR_DESC g_HDR;

// Final
Texture2D g_Texture;
Texture2D g_BlendTexture;
Texture2D g_BlendAlphaTexture;
Texture2D g_DistortionTexture;

// Effect
Texture2D g_EffectColorTexture;
Texture2D g_AlphaTexture;

bool g_WorldOrTex;
float2 g_vCenterTexPos;
vector g_vCenterPos;
float g_fRadialBlur_Power;

vector Get_WorldPos(float2 vTex)
{
    vector vDepth_Velocity_Desc = g_Depth_Velocity_Texture.Sample(PointClampSampler, vTex);
    float fViewZ = vDepth_Velocity_Desc.y * g_vCamNF.y;
    
    vector vWorldPos;
    vWorldPos.x = vTex.x * 2.f - 1.f;
    vWorldPos.y = vTex.y * -2.f + 1.f;
    vWorldPos.z = vDepth_Velocity_Desc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    return vWorldPos;
}

vector Get_Normal(vector vNormal_Spec)
{
    vector vNormal = vector(vNormal_Spec.xyz * 2.f - 1.f, 0.f);
    
    return normalize(vNormal);
}

vector Get_ViewPos(float2 vTex)
{
    vector vDepth_Velocity_Desc = g_Depth_Velocity_Texture.Sample(PointClampSampler, vTex);
    float fViewZ = vDepth_Velocity_Desc.y * g_vCamNF.y;
    
    vector ViewPos;
    ViewPos.x = vTex.x * 2.f - 1.f;
    ViewPos.y = vTex.y * -2.f + 1.f;
    ViewPos.z = vDepth_Velocity_Desc.x;
    ViewPos.w = 1.f;
    
    ViewPos = ViewPos * fViewZ;
    ViewPos = mul(ViewPos, g_ProjMatrixInv);
    
    return ViewPos;
}

float Get_Luminance(float3 vRGB)
{
    return max(dot(vRGB, g_fLuminace), 0.001f);
}

float CalcCascadeShadowFactor(uint lightIndex, vector vLightPos)
{
    float3 ProjPos = vLightPos.xyz / vLightPos.w;
    
    ProjPos.x = (ProjPos.x + 1.f) * 0.5f;
    ProjPos.y = (ProjPos.y - 1.f) * -0.5f;
    
    if(1.f < ProjPos.z)
        return 1.f;
    
    float3 LightUV = float3(ProjPos.x, ProjPos.y, lightIndex);
    
    return g_LightDepthTexture.Sample(LinearSampler, LightUV).r;
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

PS_OUT PS_Main_Debug(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_DebugTexture.Sample(LinearSampler, Input.vTexcoord);
    
    return Output;
}

PS_OUT PS_Main_DebugArray(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;

    Output.vColor = g_DebugArrayTexture.Sample(LinearSampler, float3(Input.vTexcoord, (float) g_DebugArrayIndex));
    
    return Output;
}

struct PS_OUT_Light
{
    vector vShade : SV_Target0;
    vector vSpecular : SV_Target1;
    vector vEmissive : SV_Target2;
};

PS_OUT_Light PS_Main_Directional(PS_IN Input)
{
    PS_OUT_Light Output = (PS_OUT_Light) 0;
    
    vector vNormal_Spec = g_Normal_Spec_Texture.Sample(PointSampler, Input.vTexcoord);
    
    vector vNormal = Get_Normal(vNormal_Spec);
    
    //Output.vShade = max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f);
    //Output.vShade = g_vLightDiffuse * saturate(ceil(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) * 2.f) / 2.f + g_vLightAmbient); // Ä«Å÷
    Output.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + g_vLightAmbient); // Ä«Å÷X
    
    vector vReflect = normalize(reflect(normalize(g_vLightDir), vNormal));
    
    vector vWorldPos = Get_WorldPos(Input.vTexcoord);
    
    vector vLook = vWorldPos - g_vCamPosition;
    
    Output.vSpecular = vNormal_Spec.a * g_vLightSpecular * pow(saturate(dot(normalize(vLook) * -1.f, vReflect)), 30.f);
    
    //vector vRimMask = g_RimMaskTexture.Sample(LinearSampler, Input.vTexcoord);
    
    //float fRimInensity = dot(vSpecDesc.rgb, vRimMask.rgb);
    
    //Output.vEmissive = vRimMask * fRimInensity;
    
    return Output;
}

PS_OUT_Light PS_Main_Point(PS_IN Input)
{
    PS_OUT_Light Output = (PS_OUT_Light) 0;
    
    vector vWorldPos = Get_WorldPos(Input.vTexcoord);

    vector vNormal_Spec = g_Normal_Spec_Texture.Sample(PointSampler, Input.vTexcoord);
    
    vector vNormal = Get_Normal(vNormal_Spec);
    
    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);
    
    float fAtt = 1.f / (g_vLightAtt.y + g_vLightAtt.z * fDistance + g_vLightAtt.w * (fDistance * fDistance));

    //Output.vShade = fAtt * (g_vLightDiffuse * saturate(ceil(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) * 2.f) / 2.f + g_vLightAmbient));
    Output.vShade = fAtt * (g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, normalize(vNormal)), 0.f) + g_vLightAmbient));

    vector vReflect = normalize(reflect(normalize(vLightDir), vNormal));
    vector vLook = vWorldPos - g_vCamPosition;
    
    Output.vSpecular = fAtt * (vNormal_Spec.a * g_vLightSpecular * pow(saturate(dot(normalize(vLook) * -1.f, vReflect)), 30.f));
    
    //vector vRimMask = g_RimMaskTexture.Sample(LinearSampler, Input.vTexcoord);
    
    //float fRimInensity = dot(vSpecDesc.rgb, vRimMask.rgb);
    
    //Output.vEmissive = fAtt * vRimMask * fRimInensity;
    
    return Output;
}

PS_OUT PS_Main_Water(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector FinalColor = 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTexcoord);

    if(vDiffuse.a == 0.f)
        discard;
    
    vDiffuse.rgb = pow(vDiffuse.rgb, 2.2f);
    
    vector vShade = g_ShadeTexture.Sample(LinearSampler, Input.vTexcoord);
    
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, Input.vTexcoord);
    
    FinalColor.rgb = vDiffuse.rgb * vShade.rgb; //+ vSpecular;
    FinalColor.a = vDiffuse.a;
    Output.vColor = FinalColor;
    
    return Output;
}

PS_OUT PS_Main_Deferred(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector FinalColor = 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTexcoord);
    
    if (vDiffuse.a == 0.f)
    {
        discard;
    }
    
    vDiffuse.rgb = pow(vDiffuse.rgb, 2.2f);
    
    vector vShade = g_ShadeTexture.Sample(LinearSampler, Input.vTexcoord);
    vShade.a = 1.f;
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, Input.vTexcoord);
    vSpecular.a = 0.f;
    
    vector vRimMask = g_RimMaskTexture.Sample(LinearSampler, Input.vTexcoord);
    vRimMask.a = 0.f;
    
    FinalColor = vDiffuse * vShade + vSpecular + vRimMask;
    
    vector vSsaoDesc = g_SSAOTexture.Sample(LinearSampler, Input.vTexcoord);
    
    if (TurnOnSSAO)
        FinalColor.rgb *= vSsaoDesc.rgb;
    
    vector vWorldPos = Get_WorldPos(Input.vTexcoord);
    
    
    vector vDepth_Velocity_Desc = g_Depth_Velocity_Texture.Sample(PointSampler, Input.vTexcoord);
    float fViewZ = vDepth_Velocity_Desc.y * g_vCamNF.y;
    
    if (true == g_TurnOnShadow)
    {
        float ClipZ[3];
        ClipZ[0] = g_ClipZ.y;
        ClipZ[1] = g_ClipZ.z;
        ClipZ[2] = g_ClipZ.w;
    
        vector vLightPos;
        for (uint i = 0; i < 3; ++i)
        {
            float Z = ClipZ[i] * (g_vCamNF.y - g_vCamNF.x) - g_vCamNF.y * g_vCamNF.x / g_vCamNF.y;
            vLightPos = mul(vWorldPos, g_LightViewMatrix[i]);
            vLightPos = mul(vLightPos, g_LightProjMatrix[i]);
        
            if (fViewZ <= Z)
            {
                float fLightDepth = CalcCascadeShadowFactor(i, vLightPos);
                if (vLightPos.z - 0.0025f > fLightDepth)
                {
                    FinalColor.rgb = FinalColor.rgb * 0.5f;
                }
                break;
            }
        }
    }
    
    float fFogFactor = saturate((g_vFogNF.y - fViewZ) / (g_vFogNF.y - g_vFogNF.x));
    vector vFogColor = g_vFogColor;
    vFogColor.a = 1.f;
    
    if (vWorldPos.y < g_fHellStart)
    {
        //float fHell = (vWorldPos.y + 15.f) / 35.f;
        float fHell = 1.f - ((g_fHellStart - vWorldPos.y) / 20.f);
        fHell = max(fHell, 0.f);
        FinalColor *= vector(fHell, fHell, fHell, 1.f);
        vFogColor *= fHell;
    }
       

    FinalColor = lerp(fFogFactor * FinalColor, vFogColor, (1.f - fFogFactor));
    //FinalColor = fFogFactor * FinalColor + (1.f - fFogFactor) * vFogColor;
    
    FinalColor.a = 1.f;
    Output.vColor = FinalColor;
    
    return Output;
}

PS_OUT PS_Main_Blend(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vAccum = g_EffectColorTexture.Sample(PointSampler, Input.vTexcoord);
    float fAlpha = g_AlphaTexture.Sample(PointSampler, Input.vTexcoord).r;
    
    float3 vColor = vAccum / max(vAccum.a, 1e-5);
    Output.vColor = vector(vColor, fAlpha);
    
    //Output.vColor = vector(0.f, 0.f, 0.f, 0.f);
    
    //Output.vColor = g_BlurTexture.Sample(LinearSampler, Input.vTexcoord);
    
    //float2 fTexelSize = 1.f / float2(g_fScreenWidth, g_fScreenHeight);
    
    //for (int y = -10; y <= 10; ++y)
    //{
    //    for (int x = -10; x <= 10; ++x)
    //    {
    
    //        Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord + float2(x, y) * fTexelSize) * 1.f / 441.f;
    //    }
    //}
    
    return Output;
}

//float Get_AO(float2 vTex, float2 plusTex, vector myPos, float3 myNor)
float Get_Occlusion(float distZ)
{
    float occlusion = 0.f;
    
    if (distZ > 0.05f)
    {
        float fadeLength = 2.f - 0.01f;
        
        occlusion = saturate((2.f - distZ) / fadeLength);
    }
    
    return occlusion;
}

PS_OUT PS_Main_SSAO(PS_IN Input)
{
    // ¿Ïº®ÇÑµí?
    
    PS_OUT Out = (PS_OUT) 0;
    
    vector vNormal_Spec = g_Normal_Spec_Texture.Sample(PointSampler, Input.vTexcoord);
    
    float3 MyViewNormal = normalize(mul(Get_Normal(vNormal_Spec), g_CamViewMatrix).xyz);
    
    float3 P = Get_ViewPos(Input.vTexcoord).xyz;
    
    vector vNoiseNormal = g_SSAONoiseNormal.Sample(LinearSampler, Input.vTexcoord);
    
    vNoiseNormal.xyz = normalize(vNoiseNormal.xyz * 2.f - 1.f);
    
    float OcclusionSum = 0.f;
    
    for (uint i = 0; i < 14; ++i)
    {
        float3 OffsetVector = normalize(g_OffSetVector[i]) * g_Random[i];
        
        float3 Offset = reflect(OffsetVector, vNoiseNormal.xyz);
        
        float flip = sign(dot(Offset, MyViewNormal));
        
        float3 Q = P + flip * 0.25f * Offset;
        
        vector ProjQ = mul(vector(Q, 1.f), g_CamProjMatrix);
        ProjQ /= ProjQ.w;
        
        ProjQ.x = (ProjQ.x + 1.f) * 0.5f;
        ProjQ.y = (ProjQ.y - 1.f) * -0.5f;
        
        float3 R = Get_ViewPos(ProjQ.xy).xyz; //(rz / Q.z) * Q;
        
        float DistZ = P.z - R.z;
        
        float dp = max(dot(MyViewNormal, normalize(R - P)), 0.f);
        
        float Occlusion = dp * Get_Occlusion(DistZ);
        
        OcclusionSum += Occlusion;
    }
    
    OcclusionSum /= 14.f;
    
    float Access = 1.f - OcclusionSum;
    
    Access = pow(Access, 4.f);
    
    Out.vColor = vector(Access, Access, Access, 1.f);
    
    return Out;
}

PS_OUT PS_Main_HDR(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vColor = g_DeferredTexture.Sample(LinearSampler, Input.vTexcoord);
    
    if(0.f == vColor.a)
        discard;
    
    float3 vHDRColor = vColor.rgb;
    
    if (true == TurnOnBloom)
    {
        vector vBlur = g_BloomTexture.Sample(LinearSampler, Input.vTexcoord);
        
        vHDRColor += vBlur.rgb;
    }
    
    if (true == TurnOnToneMap)
    {
        float a = 2.51f;
        float b = 0.03f;
        float c = 2.43f;
        float d = 0.59f;
        float e = 0.14f;
        vHDRColor = saturate((vHDRColor * (a * vHDRColor + b)) / (vHDRColor * (c * vHDRColor + d) + e));
        
    }
    
    vHDRColor = pow(vHDRColor, 1.f / 2.2f);
        
    Output.vColor = vector(vHDRColor, 1.f);
    
    return Output;
}

PS_OUT PS_Main_Check(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_OutlineColor;
    
    return Output;
}

PS_OUT PS_Main_Outline(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float xFilter[9] = { -1.f, 0.f, 1.f, -2.f, 0.f, 2.f, -1.f, 0.f, 1.f };
    float yFilter[9] = { 1.f, 2.f, 1.f, 0.f, 0.f, 0.f, -1.f, -2.f, -1.f };
    
    float2 tX = float2(1.f / 1280.f, 0.f);
    float2 tY = float2(0.f, 1.f / 720.f);
    
    float grid[9];
    float3 GreyScale = g_CheckColor.xyz;
    
    grid[0] = dot(g_StencilTexture.Sample(PointClampSampler, Input.vTexcoord - tX + tY).rgb, GreyScale);
    grid[1] = dot(g_StencilTexture.Sample(PointClampSampler, Input.vTexcoord + tY).rgb, GreyScale);
    grid[2] = dot(g_StencilTexture.Sample(PointClampSampler, Input.vTexcoord + tX + tY).rgb, GreyScale);
    grid[3] = dot(g_StencilTexture.Sample(PointClampSampler, Input.vTexcoord - tX).rgb, GreyScale);
    grid[4] = dot(g_StencilTexture.Sample(PointClampSampler, Input.vTexcoord).rgb, GreyScale);
    grid[5] = dot(g_StencilTexture.Sample(PointClampSampler, Input.vTexcoord + tX).rgb, GreyScale);
    grid[6] = dot(g_StencilTexture.Sample(PointClampSampler, Input.vTexcoord - tX - tY).rgb, GreyScale);
    grid[7] = dot(g_StencilTexture.Sample(PointClampSampler, Input.vTexcoord - tY).rgb, GreyScale);
    grid[8] = dot(g_StencilTexture.Sample(PointClampSampler, Input.vTexcoord + tX - tY).rgb, GreyScale);
    
    float sX = 0.f;
    float sY = 0.f;
    
    for (uint i = 0; i < 9; ++i)
    {
        sX += grid[i] * xFilter[i];
        sY += grid[i] * yFilter[i];
    }
    
    float dist = sqrt(sX * sX + sY * sY);
    float edge = dist >= 0.5f ? 1.f : 0.f;
    
    vector vColor = g_OutlineColor;
    vColor.a *= edge;
    
    Output.vColor = vColor;
    
    return Output;
}


PS_OUT PS_Main_Blur(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_BlendTexture.Sample(LinearClampSampler, Input.vTexcoord);    
    
    return Output;
}

PS_OUT PS_Main_Distortion(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float2 vDistortion = g_DistortionTexture.Sample(LinearSampler, Input.vTexcoord).rg;
    //vDistortion = vDistortion * 2.f - 1.f;
    
    float2 vTex = Input.vTexcoord + vDistortion;
    
    vector vColor = g_Texture.Sample(LinearClampSampler, vTex);
    
    Output.vColor = vColor;
    
    return Output;
}

PS_OUT PS_Draw(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vColor = g_Texture.Sample(LinearSampler, Input.vTexcoord);
    if(0.f == vColor.a)
        discard;
    
    Output.vColor = vColor;
    
    return Output;
}

PS_OUT PS_MotionBlur(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    uint iNumBlurSample = 10;
    
    vector vDepth_Velocity_Desc = g_Depth_Velocity_Texture.Sample(LinearSampler, Input.vTexcoord);
    
    float2 Velocity = vDepth_Velocity_Desc.zw / (float)iNumBlurSample;
    
    float Depth = vDepth_Velocity_Desc.x;
    
    uint iCnt = 1;
    
    vector BColor;
    float BDepth;
    
    vector vColor;
    vColor = g_Texture.Sample(LinearSampler, Input.vTexcoord);
    
    for (uint i = iCnt; i < iNumBlurSample; ++i)
    {
        BColor = g_Texture.Sample(LinearClampSampler, Input.vTexcoord + Velocity * (float) i);
        BDepth = g_Depth_Velocity_Texture.Sample(LinearClampSampler, Input.vTexcoord + Velocity * (float) i).x;
        if (Depth < BDepth + 0.04f)
        {
            iCnt++;
            vColor += BColor;
        }
    }
    
    vColor /= (float) iCnt;
    
    Output.vColor = vColor;
    
    return Output;
}

PS_OUT PS_RadialBlur(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float2 Dir = 0.f;
    if (false == g_WorldOrTex)
    {
        matrix matVP = mul(g_CamViewMatrix, g_CamProjMatrix);
        
        vector vPos = mul(g_vCenterPos, matVP);
        
        vPos /= vPos.w;
    
        vPos.x = (vPos.x + 1.f) * 0.5f;
        vPos.y = (vPos.y - 1.f) * -0.5f;
    
        Dir = vPos.xy - Input.vTexcoord;
    }
    else
    {
        Dir = g_vCenterTexPos - Input.vTexcoord;
    }

    
    float2 DirMulti = Dir * g_fRadialBlur_Power;
        
    float fSize[11] = { -0.05f, -0.04f, -0.03f, -0.02f, -0.01f, 0.f, 0.01f, 0.02f, 0.03f, 0.04f, 0.05f };
    
    float fGaussian_weights[11] =
    {
        0.0009765625f,
        0.009765625f,
        0.0439453125f,
        0.1171875f,
        0.205078125f,
        0.24609375f,
        0.205078125f,
        0.1171875f,
        0.0439453125f,
        0.009765625f,
        0.0009765625f
    };
    
    float2 UV;
    vector vColor = 0;
    for (uint i = 0; i < 11; ++i)
    {
        UV = (DirMulti * fSize[i]) + Input.vTexcoord;
        vColor += g_Texture.Sample(LinearClampSampler, UV) * fGaussian_weights[i];
    }
       
    Output.vColor = vColor;
    
    return Output;
}

technique11 DefaultTechnique
{
    pass Debug // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Debug();
    }

    pass DebugArray // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_DebugArray();
    }

    pass Light_Directional // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OnebyOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Directional();
    }

    pass Light_Point // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OnebyOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Point();
    }

    pass Water // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Water();
    }

    pass Deferrd // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Deferred();
    }

    pass Blend // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_InvAlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Blend();
    }

    pass SSAO // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_SSAO();
    }

    pass HDR // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_HDR();
    }

    pass CheckStencil // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_StencilEqual, g_OutlineColorIndex);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Check();
    }

    pass DrawOutline // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_StencilNotEqual, g_OutlineColorIndex);
        SetBlendState(BS_Outline, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Outline();
    }

    pass Draw_Blur // 11
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OnebyOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Blur();
    }

    pass Draw_Final // 12
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Distortion();
    }
    
    pass Just_Draw // 13
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Draw();
    }

    pass Motion_Blur // 14
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MotionBlur();
    }

    pass Radial_Blur // 15
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_RadialBlur();
    }

};