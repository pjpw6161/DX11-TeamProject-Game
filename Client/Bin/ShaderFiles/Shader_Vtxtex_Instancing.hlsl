#include "Engine_Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture, g_MaskTexture, g_DissolveTexture;
vector g_vCamPos;
vector g_vColor;
int2 g_vNumSprite;
uint g_iIndex;
float g_fDissolveRatio;
float g_fAlpha;

float2 g_CamNF;

bool g_isBlur;

Texture2D g_DistortionTexture;

struct VS_IN
{
    float3 vPos : Position;
    float2 vPSize : PSize;
    
    row_major matrix TransformMatrix : World;
    vector vPrevPos : PrevPosition;
    float fIndex : Index;
    float fDissolveRatio : Dissolve;
    
    uint iSVInstanceID : SV_InstanceID;
};

struct VS_OUT
{
    vector vPos : Position;
    float2 vPSize : PSize;
    uint iIndex : Index;
    vector vPrevPos : PrevPosition;
    float fDissolveRatio : Dissolve;
};

VS_OUT VS_Main(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
    
    matrix matWV, matWVP;
    
    vector vPosition = mul(float4(Input.vPos, 1.f), Input.TransformMatrix);
    
    Output.vPos = mul(vPosition, g_WorldMatrix);
    Output.vPSize = float2(Input.vPSize.x * Input.TransformMatrix._11, Input.vPSize.y * Input.TransformMatrix._22);
    Output.iIndex = uint(Input.fIndex * (g_vNumSprite.x * g_vNumSprite.y));
    Output.vPrevPos = Input.vPrevPos;
    Output.fDissolveRatio = 1.f - Input.fDissolveRatio;
    
    return Output;
}

VS_OUT VS_Main_RandomIndex(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
    
    matrix matWV, matWVP;
    
    vector vPosition = mul(float4(Input.vPos, 1.f), Input.TransformMatrix);
    
    Output.vPos = mul(vPosition, g_WorldMatrix);
    Output.vPSize = float2(Input.vPSize.x * Input.TransformMatrix._11, Input.vPSize.y * Input.TransformMatrix._22);
    Output.iIndex = Input.iSVInstanceID;
    Output.vPrevPos = Input.vPrevPos;
    Output.fDissolveRatio = 1.f - Input.fDissolveRatio;
    
    return Output;
}

VS_OUT VS_Main_RandomSprite(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
    
    matrix matWV, matWVP;
    
    vector vPosition = mul(float4(Input.vPos, 1.f), Input.TransformMatrix);
    
    Output.vPos = mul(vPosition, g_WorldMatrix);
    Output.vPSize = float2(Input.vPSize.x * Input.TransformMatrix._11, Input.vPSize.y * Input.TransformMatrix._22);
    Output.iIndex = uint(Input.fIndex * (g_vNumSprite.x * g_vNumSprite.y));
    Output.vPrevPos = Input.vPrevPos;
    Output.fDissolveRatio = 1.f - Input.fDissolveRatio;
    
    return Output;
}

struct GS_IN
{
    vector vPos : Position;
    float2 vPSize : PSize;
    uint iIndex : Index;
    vector vPrevPos : PrevPosition;
    float fDissolveRatio : Dissolve;
};

