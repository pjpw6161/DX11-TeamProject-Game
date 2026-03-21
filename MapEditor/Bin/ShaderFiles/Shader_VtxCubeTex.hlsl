#include "Engine_Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
textureCUBE g_Texture;

vector g_vLightDir;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1.f);
vector g_vMtrlSpecular = vector(0.7f, 1.0f, 0.7f, 1.f);

vector g_vCamPos;

struct VS_IN
{
    float3 vPos : Position;
    float3 vNor : Normal;
    float3 vTex : Texcoord0;
};

struct VS_OUT
{
    vector vPos : SV_Position; // == float4
    vector vNor : Normal;
    float3 vTex : Texcoord0;
    vector vWorldPos : Texcoord1;
};

VS_OUT VS_Main(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
	
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    Output.vPos = mul(vector(Input.vPos, 1.f), matWVP);
    Output.vNor = mul(vector(Input.vNor, 0.f), g_WorldMatrix);
    Output.vTex = Input.vTex;
    Output.vWorldPos = mul(vector(Input.vPos, 1.f), g_WorldMatrix);
	
    return Output;
}

struct PS_IN
{
    vector vPos : SV_Position;
    vector vNor : Normal;
    float3 vTex : Texcoord0;
    vector vWorldPos : Texcoord1;
};

struct PS_OUT
{
    vector vColor : SV_Target0;
};

PS_OUT PS_Main(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_Texture.Sample(PointSampler, Input.vTex);
    
    float fShade = saturate(dot(normalize(g_vLightDir) * -1.f, Input.vNor));
    
    vector vReflect = reflect(normalize(g_vLightDir), Input.vNor);
    vector vLook = Input.vWorldPos - g_vCamPos;
    float fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 10.f) * 0.3f;

    Output.vColor = (g_vLightDiffuse * vMtrlDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlAmbient)) + ((g_vLightSpecular * g_vMtrlSpecular) * fSpecular);
    
    //Output.vColor = g_Texture.Sample(PointSampler, Input.vTex);
    return Output;
}

technique11 DefaultTechniqueShader_VtxNorTex
{
    pass Cube
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
};