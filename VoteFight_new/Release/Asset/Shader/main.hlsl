// --------------define----------------
#define MAX_LIGHTS			3 
#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3
#define MAX_MATERIALS		16 

#define _WITH_LOCAL_VIEWER_HIGHLIGHTING
#define _WITH_THETA_PHI_CONES

// ---------------- structs---------------------------
struct LIGHT
{
    float4 m_xmf4Ambient;
    float4 m_xmf4Diffuse;
    float4 m_xmf4Specular;
    float4 m_xmf3Position;
    
    bool m_isActive;
			   
    float3 m_position;
    float3 m_direction;
			   
    int m_type;
			   
    float3 m_attenuation;
    float m_fallOff;
    float m_range;
    float m_theta;
    float m_phi;
			   
    bool m_shadowMapping;
    matrix m_toTexCoord;
};
struct Fog
{
    float4 m_color;
    float m_density;
};

//---------------c buffer --------------------------
cbuffer CB_GameFramework : register(b0)
{
    float gfTotalTime : packoffset(c0);
    float gfElapsedTime : packoffset(c0.y);
};

cbuffer CB_Camera : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 gvCameraPosition : packoffset(c8);
};

cbuffer CB_GameScene : register(b2)
{
    LIGHT m_lights[MAX_LIGHTS];
    float4 gcGlobalAmbientLight;
    Fog m_fog;
    int gnLights;
};


cbuffer CB_Object : register(b3)
{
    matrix gmtxGameObject : packoffset(c0);
    float4 gvColor : packoffset(c4);
    int gnTexturesMask : packoffset(c5);
    float2 gvTextureScale : packoffset(c5.y);
};

cbuffer CB_Sprite : register(b4)
{
    float width : packoffset(c0);
    float height : packoffset(c0.y);
    float left : packoffset(c0.z);
    float top : packoffset(c0.w);
};
//----------------------------------- Light Functions ------------------------------

float4 DirectionalLight(int nIndex, float3 vNormal, float3 vToCamera)
{
 
    float3 vToLight = m_lights[nIndex].m_position;
    float fDiffuseFactor = dot(vNormal, vToLight) * 0.7 + 0.3;
    fDiffuseFactor = fDiffuseFactor * 5;
    fDiffuseFactor = ceil(fDiffuseFactor) / 5;
   
    float fSpecularFactor = 0.0f;
    if (fDiffuseFactor > 0.0f)
    {
        float3 vHalf = normalize(vToCamera + vToLight);
        fSpecularFactor = max(dot(vHalf, vNormal), 0.0f);
    }
 
    
    float4 cColor = ((m_lights[nIndex].m_xmf4Ambient) +
                (m_lights[nIndex].m_xmf4Diffuse * fDiffuseFactor) +
                (m_lights[nIndex].m_xmf4Specular * fSpecularFactor));
    return cColor;

}


float4 SpotLight(int nIndex, float3 vPosition, float3 vNormal, float3 vToCamera)
{
    /*
    float3 vToLight = m_lights[nIndex].m_position - vPosition;
    float fDistance = length(vToLight);
    if (fDistance <= m_lights[nIndex].m_range)
    {
        float fSpecularFactor = 0.0f;
        vToLight /= fDistance;
        float fDiffuseFactor = dot(vToLight, vNormal);
        if (fDiffuseFactor > 0.0f)
        {
            float3 vHalf = normalize(vToCamera + vToLight);
            fSpecularFactor = max(dot(vHalf, vNormal), 0.0f);
        }
        float fAlpha = max(dot(-vToLight, m_lights[nIndex].m_direction), 0.0f);
        float fSpotFactor = pow(max(((fAlpha - m_lights[nIndex].m_phi) / (m_lights[nIndex].m_theta - gLights[nIndex].m_fPhi)), 0.0f), gLights[nIndex].m_fFalloff);

        float fAttenuationFactor = 1.0f / dot(gLights[nIndex].m_vAttenuation, float3(1.0f, fDistance, fDistance * fDistance));

        return (((gLights[nIndex].m_cAmbient * gMaterial.m_cAmbient) + (gLights[nIndex].m_cDiffuse * fDiffuseFactor * gMaterial.m_cDiffuse) + (gLights[nIndex].m_cSpecular * fSpecularFactor * gMaterial.m_cSpecular)) * fAttenuationFactor * fSpotFactor);
    }
    return (float4(0.0f, 0.0f, 0.0f, 0.0f));
    */
}