struct GS_OUT
{
    vector vPos : SV_Position;
    float2 vTex : Texcoord0;
    uint iIndex : Index;
    float fDissolveRatio : Dissolve;
    float LinearZ : LINEARZ;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN Input[1], inout TriangleStream<GS_OUT> Triangles)
{
    GS_OUT Output[4];
    
    float3 vLook = (g_vCamPos - Input[0].vPos).xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * Input[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * Input[0].vPSize.y * 0.5f;
    
    Output[0].vPos = vector(Input[0].vPos.xyz + vRight + vUp, 1.f);
    Output[0].vTex = float2(0.f, 0.f);

    Output[1].vPos = vector(Input[0].vPos.xyz - vRight + vUp, 1.f);
    Output[1].vTex = float2(1.f, 0.f);

    Output[2].vPos = vector(Input[0].vPos.xyz - vRight - vUp, 1.f);
    Output[2].vTex = float2(1.f, 1.f);

    Output[3].vPos = vector(Input[0].vPos.xyz + vRight - vUp, 1.f);
    Output[3].vTex = float2(0.f, 1.f);
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    /////////////////////
    Output[0].vPos = mul(Output[0].vPos, matVP);
    Output[0].LinearZ = Output[0].vPos.w;
    
    /////////////////////
    Output[1].vPos = mul(Output[1].vPos, matVP);
    Output[1].LinearZ = Output[1].vPos.w;
    
    /////////////////////
    Output[2].vPos = mul(Output[2].vPos, matVP);
    Output[2].LinearZ = Output[2].vPos.w;
    
    /////////////////////
    Output[3].vPos = mul(Output[3].vPos, matVP);
    Output[3].LinearZ = Output[3].vPos.w;

    Output[0].iIndex = Input[0].iIndex;
    Output[1].iIndex = Input[0].iIndex;
    Output[2].iIndex = Input[0].iIndex;
    Output[3].iIndex = Input[0].iIndex;

    Output[0].fDissolveRatio = Input[0].fDissolveRatio;
    Output[1].fDissolveRatio = Input[0].fDissolveRatio;
    Output[2].fDissolveRatio = Input[0].fDissolveRatio;
    Output[3].fDissolveRatio = Input[0].fDissolveRatio;

    Triangles.Append(Output[0]);
    Triangles.Append(Output[1]);
    Triangles.Append(Output[2]);
    Triangles.RestartStrip();
    
    Triangles.Append(Output[0]);
    Triangles.Append(Output[2]);
    Triangles.Append(Output[3]);
}

[maxvertexcount(6)]
void GS_MAIN_Trail(point GS_IN Input[1], inout TriangleStream<GS_OUT> Triangles)
{
    GS_OUT Output[4];
    
    float3 vLook = normalize((g_vCamPos - Input[0].vPrevPos).xyz);
    float3 vMove = (Input[0].vPos - Input[0].vPrevPos).xyz;
    float3 vRight = normalize(cross(vMove, vLook)) * Input[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * Input[0].vPSize.y * 0.5f;
    
    Output[0].vPos = vector(Input[0].vPos.xyz + vRight + vUp, 1.f);
    Output[0].vTex = float2(0.f, 0.f);

    Output[1].vPos = vector(Input[0].vPos.xyz - vRight + vUp, 1.f);
    Output[1].vTex = float2(1.f, 0.f);

    Output[2].vPos = vector(Input[0].vPrevPos.xyz - vRight - vUp, 1.f);
    Output[2].vTex = float2(1.f, 1.f);

    Output[3].vPos = vector(Input[0].vPrevPos.xyz + vRight - vUp, 1.f);
    Output[3].vTex = float2(0.f, 1.f);
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    /////////////////////
    Output[0].vPos = mul(Output[0].vPos, matVP);
    Output[0].LinearZ = Output[0].vPos.w;
    
    /////////////////////
    Output[1].vPos = mul(Output[1].vPos, matVP);
    Output[1].LinearZ = Output[1].vPos.w;
    
    /////////////////////
    Output[2].vPos = mul(Output[2].vPos, matVP);
    Output[2].LinearZ = Output[2].vPos.w;
    
    /////////////////////
    Output[3].vPos = mul(Output[3].vPos, matVP);
    Output[3].LinearZ = Output[3].vPos.w;

    Output[0].iIndex = Input[0].iIndex;
    Output[1].iIndex = Input[0].iIndex;
    Output[2].iIndex = Input[0].iIndex;
    Output[3].iIndex = Input[0].iIndex;

    Output[0].fDissolveRatio = Input[0].fDissolveRatio;
    Output[1].fDissolveRatio = Input[0].fDissolveRatio;
    Output[2].fDissolveRatio = Input[0].fDissolveRatio;
    Output[3].fDissolveRatio = Input[0].fDissolveRatio;

    Triangles.Append(Output[0]);
    Triangles.Append(Output[1]);
    Triangles.Append(Output[2]);
    Triangles.RestartStrip();
    
    Triangles.Append(Output[0]);
    Triangles.Append(Output[2]);
    Triangles.Append(Output[3]);
}

struct PS_IN
{
    vector vPos : SV_Position;
    float2 vTex : Texcoord0;
    uint iIndex : Index;
    float fDissolveRatio : Dissolve;
    float LinearZ : LINEARZ;
};


struct PS_OUT
{
    vector vColor : SV_Target0;
    vector vAlpha : SV_Target1;
    vector vBlur : SV_Target2;
};

struct PS_OUT_DISTORTION
{
    vector vDistortion : SV_Target0;
};

PS_OUT PS_Main(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vColor = g_Texture.Sample(LinearSampler, Input.vTex);
    vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex);
    
    float3 Color = vColor.rgb;
    float fAlpha = vMask.r * Input.fDissolveRatio;
    
    //float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 6.f)), 0.01f, 3e3); // 1e-5 = 0.00001, 3e3 = 3000
    
    //fWeight = max(fWeight, 1.f);
    
    //float fWeight = pow(Input.LinearZ, -2.5f);
    float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 4.f)), 1e-2, 3e3);
    fWeight = max(min(1.f, max(max(Color.r, Color.g), Color.b) * fAlpha), fAlpha) * fWeight;
    
    Output.vColor = vector(Color * fAlpha, fAlpha) * fWeight;
    Output.vAlpha = fAlpha;
    Output.vBlur = vector(Color, fAlpha) * g_isBlur;
    
    //Output.vColor.a = vMask.r * Input.fDissolveRatio;
    
    return Output;
}

