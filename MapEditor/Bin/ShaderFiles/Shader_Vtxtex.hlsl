#include "Engine_Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
texture2D g_MaskTexture;
texture2D g_MaskTexture2;
texture2D g_SkillReadyTexture;
texture2D g_TextureArray[12];
int g_TexIndex;
vector g_vColor;
float g_fx;
float g_fHpRatio;
float g_fAlpha;
int2 g_vNumSprite;
uint g_iIndex;

struct VS_IN
{
    float3 vPos : Position;
    float2 vTex : Texcoord0;
};

struct VS_OUT
{
    vector vPos : SV_Position; // == float4
    float2 vTex : Texcoord0;
};

VS_OUT VS_Main(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
	
    vector vPosition = mul(float4(Input.vPos, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
	
    Output.vPos = vPosition;
    Output.vTex = Input.vTex;
	
    return Output;
}
VS_OUT VS_Main_Mask(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
	
    vector vPosition = mul(float4(Input.vPos, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
	
    float2 vTex = Input.vTex;
    vTex.x += g_fx;
    
    Output.vPos = vPosition;
    Output.vTex = vTex;
	
    return Output;
}
struct PS_IN
{
    vector vPos : SV_Position;
    float2 vTex : Texcoord0;
};

struct PS_OUT
{
    vector vColor : SV_Target0;
};

PS_OUT PS_Main(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_Texture.Sample(LinearSampler, Input.vTex);
    
    return Output;
}

PS_OUT PS_Main_Alpha(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_Texture.Sample(LinearSampler, Input.vTex);
    
    Output.vColor.a *= g_fAlpha;
    
    return Output;
}

PS_OUT PS_Main_Color_Alpha(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor.xyz = g_vColor.xyz;
    Output.vColor.a = g_fAlpha;

    return Output;
}

PS_OUT PS_Main_Button(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vHighlight = g_TextureArray[1].Sample(LinearSampler, Input.vTex) * g_TexIndex;
    vHighlight.a = 0.f;
    
    Output.vColor = g_TextureArray[0].Sample(LinearSampler, Input.vTex) + vHighlight;
    
    return Output;
}

PS_OUT PS_MaskTexture(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_Texture.Sample(LinearSampler, Input.vTex);
    vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex);
    
    Output.vColor.a = Output.vColor.a * vMask.r;
    
    return Output;
}

PS_OUT PS_InvMaskTexture(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_Texture.Sample(LinearSampler, Input.vTex);
    if (Input.vTex.y < g_fHpRatio + 0.5f)
    {
        Output.vColor.xyz -= 0.9f;
    }
    Output.vColor.xyz += g_SkillReadyTexture.Sample(LinearClampSampler, Input.vTex - float2(0.f, g_fHpRatio)).xyz;
    float fMask = g_MaskTexture.Sample(LinearSampler, Input.vTex).r;
    
    Output.vColor.a = Output.vColor.a * 1.f - fMask;
    
    return Output;
}

PS_OUT PS_MaskColor(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex);
    
    Output.vColor = g_vColor;
    
    Output.vColor.a = Output.vColor.a * vMask.r;
    
    return Output;
}

PS_OUT PS_MaskColorAlpha(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex);
    
    Output.vColor = g_vColor;
    
    Output.vColor.a = Output.vColor.a * vMask.r * g_fAlpha;
    
    return Output;
}

PS_OUT PS_Main_HP(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex);
    vector vMask2 = g_MaskTexture2.Sample(LinearClampSampler, float2(Input.vTex.x - (g_fHpRatio - 0.5f), Input.vTex.y));
    Output.vColor = g_vColor;
    
    Output.vColor.a = (Output.vColor.a * vMask.r) * vMask2.r;
    
    return Output;
}

PS_OUT PS_Main_Hit(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_Texture.Sample(LinearSampler, Input.vTex);
    
    Output.vColor.a = Output.vColor.a * g_fAlpha;
    
    return Output;
}

PS_OUT PS_Main_Sprite(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float2 vSpriteSize = float2(1.f, 1.f) / g_vNumSprite;
    int2 vSpriteCoord;
    vSpriteCoord.x = g_iIndex % g_vNumSprite.x;
    vSpriteCoord.y = g_iIndex / g_vNumSprite.x;
    float2 vUV = Input.vTex / g_vNumSprite + (vSpriteSize * vSpriteCoord);
    
    Output.vColor = g_Texture.Sample(LinearSampler, vUV);
    
    //Output.vColor.a = Output.vColor.r
    
    if (Output.vColor.a < 0.1f)
    {
        discard;
    }

    return Output;
}

PS_OUT PS_Main_Sprite_MaskTexture(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float2 vSpriteSize = float2(1.f, 1.f) / g_vNumSprite;
    int2 vSpriteCoord;
    vSpriteCoord.x = g_iIndex % g_vNumSprite.x;
    vSpriteCoord.y = g_iIndex / g_vNumSprite.x;
    float2 vUV = Input.vTex / g_vNumSprite + (vSpriteSize * vSpriteCoord);
    
    vector vMask = g_MaskTexture.Sample(LinearSampler, vUV);
    vMask.r = max(vMask.r, vMask.g);
    vMask.r = max(vMask.r, vMask.b);
    
    if (vMask.r < 0.1f)
    {
        discard;
    }

    Output.vColor = g_Texture.Sample(LinearSampler, vUV);
    
    Output.vColor.a = vMask.r;
    
    return Output;
}

PS_OUT PS_Main_Sprite_MaskColor(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float2 vSpriteSize = float2(1.f, 1.f) / g_vNumSprite;
    int2 vSpriteCoord;
    vSpriteCoord.x = g_iIndex % g_vNumSprite.x;
    vSpriteCoord.y = g_iIndex / g_vNumSprite.x;
    float2 vUV = Input.vTex / g_vNumSprite + (vSpriteSize * vSpriteCoord);
    
    vector vMask = g_MaskTexture.Sample(LinearSampler, vUV);
    vMask.r = max(vMask.r, vMask.g);
    vMask.r = max(vMask.r, vMask.b);
    
    if (vMask.r < 0.1f)
    {
        discard;
    }

    Output.vColor = g_vColor;
    
    Output.vColor.a = vMask.r;
    
    return Output;
}

PS_OUT PS_Main_Hell(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_vColor;
    
    return Output;
}



technique11 DefaultTechnique
{
    pass UI
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main();
    }

    pass UI_Alpha
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Alpha();
    }

    pass UI_Color_Alpha
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Color_Alpha();
    }

    pass Button
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Button();
    }

    pass Background
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main();
    }

    pass BackgroundMask
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main_Mask();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main();
    }
    pass Mask_Texture
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MaskTexture();
    }

    pass Inv_Mask_Texture
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_InvMaskTexture();
    }

    pass Mask_Color
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MaskColor();
    }

    pass Mask_ColorAlpha
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MaskColorAlpha();
    }

    pass HP
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_HP();
    }

    pass Hit
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Hit();
    }

    pass Sprite
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite();
    }
    pass SpriteMaskTexture
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_MaskTexture();
    }
    pass SpriteMaskColor
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_MaskColor();
    }
    pass Hell
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Hell();
    }
};