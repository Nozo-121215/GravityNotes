#include "Common.hlsl" //必須インクルード

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    // 法線の正規化 (補間によって縮んでいるため)
    float3 worldNormal = normalize(In.Normal.xyz);
    float3 lightDirection = normalize(Light.Direction.xyz);

    // ランバート反射
    float light = 0.0f;
    if (Light.Enable)
    {
        light = saturate(-dot(lightDirection, worldNormal));
    }

    // スペキュラー反射（ブリン・フォン）
    float3 L = -lightDirection;
    float3 V = normalize(CameraPosition.xyz - In.WorldPosition.xyz);
    float3 H = normalize(L + V);
    float specular = 0.0f;
    if (Light.Enable)
    {
        specular = pow(saturate(dot(worldNormal, H)), 50.0);
    }

    // テクスチャサンプリング
    float4 texColor = g_Texture.Sample(g_SamplerState, In.TexCoord);
	float3 baseColor = texColor.rgb * In.Diffuse.rgb;
	float3 ambient = saturate(Light.Ambient.rgb);
	float3 diffuse = baseColor * light * Light.Diffuse.rgb;

    // 最終出力 (ランバート + スペキュラー)
	outDiffuse.rgb = saturate(baseColor * ambient + diffuse + specular * Light.Diffuse.rgb);
    outDiffuse.a = texColor.a * In.Diffuse.a;
}