float4 Lighting(float3 vPosition, float3 vNormal)
{
    float3 vCameraPosition = float3(gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z);
    float3 vToCamera = normalize(vCameraPosition - vPosition);
    
    float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    cColor += DirectionalLight(0, vNormal, vToCamera);
    // cColor += (gcGlobalAmbientLight);
    
    
    return (cColor);
}

float4 franelOuterLine(float3 vPosition, float3 vNormal, float4 cColor)
{
    float3 vCameraPosition = float3(gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z);
    float3 vToCamera = normalize(vCameraPosition - vPosition);
    float rim = abs(dot(vNormal, vToCamera));
    float4 newColor = cColor;
    if (rim > 0.3)
        rim = 1.0;
    else 
        rim = -1;
    newColor.rgb = newColor.rgb * rim;
    return newColor;
}

//----------------------------------------------------------
#define MAX_VERTEX_INFLUENCES			4
#define SKINNED_ANIMATION_BONES			256

cbuffer cbBoneOffsetInfo : register(b5)
{
    matrix gpmtxBoneOffsets[SKINNED_ANIMATION_BONES];
};

cbuffer cbBoneTransformInfo : register(b6)
{
    matrix gpmtxBoneTransforms[SKINNED_ANIMATION_BONES];
};

//////////////////////////////////////////////////////////////////////
//
//#define _WITH_VERTEX_LIGHTING

#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_NORMAL_MAP			0x02

Texture2D gtxtAlbedoTexture : register(t0);
Texture2D gtxtNormalTexture : register(t1);
TextureCube gtxtCubeTexture : register(t2);

SamplerState samplerState : register(s0);
SamplerState pcfSamplerState : register(s1);
SamplerState gssClamp : register(s2);

struct VS_STANDARD_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
};

struct VS_STANDARD_OUTPUT
{
	float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float2 uv : TEXCOORD;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
	float3 bitangentW : BITANGENT;
};

VS_STANDARD_OUTPUT VS_Main(VS_STANDARD_INPUT input)
{
	VS_STANDARD_OUTPUT output;

	output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;
	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.tangentW = mul(input.tangent, (float3x3)gmtxGameObject);
	output.bitangentW = mul(input.bitangent, (float3x3)gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

float4 PS_Main(VS_STANDARD_OUTPUT input) : SV_TARGET
{
	float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_ALBEDO_MAP) cAlbedoColor = gtxtAlbedoTexture.Sample(samplerState, input.uv);
	float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_NORMAL_MAP) cNormalColor = gtxtNormalTexture.Sample(samplerState, input.uv);

	float3 normalW;
	float4 cColor = cAlbedoColor;
	if (gnTexturesMask & MATERIAL_NORMAL_MAP)
	{
		float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));
		float3 vNormal = normalize(cNormalColor.rgb * 2.0f - 1.0f); //[0, 1] → [-1, 1]
		normalW = normalize(mul(vNormal, TBN));
	}
	else
	{
		normalW = normalize(input.normalW);
    }
    float4 cIllumination = Lighting(input.positionW, normalW);
  // cColor = (lerp(cColor, cIllumination, 0.5f));
    cColor = cColor * cIllumination;
   cColor = franelOuterLine(input.positionW,normalW, cColor);
    return cColor;

}

struct VS_SKINNED_STANDARD_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	int4 indices : BONEINDEX;
	float4 weights : BONEWEIGHT;
};

VS_STANDARD_OUTPUT VS_Main_Skinning(VS_SKINNED_STANDARD_INPUT input)
{
	VS_STANDARD_OUTPUT output;

	float4x4 mtxVertexToBoneWorld = (float4x4)0.0f;
	for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
	{		mtxVertexToBoneWorld += input.weights[i] * mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
	}
	output.positionW = mul(float4(input.position, 1.0f), mtxVertexToBoneWorld).xyz;
	output.normalW = mul(input.normal, (float3x3)mtxVertexToBoneWorld).xyz;
	output.tangentW = mul(input.tangent, (float3x3)mtxVertexToBoneWorld).xyz;
	output.bitangentW = mul(input.bitangent, (float3x3)mtxVertexToBoneWorld).xyz;

	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}
// ======== Bilboard =========
struct VS_BILBOARD_INPUT
{
    float3 position : POSITION;
};

struct VS_BILBOARD_OUTPUT
{
    float3 positionL : POSITION;
    float4 position : SV_POSITION;
};

VS_BILBOARD_OUTPUT VS_Bilboard(VS_BILBOARD_INPUT input)
{
    VS_BILBOARD_OUTPUT output;
    
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.positionL = input.position;

    return (output);
}

float4 PS_Bilboard(VS_BILBOARD_OUTPUT input) : SV_TARGET
{
    float4 cBaseTexColor = gtxtAlbedoTexture.Sample(samplerState, input.positionL.xy);
    float4 cColor = cBaseTexColor;
    
    return (cColor);
}


