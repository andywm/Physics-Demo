struct idata
{
	float3 position : POSITION;
	float4 colour : COLOR;
};
float4 main(in idata data) : SV_TARGET
{
	return float4(data.colour[0], data.colour[1], data.colour[2] , 1.0f);
}