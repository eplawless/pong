Texture2D g_texture;
SamplerState g_samplerState;

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
    return g_texture.Sample(g_samplerState, input.tex);
}
