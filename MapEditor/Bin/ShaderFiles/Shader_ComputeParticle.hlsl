#include "Engine_Shader_Define.hlsli"

struct Vertex_Instancing
{
    vector vRight;
    vector vUp;
    vector vLook;
    vector vPos;
    vector vPrevPos;
    float fIndex;
    float fDissolveRatio;

    float fSpeed;
    float2 vLifeTime;
    vector vOriginPos;
    vector vDirection;
    vector vOriginDir;
};

StructuredBuffer<Vertex_Instancing> InputInfo : register(t0);
RWStructuredBuffer<Vertex_Instancing> OutputInfo : register(u0);

cbuffer ParticleParams : register(b0)
{
    uint iNumInstances;
    uint iNumUse;
    float fTimeDelta;
    float fAppearRatio;
    
    vector vGravityDir;
    
    float fDissolveRatio;
    int isLoop;
    int bApplyGravity;
    int isFirstUpdate;
    
    row_major matrix WorldMatrix;
    
    vector Padding3;
}

float Random(float fSeed)
{
    float fRandom = frac(sin(fSeed) * 43758.5453f);
    return fRandom;
}

float RandomRange(float fSeed, float fMin, float fMax)
{
    float fRandom = Random(fSeed);
    float fRange = fMax - fMin;
    return (fRandom * fRange) + fMin;
}

[numthreads(256, 1, 1)]
void particle(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchID : SV_DispatchThreadID)
{
    if (dispatchID.x < iNumInstances)
    {
        Vertex_Instancing pVertex = InputInfo[dispatchID.x];
        
        if (pVertex.vLifeTime.x == 0)
        {
            pVertex.vPos = mul(pVertex.vOriginPos, WorldMatrix);
            pVertex.vPrevPos = pVertex.vPos;

            matrix WorldRotation = WorldMatrix;
            WorldRotation._11_12_13_14 = normalize(WorldRotation._11_12_13_14);
            WorldRotation._21_22_23_24 = normalize(WorldRotation._21_22_23_24);
            WorldRotation._31_32_33_34 = normalize(WorldRotation._31_32_33_34);
            WorldRotation._41_42_43_44 = vector(0.f, 0.f, 0.f, 1.f);
            pVertex.vDirection = mul(pVertex.vOriginDir, WorldRotation);
        }

        if (isLoop && isFirstUpdate)
        {
            pVertex.vPos = vector(0.f, -30000.f, 0.f, 1.f);
            pVertex.vPrevPos = pVertex.vPos;
        }

        if (bApplyGravity)
        {
            float fAlpha = (pVertex.vLifeTime.x / pVertex.vLifeTime.y) * 0.7f;
            pVertex.vDirection = lerp(pVertex.vDirection, vGravityDir, fAlpha);
        }

        if (fAppearRatio > 0.f && pVertex.vLifeTime.x <= pVertex.vLifeTime.y * fAppearRatio)
        {
            pVertex.fDissolveRatio = 1.f - (pVertex.vLifeTime.x / (pVertex.vLifeTime.y * fAppearRatio));
            pVertex.fDissolveRatio = min(pVertex.fDissolveRatio, 1.f);
        }
        else if (pVertex.vLifeTime.x > pVertex.vLifeTime.y * fDissolveRatio)
        {
            pVertex.fDissolveRatio = (pVertex.vLifeTime.x - (pVertex.vLifeTime.y * fDissolveRatio)) / (pVertex.vLifeTime.y * (1.f - fDissolveRatio));
            pVertex.fDissolveRatio = min(pVertex.fDissolveRatio, 1.f);
        }
        else
        {
            pVertex.fDissolveRatio = 0.f;
        }
        
        pVertex.fIndex = saturate(pVertex.vLifeTime.x / pVertex.vLifeTime.y);

        pVertex.vPrevPos = pVertex.vPos;
        pVertex.vPos += pVertex.vDirection * pVertex.fSpeed * fTimeDelta;
        pVertex.vLifeTime.x += fTimeDelta;

        if (dispatchID.x >= iNumUse)
        {
            pVertex.vPos = vector(-1000.f, -1000.f, -1000.f, 1.f);
        }
        else if (pVertex.vLifeTime.x > pVertex.vLifeTime.y)
        {
            if (isLoop == 1)
            {
                pVertex.vLifeTime.x = 0.f;
                pVertex.vPos = mul(vector(pVertex.vOriginPos.xyz, 1.f), WorldMatrix);
                pVertex.vPrevPos = pVertex.vPos;
            }
            else
            {
                pVertex.vPos = vector(0.f, -10000.f, 0.f, 1.f);
                pVertex.vPrevPos = pVertex.vPos;
            }
        }

        OutputInfo[dispatchID.x] = pVertex;
    }
}

cbuffer RandomParam : register(b1)
{
    float3 vMinPos;
    uint InitisLoop;
    
    float3 vMaxPos;
    float fRandomSeed;
    
    float2 vLifeTimeRange;
    float2 vScaleRange;
    
    float3 vMinDir;
    float fMinSpeed;
    
    float3 vMaxDir;
    float fMaxSpeed;
    
    //float fMaxRange;
    //float3 vPadding;
}

Texture2D<vector> InputNoiseNormalTexture : register(t1);


[numthreads(256, 1, 1)]
void Init(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchID : SV_DispatchThreadID)
{
    if (dispatchID.x < 512)
    {
        Vertex_Instancing pVertex = InputInfo[dispatchID.x];

       
        //vector fRandom = InputNoiseNormalTexture.Load(hi);
        
        float fScale = RandomRange(1, vScaleRange.x, vScaleRange.y);
        
        pVertex.vRight = vector(fScale, 0.f, 0.f, 0.f);
        pVertex.vUp = vector(0.f, fScale, 0.f, 0.f);
        pVertex.vLook = vector(0.f, 0.f, fScale, 0.f);
        
        float fPosX = RandomRange(1, vMinPos.x, vMaxPos.x);
        
        
        
        //fRandom = InputNoiseNormalTexture.Load(hi);
        
        float fPosY = RandomRange(1, vMinPos.y, vMaxPos.y);
        float fPosZ = RandomRange(1, vMinPos.z, vMaxPos.z);
        
        pVertex.vPos = vector(fPosX, fPosY, fPosZ, 1.f);
        
        pVertex.vOriginPos = pVertex.vPos;
        
        
        
        //fRandom = InputNoiseNormalTexture.Load(hi);
        
        float fSpeed = RandomRange(1, fMinSpeed, fMaxSpeed);
        float fLifeTime = RandomRange(1, vLifeTimeRange.x, vLifeTimeRange.y);
        
        pVertex.fSpeed = fSpeed;
        pVertex.vLifeTime.y = fLifeTime;
        
        if (1 == InitisLoop)
        {
            pVertex.vLifeTime.x = vLifeTimeRange.x;
        }
        else
        {
            pVertex.vLifeTime.x = 0.f;
        }
        
        
        //fRandom = InputNoiseNormalTexture.Load(hi);
        
        float fDirX = RandomRange(1, vMinDir.x, vMaxDir.x);
        float fDirY = RandomRange(1, vMinDir.y, vMaxDir.y);
        float fDirZ = RandomRange(1, vMinDir.z, vMaxDir.z);
        
        vector vDir = normalize(vector(fDirX, fDirY, fDirZ, 0.f));
        
        pVertex.vDirection = vDir;
        pVertex.vOriginDir = pVertex.vDirection;
        
        OutputInfo[dispatchID.x] = pVertex;
    }
}