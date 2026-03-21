#include "Engine_Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
texture2D g_MaskTexture;
texture2D g_MaskTexture2;
texture2D g_SkillReadyTexture;
texture2D g_TextureArray[12];
texture2D g_DissolveTexture;

bool g_isBlur;

bool g_bOn = { false };
int g_TexIndex;
vector g_vColor;
vector g_vSliceRect;
float g_fx = { 0.f };
float g_fy = { 0.f };
float g_fScrollRatio;
float g_fHpRatio;
float g_fAlpha;
float g_fTime;
float g_fDissolveRatio;
float g_fAmount;
float g_fBrightFactor = { 3.f };
float2 g_vRatio;
int2 g_vNumSprite;
uint g_iIndex;

float2 g_CamNF;


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

VS_OUT VS_Main_Dust(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
	
    vector vPosition = mul(float4(Input.vPos, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
	
    float2 vTex = Input.vTex;
    vTex.x += g_fx;
    vTex.y += g_fy;
    
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
    
    //float3 Color = g_vColor.rgb;
    //float fAlpha = g_vColor.a * vMask.r;
    
    ////float fWeight = pow(Input.LinearZ, -2.5f);
    
    ////float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 6.f)), 0.01f, 3e3); // 1e-5 = 0.00001, 3e3 = 3000
    
    ////fWeight = max(fWeight, 1.f);
    
    //float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 4.f)), 1e-2, 3e3);
    //fWeight = max(min(1.f, max(max(Color.r, Color.g), Color.b) * fAlpha), fAlpha) * fWeight;
    
    //Output.vColor = vector(Color * fAlpha, fAlpha) * fWeight;
    //Output.vAlpha = vector(fAlpha, fAlpha, fAlpha, fAlpha);
    
    
    //Output.vBlur = vector(Color, fAlpha);
    Output.vColor = g_vColor;
    
    Output.vColor.a = Output.vColor.a * vMask.r;
    
    return Output;
}

PS_OUT PS_MaskColorAlpha(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex).a * g_MaskTexture.Sample(LinearSampler, Input.vTex).r;
    
    Output.vColor = g_vColor;
    
    Output.vColor.a = Output.vColor.a * vMask * g_fAlpha;
    
    return Output;
}

PS_OUT PS_Main_HP(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_Texture.Sample(LinearSampler, float2(Input.vTex));
    vector vMask = g_MaskTexture.Sample(LinearSampler, float2(Input.vTex.x + g_fTime, Input.vTex.y));
    Output.vColor.a = Output.vColor.a * vMask.r;
    if (Input.vTex.x > g_fHpRatio)
    {
        discard;
    }

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
    
    Output.vColor.a = vMask.r * vMask.a;
    
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
    
    Output.vColor.a = vMask.r * vMask.a;
    
    return Output;
}

PS_OUT PS_Main_Hell(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_vColor;
    
    return Output;
}


PS_OUT PS_MaskTexture_Dissolve(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float fDissolve = g_DissolveTexture.Sample(LinearSampler, Input.vTex).r;
    float fAlpha = saturate((fDissolve - g_fDissolveRatio) * 10.0f);
    if (fAlpha <= 0.f)
    {
        discard;
    }

    Output.vColor = g_Texture.Sample(LinearSampler, Input.vTex);
    vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex);
    
    Output.vColor.a = Output.vColor.a * vMask.r * fAlpha;
    
    return Output;
}

PS_OUT PS_InvMaskTexture_Dissolve(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float fDissolve = g_DissolveTexture.Sample(LinearSampler, Input.vTex).r;
    float fAlpha = saturate((fDissolve - g_fDissolveRatio) * 10.0f);
    if (fAlpha <= 0.f)
    {
        discard;
    }

    Output.vColor = g_Texture.Sample(LinearSampler, Input.vTex);
    if (Input.vTex.y < g_fHpRatio + 0.5f)
    {
        Output.vColor.xyz -= 0.9f;
    }
    Output.vColor.xyz += g_SkillReadyTexture.Sample(LinearClampSampler, Input.vTex - float2(0.f, g_fHpRatio)).xyz;
    float fMask = g_MaskTexture.Sample(LinearSampler, Input.vTex).r;
    
    Output.vColor.a = Output.vColor.a * (1.f - fMask) * fAlpha;
    
    return Output;
}

PS_OUT PS_MaskColor_Dissolve(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float fDissolve = g_DissolveTexture.Sample(LinearSampler, Input.vTex).r;
    float fAlpha = saturate((fDissolve - g_fDissolveRatio) * 10.0f);
    if (fAlpha <= 0.f)
    {
        discard;
    }

    vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex);
    
    Output.vColor = g_vColor;
    
    Output.vColor.a = Output.vColor.a * vMask.r * fAlpha;
    
    return Output;
}