PS_OUT PS_Main_Sprite(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float2 vSpriteSize = float2(1.f, 1.f) / g_vNumSprite;
    int2 vSpriteCoord;
    vSpriteCoord.x = Input.iIndex % g_vNumSprite.x;
    vSpriteCoord.y = Input.iIndex / g_vNumSprite.x;
    float2 vUV = Input.vTex / g_vNumSprite + (vSpriteSize * vSpriteCoord);
    
    vector vMask = g_MaskTexture.Sample(LinearSampler, vUV);
    if (vMask.r < 0.1f)
    {
        discard;
    }
    
    float3 Color = g_vColor.rgb;
    float fAlpha = g_vColor.a * vMask.r * Input.fDissolveRatio;
    
    //float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 6.f)), 0.01f, 3e3); // 1e-5 = 0.00001, 3e3 = 3000
    
    //fWeight = max(fWeight, 1.f);
    
    //float fWeight = pow(Input.LinearZ, -2.5f);
    
    float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 4.f)), 1e-2, 3e3);
    fWeight = max(min(1.f, max(max(Color.r, Color.g), Color.b) * fAlpha), fAlpha) * fWeight;
    
    Output.vColor = vector(Color * fAlpha, fAlpha) * fWeight;
    Output.vAlpha = fAlpha;
    Output.vBlur = vector(Color, fAlpha) * g_isBlur;

    //Output.vColor = g_vColor;
    
    //Output.vColor.a = vMask.r * Input.fDissolveRatio;
    
    return Output;
}

PS_OUT PS_Main_Color(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex);
    if (vMask.r < 0.1f)
    {
        discard;
    }
    
    float3 Color = g_vColor.rgb;
    float fAlpha = g_vColor.a * vMask.r * Input.fDissolveRatio;
    
    //float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 6.f)), 0.01f, 3e3); // 1e-5 = 0.00001, 3e3 = 3000
    
    //fWeight = max(fWeight, 1.f);
    
    //float fWeight = pow(Input.LinearZ, -2.5f);
    
    float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 4.f)), 1e-2, 3e3);
    fWeight = max(min(1.f, max(max(Color.r, Color.g), Color.b) * fAlpha), fAlpha) * fWeight;
    
    Output.vColor = vector(Color * fAlpha, fAlpha) * fWeight;
    Output.vAlpha = fAlpha;
    Output.vBlur = vector(Color, fAlpha) * g_isBlur;
    
        
    //Output.vColor = g_vColor;
    //Output.vColor.a = vMask.r * Input.fDissolveRatio;
    
    return Output;
}

