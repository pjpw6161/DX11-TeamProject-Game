#include "Engine_Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
texture2D g_Texture_Cursor;

vector g_vCursorPos = vector(15.f, 0.f, 15.f, 1.f);
float g_fCursorRange = 1.f;

//vector g_vLightDir;

//vector g_vLightDiffuse;
//vector g_vLightAmbient;
//vector g_vLightSpecular;

vector g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1.f);
vector g_vMtrlSpecular = vector(0.7f, 1.0f, 0.7f, 1.f);

vector g_vCamPos;
float g_fCamFar;

// ¿ø¸í
matrix g_ReflectionViewMatrix;
texture2D g_ReflectionTexture;

struct VS_IN
{
    float3 vPos : Position;
    float3 vNor : Normal;
    float2 vTex : Texcoord0;
};

struct VS_OUT
{
    vector vPos : SV_Position; // == float4
    vector vNor : Normal;
    float2 vTex : Texcoord0;
    vector vWorldPos : Texcoord1;
    vector vProjPos : Texcoord2;
};

VS_OUT VS_Main(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
	
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    Output.vPos = mul(vector(Input.vPos, 1.f), matWVP);
    Output.vNor = normalize(mul(vector(Input.vNor, 0.f), g_WorldMatrix));
    Output.vTex = Input.vTex;
    Output.vWorldPos = mul(vector(Input.vPos, 1.f), g_WorldMatrix);
    Output.vProjPos = Output.vPos;
    
    return Output;
}

struct PS_IN
{
    vector vPos : SV_Position;
    vector vNor : Normal;
    float2 vTex : Texcoord0;
    vector vWorldPos : Texcoord1;
    vector vProjPos : Texcoord2;
};

struct PS_OUT
{
    //vector vColor : SV_Target0;
    vector vDiffuse : SV_Target0;
    vector vNormal : SV_Target1;
    vector vDepth : SV_Target2;
};

PS_OUT PS_Main(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_Texture.Sample(LinearSampler, Input.vTex * 100.f);
    
    //float fShade = saturate(dot(normalize(g_vLightDir) * -1.f, Input.vNor));
    float fShade = -1.f * Input.vNor;
    
    //vector vReflect = reflect(normalize(g_vLightDir), Input.vNor);
    vector vLook = Input.vWorldPos - g_vCamPos;
    //float fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 10.f) * 0.3f;

    //Output.vDiffuse = (g_vLightDiffuse * vMtrlDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlAmbient)) + ((g_vLightSpecular * g_vMtrlSpecular) * fSpecular);
    Output.vDiffuse = vMtrlDiffuse *  g_vMtrlAmbient +  g_vMtrlSpecular;
    Output.vNormal = Input.vNor;
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_fCamFar, 0.f, 0.f);
    
    return Output;
}

PS_OUT PS_Main_Editor(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_Texture.Sample(LinearSampler, Input.vTex * 100.f);
    vector vCursorDiffuse = vector(0.f, 0.f, 0.f, 0.f);

    if (Input.vWorldPos.x >= g_vCursorPos.x - g_fCursorRange && Input.vWorldPos.x < g_vCursorPos.x + g_fCursorRange &&
        Input.vWorldPos.z >= g_vCursorPos.z - g_fCursorRange && Input.vWorldPos.z < g_vCursorPos.z + g_fCursorRange)
    {
        float2 vCursorUV;
        vCursorUV.x = (Input.vWorldPos.x - (g_vCursorPos.x - g_fCursorRange)) / g_fCursorRange * 2.f;
        vCursorUV.y = (Input.vWorldPos.z - (g_vCursorPos.z + g_fCursorRange)) / g_fCursorRange * -2.f;
        vCursorDiffuse = g_Texture_Cursor.Sample(LinearSampler, vCursorUV / 4);
    }
    
    //float fShade = saturate(dot(normalize(g_vLightDir) * -1.f, Input.vNor));
    
    //vector vReflect = reflect(normalize(g_vLightDir), Input.vNor);
    vector vLook = Input.vWorldPos - g_vCamPos;
    //float fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 10.f) * 0.3f;

    //Output.vDiffuse = (g_vLightDiffuse * vMtrlDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlAmbient)) + ((g_vLightSpecular * g_vMtrlSpecular) * fSpecular) + vCursorDiffuse;
    Output.vDiffuse =  vMtrlDiffuse *  g_vMtrlAmbient + g_vMtrlSpecular + vCursorDiffuse;
    Output.vNormal = Input.vNor;
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_fCamFar, 0.f, 0.f);
    return Output;
}

technique11 DefaultTechniqueShader_VtxNorTex
{
    pass Terrain
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

    pass Terrain_Editor
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Editor();
    }
    pass Terrain_WireFrame
    {
        SetRasterizerState(RS_Wire);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Editor();
    }

};