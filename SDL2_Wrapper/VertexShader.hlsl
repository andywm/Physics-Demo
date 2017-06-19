struct idata
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 colour : COLOR;
	float2 tcord : TEXCORD;
};
struct odata 
{
	float4 position : SV_POSITION;
	float4 colour : COLOR;
};


void main(in idata dat, out odata odat)
{
	odat.colour = dat.colour;
	odat.position = float4(dat.position,1.0f);
	//return odat;
}