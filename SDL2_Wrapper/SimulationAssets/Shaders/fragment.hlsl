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
//#define NORMALDEBUG

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
float4 blinnPhong(float4 normal, float4 position, float4 viewPos, Light light);

float4 main(in idata data) : SV_TARGET
{
	Light l1, l2, l3;
	l1.position = mul(proj, float4(0, 30, 40, 1));
	l1.power = 40.0f;

	l2.position = mul(proj, float4(40, 0, 0, 1));
	l2.power = 30.0f;

	l3.position = mul(proj, float4(0, 30, -40, 1));
	l3.power = 20.0f;
	//was taking view which is the inverse transpose of the transform to get the
	//camera to world space. Use view_it instead...
	//float4 c2 = float4(view_it[0][0], view_it[0][1], view_it[0][2], 1.0f);

	float4 frag = blinnPhong(normalize(data.normal), data.wspos, view_it[3], l1);
	frag+= blinnPhong(normalize(data.normal), data.wspos, view_it[3], l2);
	frag+= blinnPhong(normalize(data.normal), data.wspos, view_it[3], l3);

	float4 col = frag + ambientColour;
	col[3] = diffuseColour[3];
	return col;
}


/*
all calculations should be done in world space.
*/
float4 blinnPhong(float4 normal, 
				  float4 position, 
				  float4 viewPos, 
				  struct Light light)
{
#ifdef NORMALDEBUG
	return normal;
#endif

	float4 lightDir = light.position - position;
	//distance squared.
	float distance = length(lightDir)/10;
	//distance = distance * distance;

	lightDir = normalize(lightDir);
	distance = distance * distance + 0.01f;
	float4 ambient = ambientColour;

	float NdotL = dot(normal, lightDir);
	float intensity = saturate(NdotL);
	float4 diff = intensity * diffuseColour * light.power / (distance);

#ifdef RANGEDEBUG	
	return float4(1/distance*20, 1/distance*20, 1/distance*20, 1.0f);
#endif //RANGEDEBUG	

	float4 H = normalize(lightDir + viewPos);
	float NdotH = dot(normal, H);
	intensity = pow(saturate(NdotH), specularExponent);
	float4 spec = intensity * specularColour * 2; // (distance);

	return  diff + spec;
}
