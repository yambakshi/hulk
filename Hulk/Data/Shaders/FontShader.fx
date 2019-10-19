/////////////
// GLOBALS //
/////////////
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
Texture2D fontTexture;
float4 fontColor;

///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleType
{
	Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

//////////////////
// BLEND STATES //
//////////////////
BlendState AlphaBlendingOn
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
	float2 texCoord : TEXCOORD;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType FontVertexShader(VertexInputType input)
{
    PixelInputType output;    
    
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Store the input color for the pixel shader to use.
    output.texCoord = input.texCoord;
    
	return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 FontPixelShader(PixelInputType input) : SV_Target
{	
	float4 color;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
    color = fontTexture.Sample(SampleType, input.texCoord);

	if(color.a > 0.0f)
	{
		color.r = fontColor.r;
		color.g = fontColor.g;
		color.b = fontColor.b;
		color.a *= fontColor.a;
	}
		
	return color;
}

////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 FontTechnique
{
    pass pass0
    {
		SetBlendState(AlphaBlendingOn, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader(CompileShader(vs_4_0, FontVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, FontPixelShader()));
        SetGeometryShader(NULL);
    }
}