// ======== Skybox =========
struct VS_SKYBOX_CUBEMAP_INPUT
{
    float3 position : POSITION;
};

struct VS_SKYBOX_CUBEMAP_OUTPUT
{
    float3 positionL : POSITION;
    float4 position : SV_POSITION;
};

VS_SKYBOX_CUBEMAP_OUTPUT VS_SkyBox(VS_SKYBOX_CUBEMAP_INPUT input)
{
    VS_SKYBOX_CUBEMAP_OUTPUT output;

    float3 rotatePosition = float3(0, input.position.y, 0);
    float RotateSpeed = 0.04f;

    rotatePosition.x = input.position.x * cos(gfTotalTime * RotateSpeed) + input.position.z * sin(gfTotalTime * RotateSpeed);
    rotatePosition.z = input.position.x * -sin(gfTotalTime * RotateSpeed) + input.position.z * cos(gfTotalTime * RotateSpeed);
    
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.positionL = rotatePosition;
    return (output);
}

float4 PS_SkyBox(VS_SKYBOX_CUBEMAP_OUTPUT input) : SV_TARGET
{
    float4 cColor = gtxtCubeTexture.Sample(gssClamp, input.positionL);
    return (cColor);
}


// ======== UI =========
struct VS_UI_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_UI_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_UI_OUTPUT VS_UI(VS_UI_INPUT input)
{
    VS_UI_OUTPUT output;
   
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return output;
}

VS_UI_OUTPUT VS_UI_SPRITE(VS_UI_INPUT input)
{
    VS_UI_OUTPUT output;
   
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    //output.uv = mul(float3(input.uv, 1.0f), (float3x3) (gmtxTexture)).xy;
    //if (gmtxTexture._31 == 0.0f)
    //    output.uv += 0.5;
    
    output.uv.x = input.uv.x * width + left;
    output.uv.y = input.uv.y * height + top;
	
    return output;
}

float4 PS_UI(VS_UI_OUTPUT input) : SV_TARGET
{
    float4 textureColor;
	
    // 이 텍스처 좌표 위치에서 샘플러를 사용하여 텍스처에서 픽셀 색상을 샘플링합니다.
    textureColor = gtxtAlbedoTexture.Sample(samplerState, input.uv);

    return textureColor;
}

// ======== BOUNDING_BOX =========
struct VS_POSITION_INPUT
{
    float3 position : POSITION;
};

struct VS_POSITION_OUTPUT
{
    float4 positionH : SV_POSITION;
};

VS_POSITION_OUTPUT VS_Position(VS_POSITION_INPUT input)
{
    VS_POSITION_OUTPUT output;
	
    output.positionH = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    //output.positionH = float4(input.position, 1.0f);
    return (output);
}

float4 PS_Red(VS_POSITION_OUTPUT input) : SV_TARGET
{
    return (float4(1.0f, 0.0f, 0.0f, 1.0f));
}

// ======== TERRAIN =========
struct VS_TERRAIN_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VS_TERRAIN_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

VS_TERRAIN_OUTPUT VS_Terrain(VS_TERRAIN_INPUT input)
{
    VS_TERRAIN_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.normal = mul(input.normal, (float3x3) gmtxGameObject);
	
    output.uv = input.uv;

    return (output);
}

float4 PS_Terrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
    float4 cBaseTexColor = gtxtAlbedoTexture.Sample(samplerState, input.uv);
    float4 cColor = cBaseTexColor;
    float3 normal = normalize(input.normal);
    float4 cIllumination = Lighting(input.position.xyz, normal);
    // cColor = cColor * cIllumination;
   //  cColor = franelOuterLine(input.position, normal, cColor);
    // return (lerp(cColor, cIllumination, 0.2f));
    return cColor;
    // return cColor * cIllumination;

}


//======= Outer Eage===================
struct VS_EAGE_STANDARD_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct VS_EAGE_STANDARD_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
    float3 tangentW : TANGENT;
    float3 bitangentW : BITANGENT;
};

VS_EAGE_STANDARD_OUTPUT VS_EAGE_Main(VS_EAGE_STANDARD_INPUT input)
{
    VS_EAGE_STANDARD_OUTPUT output;

    output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;
    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.tangentW = mul(input.tangent, (float3x3) gmtxGameObject);
    output.bitangentW = mul(input.bitangent, (float3x3) gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);

    return (output);
}

float4 PS_EAGE_Main(VS_EAGE_STANDARD_OUTPUT input) : SV_TARGET
{
    float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
  
    return cColor;
}