PS_OUT PS_Main_Sprite_Dissolve(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float fDissolve = g_DissolveTexture.Sample(LinearSampler, Input.vTex).r;
    float fAlpha = saturate((fDissolve - g_fDissolveRatio) * 10.0f);
    if (fAlpha <= 0.f)
    {
        discard;
    }

    float2 vSpriteSize = float2(1.f, 1.f) / g_vNumSprite;
    int2 vSpriteCoord;
    vSpriteCoord.x = g_iIndex % g_vNumSprite.x;
    vSpriteCoord.y = g_iIndex / g_vNumSprite.x;
    float2 vUV = Input.vTex / g_vNumSprite + (vSpriteSize * vSpriteCoord);
    
    Output.vColor = g_Texture.Sample(LinearSampler, vUV);
    
    //Output.vColor.a = Output.vColor.r
    
    Output.vColor.a = fAlpha;
    
    if (Output.vColor.a < 0.1f)
    {
        discard;
    }

    return Output;
}

PS_OUT PS_Main_Sprite_MaskTexture_Dissolve(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float fDissolve = g_DissolveTexture.Sample(LinearSampler, Input.vTex).r;
    float fAlpha = saturate((fDissolve - g_fDissolveRatio) * 10.0f);
    if (fAlpha <= 0.f)
    {
        discard;
    }

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
    
    Output.vColor.a = vMask.r * vMask.a * fAlpha;
    
    return Output;
}

PS_OUT PS_Main_Sprite_MaskColor_Dissolve(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float fDissolve = g_DissolveTexture.Sample(LinearSampler, Input.vTex).r;
    float fAlpha = saturate((fDissolve - g_fDissolveRatio) * 10.0f);
    if (fAlpha <= 0.f)
    {
        discard;
    }

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
    
    Output.vColor.a = vMask.r * vMask.a * fAlpha;
    
    return Output;
}

PS_OUT PS_Main_Dust(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_Texture.Sample(LinearSampler, Input.vTex);
    
    return Output;
}

PS_OUT PS_Main_Dissolve(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float fDissolve = g_DissolveTexture.Sample(LinearSampler, Input.vTex).r;
    float fAlpha = saturate((fDissolve - g_fDissolveRatio) * 10.0f);
    if (fAlpha <= 0.f)
    {
        discard;
    }

    Output.vColor = g_Texture.Sample(LinearSampler, Input.vTex);
    Output.vColor.a *= fAlpha;
    
    return Output;
}
PS_OUT PS_Main_MP(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_Texture.Sample(LinearSampler, Input.vTex);
    vector vMask = g_MaskTexture.Sample(LinearSampler, float2(Input.vTex.x + g_fTime, Input.vTex.y));
    Output.vColor.a = Output.vColor.a * vMask.r;
    if (Input.vTex.x < 1.f - g_fHpRatio)
    {
        discard;
    }

    return Output;
}

PS_OUT PS_Main_BLUR(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;


    float4 texColor = g_Texture.Sample(LinearSampler, Input.vTex);
    float brightness = texColor.r + texColor.g + texColor.b;

    float threshold = 0.9;


    if (brightness > threshold)
    {
        texColor *= 3.f;
    }


    float edgeThreshold = 0.1;
    float edge = 1.0 - smoothstep(1.0 - edgeThreshold, 1.0 + edgeThreshold, texColor.a);

    float4 blurColor = float4(0.0, 0.0, 0.0, 0.0);
    float totalWeight = 0.0;

    for (int i = -2; i <= 2; ++i)
    {
        for (int j = -2; j <= 2; ++j)
        {
            float2 offset = float2(i, j) * g_fAmount;
            float weight = 8.f;
            blurColor += g_Texture.Sample(LinearSampler, Input.vTex + offset) * weight;
            totalWeight += weight;
        }
    }

    blurColor /= totalWeight;

    blurColor.rgb *= g_vColor.rgb;


    Output.vColor = lerp(texColor, blurColor, edge);

    return Output;
}
PS_OUT PS_Bright(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_Texture.Sample(LinearSampler, Input.vTex);
    vector vMask = g_MaskTexture.Sample(LinearSampler, float2(Input.vTex.x + g_fx, Input.vTex.y + g_fy));
    
    Output.vColor.a = Output.vColor.a * vMask.r;


    Output.vColor *= g_fBrightFactor;
  
    
    return Output;
}
PS_OUT PS_ScrollAlpha(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_Texture.Sample(LinearSampler, Input.vTex);
    
    Output.vColor.a *= g_fAlpha;
    if (g_fScrollRatio < 0)
    {
        if (Input.vTex.y < (g_fScrollRatio * -1.f))
        {
            discard;
        }
    }
    if (g_fScrollRatio > 0)
    {
        if (Input.vTex.y > g_fScrollRatio)
        {
            discard;
        }
    }

    return Output;
}
PS_OUT PS_Main_HPNoMask(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_Texture.Sample(LinearSampler, Input.vTex);
    if (Input.vTex.x > g_fHpRatio)
    {
        discard;
    }

    return Output;
}