PS_OUT PS_Main_Dissolve(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float fDissolve = g_DissolveTexture.Sample(LinearSampler, Input.vTex).r;
    float fAlpha = saturate((fDissolve - (g_fDissolveRatio + 1.f - Input.fDissolveRatio)) * 10.0f);
    if (fAlpha <= 0.f)
    {
        discard;
    }
    
    vector vColor = g_Texture.Sample(LinearSampler, Input.vTex);
    vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex);
    
    float3 Color = vColor.rgb;
    fAlpha = vMask.r * fAlpha;
    
    //float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 6.f)), 0.01f, 3e3); // 1e-5 = 0.00001, 3e3 = 3000
    
    //fWeight = max(fWeight, 1.f);
    
    //float fWeight = pow(Input.LinearZ, -2.5f);
    
    float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 4.f)), 1e-2, 3e3);
    fWeight = max(min(1.f, max(max(Color.r, Color.g), Color.b) * fAlpha), fAlpha) * fWeight;
    
    Output.vColor = vector(Color * fAlpha, fAlpha) * fWeight;
    Output.vAlpha = fAlpha;
    Output.vBlur = vector(Color, fAlpha) * g_isBlur;
    
    
    
    //Output.vColor.a = vMask.r;
    
    return Output;
}

PS_OUT PS_Main_Sprite_Dissolve(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float fDissolve = g_DissolveTexture.Sample(LinearSampler, Input.vTex).r;
    float fAlpha = saturate((fDissolve - (g_fDissolveRatio + 1.f - Input.fDissolveRatio)) * 10.0f);
    if (fAlpha <= 0.f)
    {
        discard;
    }

    float2 vSpriteSize = float2(1.f, 1.f) / g_vNumSprite;
    int2 vSpriteCoord;
    vSpriteCoord.x = Input.iIndex % g_vNumSprite.x;
    vSpriteCoord.y = Input.iIndex / g_vNumSprite.x;
    float2 vUV = Input.vTex / g_vNumSprite + (vSpriteSize * vSpriteCoord);
    
    vector vMask = g_MaskTexture.Sample(LinearSampler, vUV);
    if (vMask.r < 0.1f)
    {
        discard;
    }

    float3 Color = g_vColor.rgb;
    fAlpha = g_vColor.a * vMask.r * fAlpha;
    
    float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 4.f)), 1e-2, 3e3);
    fWeight = max(min(1.f, max(max(Color.r, Color.g), Color.b) * fAlpha), fAlpha) * fWeight;
    
    Output.vColor = vector(Color * fAlpha, fAlpha) * fWeight;
    Output.vAlpha = fAlpha;
    Output.vBlur = vector(Color, fAlpha) * g_isBlur;

    return Output;
}

PS_OUT PS_Main_Color_Dissolve(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float fDissolve = g_DissolveTexture.Sample(LinearSampler, Input.vTex).r;
    float fAlpha = saturate((fDissolve - (g_fDissolveRatio + 1.f - Input.fDissolveRatio)) * 10.0f);
    if (fAlpha <= 0.f)
    {
        discard;
    }
    
    vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex);
    if (vMask.r < 0.1f)
    {
        discard;
    }
    
    float3 Color = g_vColor.rgb;
    fAlpha = g_vColor.a * vMask.r * fAlpha;
        
    float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 4.f)), 1e-2, 3e3);
    fWeight = max(min(1.f, max(max(Color.r, Color.g), Color.b) * fAlpha), fAlpha) * fWeight;
    
    Output.vColor = vector(Color * fAlpha, fAlpha) * fWeight;
    Output.vAlpha = fAlpha;
    Output.vBlur = vector(Color, fAlpha) * g_isBlur;
        
    return Output;
}

