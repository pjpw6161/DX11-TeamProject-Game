#include "Engine_Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

struct VS_IN
{
    float3 vPos : Position;
    float3 vColor : Color;
};

struct VS_OUT
{
    vector vPos : SV_Position;
    float3 vColor : Color;
};

VS_OUT VS_Main(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
	
    vector vPosition = mul(float4(Input.vPos, 1.f), g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
	
    Output.vPos = vPosition;
    Output.vColor = Input.vColor;
    
    return Output;
}

struct PS_IN
{
    vector vPos : SV_Position;
    float3 vColor : Color;
};

struct PS_OUT
{
    vector vColor : SV_Target0;
};

PS_OUT PS_Main(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = vector(Input.vColor, 1.f);
    
    return Output;
}

technique11 DefaultTechnique
{
    pass PhysX
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