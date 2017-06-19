cbuffer ViewProjection : register(b0)
{
	float4x4 view;
	float4x4 proj;
	float4x4 view_it;
	float4x4 proj_it;
}
cbuffer Model : register(b1)
{
	float4x4 model;
	float4x4 model_it;
}

cbuffer Material : register(b2)
{
	float4 diffuseColour;
	float4 ambientColour;
	float4 specularColour;
	float specularExponent;
}

//#define RANGEDEBUG
#define NORMALDEBUG

SamplerState LinearSampler : register(s0);

struct idata
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL;
	float4 wspos : COLOR;
	float2 texcoords : TEXCOORD;
};

struct Light
{
	float4 position;
	float power;

};
//float4 blinnPhong(float4 normal, float4 position, float4 viewPos, Light light);

float4 main(in idata data) : SV_TARGET
{
	//was taking view which is the inverse transpose of the transform to get the
	//camera to world space. Use view_it instead...
	//float4 c2 = float4(view_it[0][0], view_it[0][1], view_it[0][2], 1.0f);
	return data.normal;
	//return  blinnPhong(normalize(data.normal), data.wspos, view_it[3], l);	
}


