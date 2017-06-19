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

struct idata
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texcoords : TEXCOORD;
};
struct odata 
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL;
	float4 wspos : COLOR;
	float2 texcoords : TEXCOORD;
};


void main(in idata dat, out odata odat)
{
	odat.texcoords = dat.texcoords;
	odat.position = float4(dat.position, 1.0f);
	odat.normal = float4(dat.normal, 1.0f);
	odat.wspos = mul(model,odat.position);
	//point' = point * model * view * projection
	//Pos * M * V * P or P * V * M * Pos. But yours is M * V * P * Pos. –

	float4x4 mvp = mul(mul(proj, view), model);
	//float4x4 mvp_it = mul(mul(proj_it, view_it), model_it);
	float4x4 mvp_it = model_it;

	odat.position = mul(mul(mul(proj, view), model), odat.position);
	//odat.position = mul(mvp, odat.position);
	odat.normal = mul(mvp_it, odat.normal);
}
/*
cbuffer {
	float4x4 model;
	float4x4 view;
	float4x4 projection;
};

{
	float4x4 MVP = P * V * M;

	out.pos = mul(MVP, in.pos);

	float4x4 MVPIT = PIT * VIT * MIT;

	out.norm = mul(MVPIT, in.normal)
}*/
///Zpc

//right handed multiplication
//float4x4 modelView = mul(model, view);
//float4x4 modelViewProjection = mul(modelView, proj);
//odat.position = mul(modelViewProjection, odat.position);

//left handed multiplication
//odat.position = mul(model, odat.position);
//odat.position = mul(view, odat.position);
//odat.position = mul(proj, odat.position);

//odat.position = mul(model, odat.position);
//odat.position = mul(view, odat.position);
//odat.position = mul(proj, odat.position);





//float4x4 mvp = mul(model, mul(view, proj));
//float4x4 mvp = mul(mul(model, view), proj);

//float4x4 mvp = mul(mul(proj, view), model));
//float4x4 mvp = mul(proj, mul(view, model));


//float4x4 mvp = mul(proj, mul(view, model)); //this is backwards.

//odat.position = mul(odat.position, mvp);

//odat.position = mul(mul(mul(odat.position, model), view), proj);
//odat.position = mul(odat.position, mul(mul(model,view), proj));

//odat.position = mul(mvp, odat.position);