PS_OUT PS_Main_Sprite_Texture(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float2 vSpriteSize = float2(1.f, 1.f) / g_vNumSprite;
    int2 vSpriteCoord;
    vSpriteCoord.x = Input.iIndex % g_vNumSprite.x;
    vSpriteCoord.y = Input.iIndex / g_vNumSprite.x;
    float2 vUV = Input.vTex / g_vNumSprite + (vSpriteSize * vSpriteCoord);
    
    vector vColor = g_Texture.Sample(LinearSampler, vUV);
    
    float3 Color = vColor.rgb;
    float fAlpha = vColor.a * Input.fDissolveRatio;
    
    //float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 6.f)), 0.01f, 3e3); // 1e-5 = 0.00001, 3e3 = 3000
    
    //fWeight = max(fWeight, 1.f);
    
    //float fWeight = pow(Input.LinearZ, -2.5f);
    
    float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 4.f)), 1e-2, 3e3);
    fWeight = max(min(1.f, max(max(Color.r, Color.g), Color.b) * fAlpha), fAlpha) * fWeight;
    
    Output.vColor = vector(Color * fAlpha, fAlpha) * fWeight;
    Output.vAlpha = fAlpha;
    Output.vBlur = vector(Color, fAlpha) * g_isBlur;
    
    
    //Output.vColor.a *= Input.fDissolveRatio;
    
    return Output;
}

PS_OUT PS_Main_Sprite_Texture_Dissolve(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float fDissolve = g_DissolveTexture.Sample(LinearSampler, Input.vTex).r;
    float fAlpha = saturate((fDissolve - (g_fDissolveRatio + 1.f - Input.fDissolveRatio)) * 10.0f);
    if (fAlpha <= 0.f)
    {
        discard;
    }

    float2 vSpriteSize = float2(1.f, 1.f) / g_vNumSprite;
    int2 vSpriteCoord;
    vSpriteCoord.x = Input.iIndex % g_vNumSprite.x;
    vSpriteCoord.y = Input.iIndex / g_vNumSprite.x;
    float2 vUV = Input.vTex / g_vNumSprite + (vSpriteSize * vSpriteCoord);
    
    vector vColor = g_Texture.Sample(LinearSampler, vUV);
    
    float3 Color = vColor.rgb;
    fAlpha = vColor.a * fAlpha;
    
    //float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 6.f)), 0.01f, 3e3); // 1e-5 = 0.00001, 3e3 = 3000
    
    //fWeight = max(fWeight, 1.f);
    
    //float fWeight = pow(Input.LinearZ, -2.5f);
    
    float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 4.f)), 1e-2, 3e3);
    fWeight = max(min(1.f, max(max(Color.r, Color.g), Color.b) * fAlpha), fAlpha) * fWeight;
    
    Output.vColor = vector(Color * fAlpha, fAlpha) * fWeight;
    Output.vAlpha = fAlpha;
    Output.vBlur = vector(Color, fAlpha) * g_isBlur;
    
    
    return Output;
}

