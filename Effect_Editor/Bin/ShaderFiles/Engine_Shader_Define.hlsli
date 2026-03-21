float3 g_fLuminace = float3(0.2126f, 0.7152f, 0.0722f);
float g_fThreshold;

float3 g_OffSetVector[14] =
{
    { 1.f, 1.f, 1.f },
    { -1.f, -1.f, -1.f },
    { -1.f, 1.f, 1.f },
    { 1.f, -1.f, -1.f },
    { 1.f, 1.f, -1.f },
    { -1.f, -1.f, 1.f },
    { -1.f, 1.f, -1.f },
    { 1.f, -1.f, 1.f },
    { -1.f, 0.f, 0.f },
    { 1.f, 0.f, 0.f },
    { 0.f, -1.f, 0.f },
    { 0.f, 1.f, 0.f },
    { 0.f, 0.f, -1.f },
    { 0.f, 0.f, 1.f }
};

float g_Random[14] =
{
    0.2758f, 0.4928f, 0.3812f, 0.7589f, 0.9283f, 0.6729f, 0.3438f, 0.8947f, 0.5623f, 0.6829f, 0.7456f, 0.9382f, 0.8274f, 0.9629f
};

struct SSAO_DESC
{
    float fIntensity;
    float fRadius;
    float fScale;
    float fBias;
};

struct HDR_DESC
{
    float fMiddleGray;
    float fLumWhiteSqr;
    float2 vPadding;
};

// Sampler
sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;
};

sampler PointClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = clamp;
    AddressV = clamp;
};

sampler PointMirrorSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};


sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

sampler LinearClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;
    AddressV = clamp;
};

sampler LinearMirrorSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Mirror;
    AddressV = Mirror;
};

sampler LinearBorderSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Border;
    AddressV = Border;
    BorderColor = vector(1e5f, 1e5f, 1e5f, 1e5f);
};

// Rasterizer State
RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockWise = false;
};

RasterizerState RS_CCW
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockWise = true;
};

RasterizerState RS_None
{
    FillMode = Solid;
    CullMode = None;
};

RasterizerState RS_Wire
{
    FillMode = Wireframe;
    CullMode = Back;
    FrontCounterClockWise = false;
};

RasterizerState RS_Depth
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockWise = false;

    DepthBias = 100000;
    DepthBiasClamp = 0.f;
    SlopeScaledDepthBias = 1.f;
};

// Depth Stencil State
DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = Less_Equal;
    StencilEnable = true;
    FrontfaceStencilFail = KEEP;
    FrontfaceStencilDepthFail = KEEP;
    FrontfaceStencilPass = REPLACE;
};

DepthStencilState DSS_Effect
{
    DepthEnable = true;
    DepthWriteMask = Zero; // 깊이 버퍼에 쓰기를 비활성화합니다.
    DepthFunc = Less_Equal; // 깊이 테스트는 Less_Equal로 유지합니다.
    
    //StencilEnable = true;
    //FrontfaceStencilFail = KEEP;
    //FrontfaceStencilDepthFail = KEEP;
    //FrontfaceStencilPass = REPLACE;
};

DepthStencilState DSS_None
{
    DepthEnable = false;
    DepthWriteMask = Zero;
};

DepthStencilState DSS_DrawStencil
{
    DepthEnable = true;
    DepthWriteMask = all;
    StencilEnable = true;
    FrontfaceStencilFunc = ALWAYS;
    FrontfaceStencilFail = KEEP;
    FrontfaceStencilDepthFail = KEEP;
    FrontfaceStencilPass = REPLACE;
};

DepthStencilState DSS_StencilEqual
{
    DepthEnable = false;
    DepthWriteMask = Zero;
    StencilEnable = true;
    FrontfaceStencilFunc = EQUAL;
    FrontfaceStencilFail = KEEP;
    FrontfaceStencilDepthFail = KEEP;
    FrontfaceStencilPass = KEEP;
};

DepthStencilState DSS_StencilNotEqual
{
    DepthEnable = false;
    DepthWriteMask = Zero;
    StencilEnable = true;
    FrontFaceStencilFunc = NOT_EQUAL;
};

// Blend State
BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;

    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
};

BlendState BS_InvAlphaBlend
{
    BlendEnable[0] = true;
    
    SrcBlend = Inv_Src_Alpha;
    DestBlend = Src_Alpha;
    SrcBlendAlpha = Inv_Src_Alpha;
    DestBlendAlpha = Src_Alpha;
    BlendOp = Add;
};

BlendState BS_OnebyOne
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = One;
    DestBlend = One;
    BlendOp = Add;
};

BlendState BS_Half
{
    BlendEnable[0] = true;

    SrcBlend = Inv_Dest_Alpha;
    DestBlend = Dest_Alpha;
    BlendOp = Add;
};

BlendState BS_Outline
{
    BlendEnable[0] = true;

    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
    SrcBlendAlpha = Zero;
    DestBlendAlpha = One;
    BlendOpAlpha = Add;
};

BlendState BS_Effect
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    BlendEnable[2] = true;

    BlendOp = Add;

    SrcBlend[0] = One;
    DestBlend[0] = One;
    SrcBlendAlpha[0] = One;
    DestBlendAlpha[0] = One;
    
    SrcBlend[1] = Zero;
    DestBlend[1] = Inv_Src_Alpha;

    SrcBlend[2] = Src_Alpha;
    DestBlend[2] = Inv_Src_Alpha;
};

BlendState BS_BlurEffect
{
    BlendEnable[0] = true;

    BlendOp[0] = Add;
    SrcBlend[0] = One;
    DestBlend[0] = One;
    SrcBlendAlpha[0] = One;
    DestBlendAlpha[0] = One;
};