PS_OUT PS_MaskTexture_DissolveMove(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float fDissolve = g_DissolveTexture.Sample(LinearSampler, Input.vTex).r;
    
    if (g_fDissolveRatio > fDissolve)
    {
        discard;
    }
    
    Output.vColor = g_Texture.Sample(LinearSampler, float2(Input.vTex.x + g_fx, Input.vTex.y + g_fy));
    vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex);
    
    Output.vColor.a = Output.vColor.a * vMask.r;
    
    return Output;
}

PS_OUT PS_Main_FadeVertical(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float fBaseAlpha = 0.9f;
    Output.vColor = g_Texture.Sample(LinearSampler, Input.vTex) * vector(0.f, 0.f, 0.f, 1.f);

    if (Input.vTex.y < 0.3f)
    {
        Output.vColor.a = Input.vTex.y * (3.f);
    }
    else if (Input.vTex.y > 0.6f)
    {
        Output.vColor.a = fBaseAlpha + (fBaseAlpha - Input.vTex.y * (3.f / 2.f));
    }
    else
        Output.vColor.a = fBaseAlpha;

    Output.vColor.a *= 2.f / 3.f;
    
    return Output;

}
PS_OUT PS_Main_FadeHorizontal(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    float fBaseAlpha = 0.9f;
    Output.vColor = g_Texture.Sample(LinearSampler, Input.vTex) * vector(0.f, 0.f, 0.f, 1.f);

    if (Input.vTex.x < 0.3f)
    {
        Output.vColor.a = Input.vTex.x * (0.9f / 0.2f);
    }
    else if (Input.vTex.x > 0.8f)
    {
        Output.vColor.a = fBaseAlpha + (fBaseAlpha / 0.2f) * (0.8f - Input.vTex.x);
    }
    else
        Output.vColor.a = fBaseAlpha;

    return Output;

}

PS_OUT PS_Main_LerpColorNAlpha(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_Texture.Sample(LinearSampler, Input.vTex);
    if (g_vColor.a != 0.f)
    {
        Output.vColor.xyz = lerp(Output.vColor, g_vColor, 0.6f);
    }

    if (Output.vColor.a > 0.1f)
    {
        Output.vColor.a = g_fAlpha;
    }

    //if (g_isBlur)
    //    Output.vBlurColor;
    //else
    //    Output.vColor;
    
    return Output;
}

PS_OUT PS_Main_HPBoss(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_Texture.Sample(LinearSampler, float2(Input.vTex));
    vector vMask = g_MaskTexture.Sample(LinearSampler, float2(Input.vTex.x + g_fTime, Input.vTex.y));
    Output.vColor.a = Output.vColor.a * vMask.r;
    if (Input.vTex.x > g_fHpRatio)
    {
        discard;
    }
    if (g_bOn)
    {
        Output.vColor.xyz *= 1.5f;

    }
    return Output;
}

PS_OUT PS_Main_MaskColorMove(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vMask = g_MaskTexture.Sample(LinearSampler, float2(Input.vTex.x + g_fx, Input.vTex.y + g_fy));
    
    Output.vColor = g_vColor;
    
    Output.vColor.a = Output.vColor.a * vMask.r;
    
    return Output;
}

PS_OUT PS_Main_ChangeBright(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_Texture.Sample(LinearSampler, Input.vTex);

    Output.vColor *= g_fTime;
        
    Output.vColor.a *= g_fAlpha;
    
    return Output;
}
PS_OUT PS_Main_Move(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_Texture.Sample(LinearSampler, float2(Input.vTex.x + g_fx, Input.vTex.y + g_fy));
    
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
    
    pass Mask_Texture_Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MaskTexture_Dissolve();
    }

    pass Inv_Mask_Texture_Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_InvMaskTexture_Dissolve();
    }

    pass Mask_Color_Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MaskColor_Dissolve();
    }

    pass Sprite_Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_Dissolve();
    }
    pass SpriteMaskTexture_Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_MaskTexture_Dissolve();
    }
    pass SpriteMaskColor_Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_MaskColor_Dissolve();
    }

    pass Dust
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main_Dust();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Dust();
    
    }
    pass Dissolve
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Dissolve();
    
    }
    pass MP
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_MP();
    }

    pass BLUR
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_BLUR();
    }
    pass Bright
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Bright();
    }
    pass ScrollAlpha
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_ScrollAlpha();
    }
    pass HPNoMask
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_HPNoMask();
    }
    pass MaskDissolveMove
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MaskTexture_DissolveMove();
    }
    pass FadeVertical
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_FadeVertical();
    }
    pass FadeHorizontal
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_FadeHorizontal();
    }
    pass LerpColorNAlpha
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_LerpColorNAlpha();
    }
    pass HPBoss
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_HPBoss();
    }
    pass MaskColorMove
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_MaskColorMove();
    }
    pass ChangeBright
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_ChangeBright();
    }
    pass Move
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Move();
    }
};