PS_OUT PS_Main_Sprite_Diff_Mask(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float2 vSpriteSize = float2(1.f, 1.f) / g_vNumSprite;
    int2 vSpriteCoord;
    vSpriteCoord.x = Input.iIndex % g_vNumSprite.x;
    vSpriteCoord.y = Input.iIndex / g_vNumSprite.x;
    float2 vUV = Input.vTex / g_vNumSprite + (vSpriteSize * vSpriteCoord);
    
    vector vMask = g_MaskTexture.Sample(LinearSampler, vUV);
    if (vMask.r < 0.1f)
    {
        discard;
    }

    vector vColor = g_Texture.Sample(LinearSampler, vUV);
    
    float3 Color = vColor.rgb;
    float fAlpha = vMask.r * vMask.a * Input.fDissolveRatio;
    
    //float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 6.f)), 0.01f, 3e3); // 1e-5 = 0.00001, 3e3 = 3000
    
    //fWeight = max(fWeight, 1.f);
    
    //float fWeight = pow(Input.LinearZ, -2.5f);
    
    float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 4.f)), 1e-2, 3e3);
    fWeight = max(min(1.f, max(max(Color.r, Color.g), Color.b) * fAlpha), fAlpha) * fWeight;
    
    Output.vColor = vector(Color * fAlpha, fAlpha) * fWeight;
    Output.vAlpha = fAlpha;
    Output.vBlur = vector(Color, fAlpha) * g_isBlur;
       
    
    
    //Output.vColor.a = vMask.r * vMask.a;
    //Output.vColor.a *= Input.fDissolveRatio;

    return Output;
}

PS_OUT PS_Main_Sprite_Diff_Mask_Dissolve(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    float fDissolve = g_DissolveTexture.Sample(LinearSampler, Input.vTex).r;
    float fAlpha = saturate((fDissolve - (g_fDissolveRatio + 1.f - Input.fDissolveRatio)) * 10.0f);
    if (fAlpha <= 0.f)
    {
        discard;
    }
    
    float2 vSpriteSize = float2(1.f, 1.f) / g_vNumSprite;
    int2 vSpriteCoord;
    vSpriteCoord.x = Input.iIndex % g_vNumSprite.x;
    vSpriteCoord.y = Input.iIndex / g_vNumSprite.x;
    float2 vUV = Input.vTex / g_vNumSprite + (vSpriteSize * vSpriteCoord);
    
    vector vMask = g_MaskTexture.Sample(LinearSampler, vUV);
    if (vMask.r < 0.1f)
    {
        discard;
    }

    vector vColor = g_Texture.Sample(LinearSampler, vUV);
    
    
    float3 Color = vColor.rgb;
    fAlpha = vMask.r * vMask.a * fAlpha;
    
    //float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 6.f)), 0.01f, 3e3); // 1e-5 = 0.00001, 3e3 = 3000
    
    //fWeight = max(fWeight, 1.f);
    
    float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 4.f)), 1e-2, 3e3);
    fWeight = max(min(1.f, max(max(Color.r, Color.g), Color.b) * fAlpha), fAlpha) * fWeight;
    
    Output.vColor = vector(Color * fAlpha, fAlpha) * fWeight;
    Output.vAlpha = fAlpha;
    Output.vBlur = vector(Color, fAlpha) * g_isBlur;
    
    
   // Output.vColor.a = vMask.r * vMask.a;


    return Output;
}

PS_OUT PS_Main_Color_Alpha(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vColor = g_vColor;
    vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex);
    if (vMask.r < 0.1f)
    {
        discard;
    }
    
    float3 Color = vColor.rgb;
    float fAlpha = g_vColor.a * vMask.r * Input.fDissolveRatio * saturate(g_fAlpha);
    
    float fWeight = clamp(0.03f / (1e-5 + pow(Input.LinearZ, 4.f)), 1e-2, 3e3);
    fWeight = max(min(1.f, max(max(Color.r, Color.g), Color.b) * fAlpha), fAlpha) * fWeight;
    
    Output.vColor = vector(Color * fAlpha, fAlpha) * fWeight;
    Output.vAlpha = fAlpha;
    Output.vBlur = vector(Color, fAlpha) * g_isBlur;
    
    return Output;
}

PS_OUT_DISTORTION PS_Distortion(PS_IN Input)
{
    PS_OUT_DISTORTION Output = (PS_OUT_DISTORTION) 0;
    vector vMask = g_MaskTexture.Sample(LinearSampler, Input.vTex);
    if (vMask.r < 0.1f)
    {
        discard;
    }

    Output.vDistortion = normalize(g_DistortionTexture.Sample(LinearSampler, Input.vTex)) * g_fAlpha * vMask.r * Input.fDissolveRatio;
    
    return Output;
}

