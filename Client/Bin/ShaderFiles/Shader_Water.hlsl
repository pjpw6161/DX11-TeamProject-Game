#include "Engine_Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_OldWorldMatrix, g_OldViewMatrix;

Texture2D g_NormalTexture;

Texture2D g_ReflectionTexture; // 반사 텍스처
Texture2D g_RefractionTexture; // 굴절 텍스처

matrix g_ReflectionViewMatrix; // 반사 View Matrix

float g_fReflectionScale;
float g_fRefractionScale;

float g_fWaterPos;

vector g_vCamPos;
float2 g_vCamNF;


struct VS_IN
{
    float3 vPos : POSITION;
    float3 vNor : Normal;
    float2 vTex : TEXCOORD0;
};

struct VS_OUT_WATER
{
    vector vPos : SV_POSITION;
    vector vNor : Normal;
    float2 vTex : TEXCOORD0;
    vector vProjPos : PROJPOS;
    float3 vTangent : Tangent;
    float3 vBinormal : Binormal;
    vector vReflectionPos : REFLECTION;
    vector vRefractionPos : REFRACTION;
    
    vector vOldPos : OLDPOS;
};

VS_OUT_WATER VS_MAIN_Water(VS_IN Input)
{
    VS_OUT_WATER Output = (VS_OUT_WATER) 0;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    matrix matOldWV, matOldWVP;
    
    matOldWV = mul(g_OldWorldMatrix, g_OldViewMatrix);
    matOldWVP = mul(matOldWV, g_ProjMatrix);
    
    Output.vPos = mul(vector(Input.vPos, 1.f), matWVP);
    Output.vNor = normalize(mul(vector(Input.vNor, 0.f), g_WorldMatrix));
    Output.vTex = Input.vTex;
    Output.vProjPos = Output.vPos;
    Output.vTangent = normalize(mul(vector(1.f, 0.f, .0f, 0.f), g_WorldMatrix)).xyz;
    Output.vBinormal = normalize(cross(Output.vNor.xyz, Output.vTangent));
    Output.vOldPos = mul(vector(Input.vPos, 1.f), matOldWVP);
    
    matrix matWRV, matWRVP;
    
    matWRV = mul(g_WorldMatrix, g_ReflectionViewMatrix);
    matWRVP = mul(matWRV, g_ProjMatrix);
        
    Output.vReflectionPos = mul(vector(Input.vPos, 1.f), matWRVP);
    Output.vRefractionPos = Output.vPos;
    
    return Output;
}

struct PS_IN_WATER
{
    vector vPos : SV_POSITION;
    vector vNor : Normal;
    float2 vTex : TEXCOORD0;
    vector vProjPos : PROJPOS;
    float3 vTangent : Tangent;
    float3 vBinormal : Binormal;
    vector vReflectionPos : REFLECTION;
    vector vRefractionPos : REFRACTION;
    
    vector vOldPos : OLDPOS;
};

struct PS_OUT_WATER
{
    vector vDiffuse : SV_Target0;
    vector vNormal : SV_Target1;
    vector vDepth : SV_Target2;
    vector vRimMask : SV_Target3;
};

PS_OUT_WATER PS_MAIN_Water(PS_IN_WATER Input)
{
    PS_OUT_WATER Output = (PS_OUT_WATER) 0;
    
    
    vector vNormalDesc;
    
    float2 vWaterTex = Input.vTex + g_fWaterPos;
    
    
    float3 vNormal;
    // 반사와 굴절
    vNormalDesc = g_NormalTexture.Sample(LinearSampler, vWaterTex * 2.f);
    vNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);
    
    float3x3 WorldMatrix = float3x3(Input.vTangent, Input.vBinormal, Input.vNor.xyz);
    
    vNormal = normalize(mul(vNormal.xyz, WorldMatrix) * -1.f);
    
    //vNormal = normalize(mul(vNormal, g_WorldMatrix) * -1.f);
    
    float2 vReflectionTexcoord;
    
    vReflectionTexcoord.x = Input.vReflectionPos.x / Input.vReflectionPos.w / 2.f + 0.5f;
    vReflectionTexcoord.y = -Input.vReflectionPos.y / Input.vReflectionPos.w / 2.f + 0.5f;
    
    //float2 vRefractionTexcoord;
    
    //vRefractionTexcoord.x = Input.vRefractionPos.x / Input.vRefractionPos.w / 2.f + 0.5f;
    //vRefractionTexcoord.y = -Input.vRefractionPos.y / Input.vRefractionPos.w / 2.f + 0.5f;
    
    vReflectionTexcoord += vNormal.xy * g_fReflectionScale;
    //vRefractionTexcoord += vNormal.xy * g_fRefractionScale;
    
    vector vReflectionDiffuse;
    
    vReflectionDiffuse = g_ReflectionTexture.Sample(LinearClampSampler, vReflectionTexcoord);
    
    vReflectionDiffuse.rgb = pow(vReflectionDiffuse.rgb, 1.f / 2.2f);
    
    //vector vRefractionDiffuse;
    //vRefractionDiffuse = g_RefractionTexture.Sample(LinearMirrorSampler, vRefractionTexcoord);
    
    // 프레넬(반사율과 투과율 계산)

    //vector vLook = normalize(Input.vProjPos - g_vCamPos);
    
    //float fFresnel = 0.02f + 0.97f * pow((1.f - dot(vLook, vNormal)), 5.f);
    
    //vector vMergeDiffuse = lerp(vReflectionDiffuse, vRefractionDiffuse, fFresnel);
    
   // vector vMergeDiffuse = (1.f - fFresnel) * vRefractionDiffuse + fFresnel * vReflectionDiffuse;
    //float4 vMergeDiffuse = vRefractionDiffuse * (1 - fFresnel) * vRefractionDiffuse.a * vReflectionDiffuse.a + vReflectionDiffuse * fFresnel * vReflectionDiffuse.a * vRefractionDiffuse.a;
    //vMergeDiffuse = saturate(vMergeDiffuse * float4(0.95f, 1.00f, 1.05f, 1.0f) + float4(0.15f, 0.15f, 0.15f, 0.0f));
    
    float2 Velocity = (Input.vProjPos.xy / Input.vProjPos.w) - (Input.vOldPos.xy / Input.vOldPos.w);
    
    float2 vCalDir;
    vCalDir = Velocity * 0.5f;
    vCalDir.y *= -1.f;
    
    Output.vDiffuse = vReflectionDiffuse;
    Output.vNormal = vector(vNormal * 0.5f + 0.5f, 1.f);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_vCamNF.y, vCalDir.x, vCalDir.y);
    
    return Output;
}

// 물 쉐이더
technique11 DefaultTechniqueShader_VtxTex
{
    pass Water
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_Water();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Water();
    }
}