technique11 DefaultTechnique
{
    pass Particle_Texture_Mask
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main();
    }

    pass Particle_Sprite_Color
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite();
    }

    pass Particle_Color
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Color();
    }

    pass Particle_Texture_Mask_Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Dissolve();
    }

    pass Particle_Sprite_Color_Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_Dissolve();
    }

    pass Particle_Color_Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Color_Dissolve();
    }

    pass Particle_Sprite_Texture
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_Texture();
    }

    pass Particle_Sprite_Texture_Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_Texture_Dissolve();
    }

    pass Particle_Sprite_Texture_RandomIndex
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main_RandomIndex();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_Texture();
    }

    pass Particle_Sprite_Texture_RandomIndex_Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main_RandomIndex();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_Texture_Dissolve();
    }

    pass Particle_Sprite_Color_RandomIndex
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main_RandomIndex();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite();
    }

    pass Particle_Sprite_Color_RandomIndex_Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main_RandomIndex();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_Dissolve();
    }

    pass Particle_Texture_Mask_Trail
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN_Trail();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main();
    }

    pass Particle_Sprite_Color_Trail
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN_Trail();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite();
    }

    pass Particle_Color_Trail
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN_Trail();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Color();
    }

    pass Particle_Texture_Mask_Dissolve_Trail
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN_Trail();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Dissolve();
    }

    pass Particle_Sprite_Color_Dissolve_Trail
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN_Trail();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_Dissolve();
    }

    pass Particle_Color_Dissolve_Trail
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN_Trail();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Color_Dissolve();
    }

    pass Particle_Sprite_Texture_Trail
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN_Trail();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_Texture();
    }

    pass Particle_Sprite_Texture_Dissolve_Trail
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN_Trail();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_Texture_Dissolve();
    }

    pass Particle_Sprite_Texture_RandomIndex_Trail
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main_RandomIndex();
        GeometryShader = compile gs_5_0 GS_MAIN_Trail();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_Texture();
    }

    pass Particle_Sprite_Texture_RandomIndex_Dissolve_Trail
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main_RandomIndex();
        GeometryShader = compile gs_5_0 GS_MAIN_Trail();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_Texture_Dissolve();
    }

    pass Particle_Sprite_Color_RandomIndex_Trail
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main_RandomIndex();
        GeometryShader = compile gs_5_0 GS_MAIN_Trail();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite();
    }

    pass Particle_Sprite_Color_RandomIndex_Dissolve_Trail
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main_RandomIndex();
        GeometryShader = compile gs_5_0 GS_MAIN_Trail();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_Dissolve();
    }

    pass Particle_Sprite_Diff_Mask
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_Diff_Mask();
    }

    pass Particle_Sprite_Diff_Mask_Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_Diff_Mask_Dissolve();
    }

    pass Particle_Sprite_Diff_Mask_RandomIndex
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main_RandomIndex();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_Diff_Mask();
    }

    pass Particle_Sprite_Diff_Mask_RandomIndex_Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main_RandomIndex();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_Diff_Mask_Dissolve();
    }

    pass Particle_Sprite_Diff_Mask_Trail
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN_Trail();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_Diff_Mask();
    }

    pass Particle_Sprite_Diff_Mask_Dissolve_Trail
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN_Trail();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_Diff_Mask_Dissolve();
    }

    pass Particle_Sprite_Diff_Mask_RandomIndex_Trail
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main_RandomIndex();
        GeometryShader = compile gs_5_0 GS_MAIN_Trail();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_Diff_Mask();
    }

    pass Particle_Sprite_Diff_Mask_RandomIndex_Dissolve_Trail
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main_RandomIndex();
        GeometryShader = compile gs_5_0 GS_MAIN_Trail();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Sprite_Diff_Mask_Dissolve();
    }

    pass Particle_Color_Alpha
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Effect, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Color_Alpha();
    }

    pass Particle_Distortion
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_OnebyOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Distortion();
    }
};