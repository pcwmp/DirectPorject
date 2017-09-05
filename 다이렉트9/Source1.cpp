#include "Share.h"

struct VS_OUTPUT_BASE
{
	float4 Pos						: POSITION;
	float3 TexCoord0				: TEXCOORD0;
	float2 TexCoord1				: TEXCOORD1;

	float  diffuse					: TEXCOORD2;
	float3 reflection				: TEXCOORD3;
	float3 viewDir					: TEXCOORD4;

	float4 color					: COLOR;
	float4 dynamic_ShadowCoord		: TEXCOORD5;
	float4 static_ShadowCoord		: TEXCOORD6;
	float4 dynamic_ShadowCoordLow	: TEXCOORD7;
	float  fog						: FOG;
};

struct VS_OUTPUT_BASE_NOSHADOW
{
	float4 Pos					: POSITION;
	float3 TexCoord0			: TEXCOORD0;
	float2 TexCoord1			: TEXCOORD1;

	float3 diffuse				: TEXCOORD2;
	float3 reflection			: TEXCOORD3;
	float3 viewDir				: TEXCOORD4;

	float3 viwDir				: TEXCOORD5;
	float4 color				: COLOR;
	float  fog					: FOG;
};

struct VS_INPUT_BASE
{
	float4 Pos					: POSITION;
	float4 BlendWeights			: BLENDWEIGHT;
	float3 Normal				: NORMAL;
	float4 vertexColor			: COLOR0;
	float2 vTex0				: TEXCOORD0;
	float2 vTex1				: TEXCOORD1;
	float3 Binormal				: TEXCOORD2;
};

struct VS_INPUT_BASE_NOSHADOW
{
	float4 Pos					: POSITION;
	float4 BlendWeights		    : BLENDWEIGHT;
	float3 Normal				: NORMAL;
	float4 vertexColor			: COLOR0;
	float3 vTex0				: TEXCOORD0;
	float2 vTex1				: TEXCOORD1;
};

VS_OUTPUT_BASE VS_BASE_BUMP(VS_INPUT_BASE input)
{
	VS_OUTPUT_BASE output = (VS_OUTPUT_BASE)0;

	float3 Pos = 0.0f;
	float3 Normal = 0.0f;
	float3 Binormal = 0.0f;
	float3 Tangent = 0.0f;

	if (uniform_vertex_blend != 0)
	{
		float LastWeight = 0.0f;
		float BlendWeightsArray[4] = (float[4])input.BlendWeights;

		for (int iBone = 0; iBone < blendBoneNum - 1; iBone++)
		{
			LastWeight = LastWeight + BlendWeightsArray[iBone];

			Pos += mul(input.Pos, boneMatrixArray[iBone]) * BlendWeightsArray[iBone];
			Normal += mul(input.Normal, (float3x3)boneMatrixArray[iBone]) * BlendWeightsArray[iBone];
			Binormal += mul(input.Binormal, (float3x3)boneMatrixArray[iBone]) * BlendWeightsArray[iBone];
			Tangent += mul(input.vTex1/*Tangent*/, (float3x3)boneMatrixArray[iBone]) * BlendWeightsArray[iBone];
		}
		LastWeight = 1.0f - LastWeight;

		Pos += (mul(input.Pos, boneMatrixArray[blendBoneNum - 1]) * LastWeight);
		Normal += (mul(input.Normal, (float3x3)boneMatrixArray[blendBoneNum - 1]) * LastWeight);
		Binormal += (mul(input.Binormal, (float3x3)boneMatrixArray[blendBoneNum - 1]) * LastWeight);
		Tangent += (mul(input.vTex1/*Tangent*/, (float3x3)boneMatrixArray[blendBoneNum - 1]) * LastWeight);
	}
	else
	{
		Pos = mul(float4(input.Pos.xyz, 1.0f), matWorld);
		Normal = mul(input.Normal, matWorld);
		Binormal = mul(input.Binormal, matWorld);
		Tangent = mul(input.vTex1/*Tangent*/, matWorld);
	}

	float3 worldNormal = normalize(Normal);
	float3 position_view = mul(float4(Pos, 1.f), matView);
	float3 reflect_view = reflect(normalize(position_view), worldNormal);
	//--------------------------------------------------------------------------
	float3 viewDir = cameraPosition - Pos;
	float3 LightDir = directlight.pos - Pos;
	//--------------------------------------------------------------------------
	//접선 공간으로 변환.
	output.Pos = mul(float4(position_view, 1.0f), matProj);
	output.viewDir.x = dot(Tangent, viewDir);
	output.viewDir.y = dot(Binormal, viewDir);
	output.viewDir.z = dot(Normal, viewDir);

	output.reflection/*lightDir*/.x = dot(Tangent, LightDir);
	output.reflection/*lightDir*/.y = dot(Binormal, LightDir);
	output.reflection/*lightDir*/.z = dot(Normal, LightDir);
	//--------------------------------------------------------------------------

	if (uniform_fog_enable)
	{
		output.fog = saturate((uniform_fog_end - position_view.z) / (uniform_fog_end - uniform_fog_start));
	}

	float4 tmp = 0.f;

	tmp = (uniform_texture_coord_index_0[0] * float4(input.vTex0, 1, 1)
		+ uniform_texture_coord_index_0[3] * float4(reflect_view, 1));

	output.TexCoord0.xy = mul(tmp, uniform_texture_coord_matrix0).xy;

	tmp = (
		uniform_texture_coord_index_1[0] * float4(input.vTex0, 1, 1)
		//+ uniform_texture_coord_index_1[1] * float4(input.vTex1, 1, 1)
		+ uniform_texture_coord_index_1[2] * float4(position_view, 1)
		+ uniform_texture_coord_index_1[3] * float4(reflect_view, 1)
		);

	output.TexCoord1 = mul(tmp, uniform_texture_coord_matrix1).xy;
	output.color = 1.f;

	if (stuffType == ST_MESH || stuffType == ST_DISTORT)
	{
		output.color.a = material.diffuse.a;
	}
	else if (stuffType == ST_BILLBOARD || stuffType == ST_BAND)
	{
		output.color.a = 1.f;
	}
	else if (stuffType == ST_PARTICLE)
	{
		output.color = input.vertexColor;
	}
	else
	{
		output.color.a = input.vertexColor.a;
	}


	float3 lightDir = normalize(Pos.xyz - directlight.pos.xyz);

	//범프할때 그림자 이 포지션으로 하면 이상하게 나올듯 일단 논외.
	float3 shadow_height = CalcShadowHeight(Pos.y, lightDir);
	float3 shadow_height_dynamic = CalcShadowHeight(Pos.y - userHeight, lightDir);

	output.static_ShadowCoord.z = CalcHeightMap(Pos.y) + HEIGHTMAP_BIAS;
	output.static_ShadowCoord.y = Pos.z + shadow_height.z;
	output.static_ShadowCoord.x = Pos.x + shadow_height.x;
	output.static_ShadowCoord.xy /= STATIC_SHADOW_SIZE;

	float shadow_start = uniform_shadow_size / 2.0;
	output.dynamic_ShadowCoord.z = CalcHeightMap(Pos.y) + HEIGHTMAP_BIAS;
	output.dynamic_ShadowCoord.y = Pos.z - uniform_shadow_pos.y + shadow_start + shadow_height_dynamic.z;
	output.dynamic_ShadowCoord.x = Pos.x - uniform_shadow_pos.x + shadow_start + shadow_height_dynamic.x;
	output.dynamic_ShadowCoord.xy /= uniform_shadow_size;

	shadow_start = uniform_shadow_size_LowQuality / 2.0;
	output.dynamic_ShadowCoordLow.z = CalcHeightMap(Pos.y) + HEIGHTMAP_BIAS;
	output.dynamic_ShadowCoordLow.y = Pos.z - uniform_shadow_pos_LowQuality.y + shadow_start + shadow_height_dynamic.z;
	output.dynamic_ShadowCoordLow.x = Pos.x - uniform_shadow_pos_LowQuality.x + shadow_start + shadow_height_dynamic.x;
	output.dynamic_ShadowCoordLow.xy /= uniform_shadow_size_LowQuality;

	return output;
}

vector PS_BASE_BUMP(VS_OUTPUT_BASE In) : COLOR
{
	if (stuffType == ST_DISTORT || stuffType == ST_GLASS)
	{
		In.TexCoord0 /= In.TexCoord0.z;
	}

	float4 gl_FragColor = 0.f;
	float4 diffuse = In.color;
	float4 color_texture = tex2D(S0, In.TexCoord0);

	float alpha1 =
		uniform_stage0_alpha_arg1[1] * color_texture.a +
		uniform_stage0_alpha_arg1[2] * uniform_texture_factor.a;
	float alpha2 =
		uniform_stage0_alpha_arg2[0] * diffuse.a/*varing_diffuse.a*/ +
		uniform_stage0_alpha_arg2[2] * uniform_texture_factor.a;

	gl_FragColor.a =
		uniform_stage0_alpha_op[0] * alpha1 +
		uniform_stage0_alpha_op[4] * alpha1 * alpha2;

	//if (gl_FragColor.a <= uniform_alpha_test)
	//{
	//	discard;
	//	return color_texture;
	//}

	float3 color1 = uniform_stage0_color_arg1[1] * color_texture.rgb
		+ uniform_stage0_color_arg1[2] * uniform_texture_factor.rgb;
	float3 color2 = uniform_stage0_color_arg2[0] * diffuse.rgb/*varing_diffuse.rgb*/
		+ uniform_stage0_color_arg2[2] * uniform_texture_factor.rgb
		+ uniform_stage0_color_arg2[3] * color_texture.a;

	gl_FragColor.rgb =
		uniform_stage0_color_op[0] * color1 +
		uniform_stage0_color_op[4] * color1 * color2;

	if (uniform_texture_stage >= 2)
	{
		color_texture = tex2D(S1, In.TexCoord1);

		float4 color1_all =
			uniform_stage1_color_arg1[0] * gl_FragColor +
			uniform_stage1_color_arg1[1] * color_texture;

		float3 color1 = color1_all.rgb;
		float3 color2 =
			uniform_stage1_color_arg2[0] * gl_FragColor.rgb +
			uniform_stage1_color_arg2[1] * color_texture.rgb +
			uniform_stage1_color_arg2[2] * uniform_texture_factor.rgb;

		float coef1 =
			uniform_stage1_color_op[0] * 1.0 +
			uniform_stage1_color_op[5] * color_texture.a +
			uniform_stage1_color_op[6] * gl_FragColor.a;
		float coef2 =
			uniform_stage1_color_op[1] * 1.0 +
			uniform_stage1_color_op[10] * (1.0 - color_texture.a) +
			uniform_stage1_color_op[6] * (1.0 - gl_FragColor.a) +
			uniform_stage1_color_op[8] * (1.0 - uniform_texture_factor.a) +
			uniform_stage1_color_op[7] * (color1_all.a) +
			uniform_stage1_color_op[9] * (1.0 - color1_all.a);

		gl_FragColor.rgb =
			uniform_stage1_color_op[3] *
			(
			coef1 * color1
			+ coef2 * color2
			+ uniform_stage1_color_op[2]
		)
			+ uniform_stage1_color_op[4] * color1 * color2
			+ uniform_stage1_color_op[12] * (color1 * color2 + color1_all.a)
			+ uniform_stage1_color_op[13] * ((1.0 - color1) * color2 + color1_all.a);

		alpha1 =
			uniform_stage1_alpha_arg1[0] * gl_FragColor.a +
			uniform_stage1_alpha_arg1[1] * color_texture.a;
		alpha2 =
			uniform_stage1_alpha_arg2[0] * gl_FragColor.a +
			uniform_stage1_alpha_arg2[1] * color_texture.a;

		gl_FragColor.a =
			uniform_stage1_alpha_op[3] *
			(
			uniform_stage1_alpha_op[0] * alpha1 +
			uniform_stage1_alpha_op[1] * alpha2 +
			uniform_stage1_alpha_op[2]
		) +
			uniform_stage1_alpha_op[4] * alpha1 * alpha2;
	}

	//조명
	float3 fvLightDir = normalize(In.reflection/*lightDir*/);
	float3 fvNormal = normalize( (tex2D(S5/*bumpTex*/, In.TexCoord0).xyz * 2.0f) - 1.0f );
	fvNormal *= 2;
	float fNDotL = dot(fvNormal, fvLightDir);

	float3 fvReflection = normalize(((2.0f * fvNormal) * (fNDotL)) - fvLightDir);

	float3 fvViewDir = normalize(In.viewDir);
	float fRDotV = max(0.0f, dot(fvReflection, fvViewDir));

	float4 fvTotalAmbient = material.ambient * (0.16 + directlight.ambient) * gl_FragColor;
	float4 fvTotalDiffuse = material.diffuse * directlight.diffuse * diffuse * fNDotL * gl_FragColor;
	float4 fvTotalSpecular = pow(saturate(dot(normalize(fvReflection), -normalize(fvViewDir))), 40.f);
	fvTotalSpecular = 0;

	//return gl_FragColor;
	//return (saturate(fvTotalAmbient + fvTotalDiffuse + fvTotalSpecular));

	float3 fvDiffuse = fNDotL; //output.diffuse = dot(normalize(directlight.pos.xyz), worldNormal);

	const float bias = 1.0 / STATIC_MAP_SIZE;
	float shadow = CalcShadow(float2(0, 0), In.dynamic_ShadowCoord, In.static_ShadowCoord, In.dynamic_ShadowCoordLow);
	shadow += CalcShadow(float2(bias, 0), In.dynamic_ShadowCoord, In.static_ShadowCoord, In.dynamic_ShadowCoordLow);
	shadow += CalcShadow(float2(-bias, 0), In.dynamic_ShadowCoord, In.static_ShadowCoord, In.dynamic_ShadowCoordLow);
	shadow += CalcShadow(float2(0, bias), In.dynamic_ShadowCoord, In.static_ShadowCoord, In.dynamic_ShadowCoordLow);
	shadow += CalcShadow(float2(0, -bias), In.dynamic_ShadowCoord, In.static_ShadowCoord, In.dynamic_ShadowCoordLow);

	shadow /= 5.0;

	float4 lightColor = CalculateDirectLights_bump(fvViewDir, fvReflection, fvDiffuse, In.TexCoord0, shadow);

	gl_FragColor.rgb *= lightColor.rgb;

	if (uniform_fog_enable)
	{
		gl_FragColor.rgb = lerp(uniform_fog_color.rgb, clamp(gl_FragColor.rgb, 0.0, 1.0), In.fog);
	}

	return gl_FragColor;
}

///////////////////////////////////////////////////////////////////


VS_OUTPUT_BASE VS_BASE(VS_INPUT_BASE input)
{
	if (isNormalMapFrame)
	{
		//하나의 함수에 다넣기 너무 지저분해서 분리함
		//input ouput내용이 이름과 다른경우가있는데 주석으로 실제 의미를 적어두었음
		return VS_BASE_BUMP(input);
	}

	VS_OUTPUT_BASE output = (VS_OUTPUT_BASE)0;
	float3 Pos = 0.0f;
	float3 Normal = 0.0f;

	if (uniform_vertex_blend != 0)
	{
		float LastWeight = 0.0f;
		float BlendWeightsArray[4] = (float[4])input.BlendWeights;

		for (int iBone = 0; iBone < blendBoneNum - 1; iBone++)
		{
			LastWeight = LastWeight + BlendWeightsArray[iBone];

			Pos += mul(input.Pos, boneMatrixArray[iBone]) * BlendWeightsArray[iBone];
			Normal += mul(input.Normal, (float3x3)boneMatrixArray[iBone]) * BlendWeightsArray[iBone];
		}
		LastWeight = 1.0f - LastWeight;

		Pos += (mul(input.Pos, boneMatrixArray[blendBoneNum - 1]) * LastWeight);
		Normal += (mul(input.Normal, (float3x3)boneMatrixArray[blendBoneNum - 1]) * LastWeight);
	}
	else
	{
		Pos = mul(float4(input.Pos.xyz, 1.0f), matWorld);
		Normal = mul(input.Normal, matWorld);
	}

	float3 worldNormal = normalize(Normal);
	float3 position_view = mul(float4(Pos, 1.f), matView);
	float3 reflect_view = reflect(normalize(position_view), worldNormal);
	//--------------------------------------------------------------------------
	float3 lightDir = normalize(Pos.xyz - directlight.pos.xyz);	
	output.viewDir = normalize(Pos.xyz - cameraPosition.xyz);
	//--------------------------------------------------------------------------
	output.Pos = mul(float4(position_view, 1.0f), matProj);
	output.reflection = reflect(lightDir, worldNormal);
	output.diffuse = dot(normalize(directlight.pos.xyz), worldNormal);
	//--------------------------------------------------------------------------

	if (uniform_fog_enable)
	{
		output.fog = saturate((uniform_fog_end - position_view.z) / (uniform_fog_end - uniform_fog_start));
	}

	float4 tmp = 0.f;

	tmp = (uniform_texture_coord_index_0[0] * float4(input.vTex0, 1, 1)
		+ uniform_texture_coord_index_0[3] * float4(reflect_view, 1));

	output.TexCoord0.xy = mul(tmp, uniform_texture_coord_matrix0).xy;

	tmp = (
		uniform_texture_coord_index_1[0] * float4(input.vTex0, 1, 1)
		+ uniform_texture_coord_index_1[1] * float4(input.vTex1, 1, 1)
		+ uniform_texture_coord_index_1[2] * float4(position_view, 1)
		+ uniform_texture_coord_index_1[3] * float4(reflect_view, 1)
		);

	output.TexCoord1 = mul(tmp, uniform_texture_coord_matrix1).xy;
	output.color = 1.f;

	if (stuffType == ST_MESH || stuffType == ST_DISTORT)
	{
		output.color.a = material.diffuse.a;
	}
	else if (stuffType == ST_BILLBOARD || stuffType == ST_BAND)
	{
		output.color.a = 1.f;
	}
	else if (stuffType == ST_PARTICLE)
	{
		output.color = input.vertexColor;
	}
	else
	{
		output.color.a = input.vertexColor.a;
	}

	float3 shadow_height = CalcShadowHeight(Pos.y, lightDir);
	float3 shadow_height_dynamic = CalcShadowHeight(Pos.y - userHeight, lightDir);

	output.static_ShadowCoord.z = CalcHeightMap(Pos.y) + HEIGHTMAP_BIAS;
	output.static_ShadowCoord.y = Pos.z + shadow_height.z;
	output.static_ShadowCoord.x = Pos.x + shadow_height.x;
	output.static_ShadowCoord.xy /= STATIC_SHADOW_SIZE;

	float shadow_start = uniform_shadow_size / 2.0;
	output.dynamic_ShadowCoord.z = CalcHeightMap(Pos.y) + HEIGHTMAP_BIAS;
	output.dynamic_ShadowCoord.y = Pos.z - uniform_shadow_pos.y + shadow_start + shadow_height_dynamic.z;
	output.dynamic_ShadowCoord.x = Pos.x - uniform_shadow_pos.x + shadow_start + shadow_height_dynamic.x;
	output.dynamic_ShadowCoord.xy /= uniform_shadow_size;

	shadow_start = uniform_shadow_size_LowQuality / 2.0;
	output.dynamic_ShadowCoordLow.z = CalcHeightMap(Pos.y) + HEIGHTMAP_BIAS;
	output.dynamic_ShadowCoordLow.y = Pos.z - uniform_shadow_pos_LowQuality.y + shadow_start + shadow_height_dynamic.z;
	output.dynamic_ShadowCoordLow.x = Pos.x - uniform_shadow_pos_LowQuality.x + shadow_start + shadow_height_dynamic.x;
	output.dynamic_ShadowCoordLow.xy /= uniform_shadow_size_LowQuality;

	return output;
}

vector PS_BASE(VS_OUTPUT_BASE In) : COLOR
{
	if (isNormalMapFrame)
	{
		//하나의 함수에 다넣기 너무 지저분해서 분리함
		//input ouput내용이 이름과 다른경우가있는데 주석으로 실제 의미를 적어두었음
		return PS_BASE_BUMP(In);
	}

	if (stuffType == ST_DISTORT || stuffType == ST_GLASS)
	{
		In.TexCoord0 /= In.TexCoord0.z;
	}

	float4 gl_FragColor = 0.f;
	float4 diffuse = In.color;
	float4 color_texture = tex2D(S0, In.TexCoord0);

	float alpha1 =
		uniform_stage0_alpha_arg1[1] * color_texture.a +
		uniform_stage0_alpha_arg1[2] * uniform_texture_factor.a;
	float alpha2 =
		uniform_stage0_alpha_arg2[0] * diffuse.a/*varing_diffuse.a*/ +
		uniform_stage0_alpha_arg2[2] * uniform_texture_factor.a;

	gl_FragColor.a = 
		uniform_stage0_alpha_op[0] * alpha1 +
		uniform_stage0_alpha_op[4] * alpha1 * alpha2;

	//if (gl_FragColor.a <= uniform_alpha_test)
	//{
	//	discard;
	//	return color_texture;
	//}

	float3 color1 = uniform_stage0_color_arg1[1] * color_texture.rgb
		+ uniform_stage0_color_arg1[2] * uniform_texture_factor.rgb;
	float3 color2 = uniform_stage0_color_arg2[0] * diffuse.rgb/*varing_diffuse.rgb*/
		+ uniform_stage0_color_arg2[2] * uniform_texture_factor.rgb
		+ uniform_stage0_color_arg2[3] * color_texture.a;

	gl_FragColor.rgb =
		uniform_stage0_color_op[0] * color1 +
		uniform_stage0_color_op[4] * color1 * color2;

	if (uniform_texture_stage >= 2)
	{
		color_texture = tex2D(S1, In.TexCoord1);

		float4 color1_all =
			uniform_stage1_color_arg1[0] * gl_FragColor +
			uniform_stage1_color_arg1[1] * color_texture;

		float3 color1 = color1_all.rgb;
		float3 color2 =
			uniform_stage1_color_arg2[0] * gl_FragColor.rgb +
			uniform_stage1_color_arg2[1] * color_texture.rgb +
			uniform_stage1_color_arg2[2] * uniform_texture_factor.rgb;

		float coef1 =
			uniform_stage1_color_op[0] * 1.0 +
			uniform_stage1_color_op[5] * color_texture.a +
			uniform_stage1_color_op[6] * gl_FragColor.a;
		float coef2 =
			uniform_stage1_color_op[1] * 1.0 +
			uniform_stage1_color_op[10] * (1.0 - color_texture.a) +
			uniform_stage1_color_op[6] * (1.0 - gl_FragColor.a) +
			uniform_stage1_color_op[8] * (1.0 - uniform_texture_factor.a) +
			uniform_stage1_color_op[7] * (color1_all.a) +
			uniform_stage1_color_op[9] * (1.0 - color1_all.a);

		gl_FragColor.rgb =
			uniform_stage1_color_op[3] *
			(
			coef1 * color1
			+ coef2 * color2
			+ uniform_stage1_color_op[2]
		)
			+ uniform_stage1_color_op[4] * color1 * color2
			+ uniform_stage1_color_op[12] * (color1 * color2 + color1_all.a)
			+ uniform_stage1_color_op[13] * ((1.0 - color1) * color2 + color1_all.a);

		alpha1 =
			uniform_stage1_alpha_arg1[0] * gl_FragColor.a +
			uniform_stage1_alpha_arg1[1] * color_texture.a;
		alpha2 =
			uniform_stage1_alpha_arg2[0] * gl_FragColor.a +
			uniform_stage1_alpha_arg2[1] * color_texture.a;

		gl_FragColor.a =
			uniform_stage1_alpha_op[3] *
			(
			uniform_stage1_alpha_op[0] * alpha1 +
			uniform_stage1_alpha_op[1] * alpha2 +
			uniform_stage1_alpha_op[2]
		) +
			uniform_stage1_alpha_op[4] * alpha1 * alpha2;
	}

	//조명
	const float bias = 1.0 / STATIC_MAP_SIZE;
	float shadow = CalcShadow(float2(0, 0), In.dynamic_ShadowCoord, In.static_ShadowCoord, In.dynamic_ShadowCoordLow);
	shadow += CalcShadow(float2(bias, 0), In.dynamic_ShadowCoord, In.static_ShadowCoord, In.dynamic_ShadowCoordLow);
	shadow += CalcShadow(float2(-bias, 0), In.dynamic_ShadowCoord, In.static_ShadowCoord, In.dynamic_ShadowCoordLow);
	shadow += CalcShadow(float2(0, bias), In.dynamic_ShadowCoord, In.static_ShadowCoord, In.dynamic_ShadowCoordLow);
	shadow += CalcShadow(float2(0, -bias), In.dynamic_ShadowCoord, In.static_ShadowCoord, In.dynamic_ShadowCoordLow);

	shadow /= 5.0;

	//그림자랑 픽셀 컬러값(디퓨즈컬러)를 넘겨서 조명 계산에 사용한다.
	float4 lightColor = CalculateDirectLights_(In.viewDir, In.reflection, In.diffuse, shadow, gl_FragColor, 1.f);

	//마우스 오버시 라이트를끄는데 그림자랑 괴리감이 든다.
	//그래서 라이트를 끌경우 라이트연산을 제거하지않고 엠비언트를 살짝줄여서 마우스 오버 느낌을준다!
	gl_FragColor.rgb = lightColor.rgb;

	if (uniform_fog_enable)
	{
		gl_FragColor.rgb = lerp(uniform_fog_color.rgb, clamp(gl_FragColor.rgb, 0.0, 1.0), In.fog);
	}

	return gl_FragColor;
}

///////////////////////////////////////////////////////////////////

VS_OUTPUT_BASE_NOSHADOW VS_BASE_NOSHADOW(VS_INPUT_BASE_NOSHADOW input)
{
	VS_OUTPUT_BASE_NOSHADOW output = (VS_OUTPUT_BASE_NOSHADOW)0;
	float3 Pos = 0.0f;
	float3 Normal = 0.0f;

	if (uniform_vertex_blend != 0)
	{
		float LastWeight = 0.0f;
		float BlendWeightsArray[4] = (float[4])input.BlendWeights;

		for (int iBone = 0; iBone < blendBoneNum - 1; iBone++)
		{
			LastWeight = LastWeight + BlendWeightsArray[iBone];

			Pos += mul(input.Pos, boneMatrixArray[iBone]) * BlendWeightsArray[iBone];
			Normal += mul(input.Normal, (float3x3)boneMatrixArray[iBone]) * BlendWeightsArray[iBone];
		}
		LastWeight = 1.0f - LastWeight;

		Pos += (mul(input.Pos, boneMatrixArray[blendBoneNum - 1]) * LastWeight);
		Normal += (mul(input.Normal, (float3x3)boneMatrixArray[blendBoneNum - 1]) * LastWeight);
	}
	else
	{
		Pos = mul(float4(input.Pos.xyz, 1.0f), matWorld);
		Normal = mul(input.Normal, matWorld);
	}

	float3 worldNormal = normalize(Normal);
	float3 position_view = mul(float4(Pos, 1.f), matView);
	float3 reflect_view = reflect(normalize(position_view), worldNormal);

	//--------------------------------------------------------------------------
	float3 lightDir = normalize(Pos.xyz - directlight.pos.xyz);
	output.viewDir = normalize(Pos.xyz - cameraPosition.xyz);
	//--------------------------------------------------------------------------
	output.Pos = mul(float4(position_view, 1.0f), matProj);
	output.reflection = reflect(lightDir, worldNormal);
	output.diffuse = dot(normalize(directlight.pos.xyz), worldNormal);
	//--------------------------------------------------------------------------

	if (uniform_fog_enable)
	{
		output.fog = saturate((uniform_fog_end - position_view.z) / (uniform_fog_end - uniform_fog_start));
	}
	output.TexCoord0.z = 1.f;

	if (stuffType == ST_DISTORT)
	{
		output.TexCoord0 = input.vTex0;
		output.TexCoord1 = input.vTex1;
	}
	else if (stuffType == ST_GLASS)
	{
		float2 toUV = float2(0.5f, -0.5f);
		float3 tex = output.Pos.xyw;
		tex.xy *= toUV;
		tex.xy = 0.5f * output.Pos.w + tex.xy;
		output.TexCoord0 = tex;
		output.TexCoord1 = input.vTex0;
	}
	else
	{
		output.TexCoord0 = input.vTex0;
		output.TexCoord1 = input.vTex0;
	}

	if (env0 == 2)//광학미채
	{
		float dx = env_dx0 - 0.1f;
		float3 position_OpticalMesh = position_view / position_view.z;
		float3 normal_OpticalMesh = (normalize(mul(Normal, matView)) * dx) + ((1 + dx) * position_OpticalMesh) + 0.5f;
		output.TexCoord0.xy = normal_OpticalMesh.xy;
		output.TexCoord0.y = 1 - output.TexCoord0.y;
	}

	float4 tmp = 0.f;

	tmp = (uniform_texture_coord_index_0[0] * float4(output.TexCoord0, 1)//광학미채때문에..
		+ uniform_texture_coord_index_0[3] * float4(reflect_view, 1));

	output.TexCoord0.xy = mul(tmp, uniform_texture_coord_matrix0).xy;

	tmp = (
		uniform_texture_coord_index_1[0] * float4(output.TexCoord1, 1, 1)
		+ uniform_texture_coord_index_1[1] * float4(input.vTex1, 1, 1)
		+ uniform_texture_coord_index_1[2] * float4(position_view, 1)
		+ uniform_texture_coord_index_1[3] * float4(reflect_view, 1)
		);

	output.TexCoord1 = mul(tmp, uniform_texture_coord_matrix1).xy;
	output.color = 1.f;

	if (stuffType == ST_MESH || stuffType == ST_DISTORT)
	{
		output.color.a = material.diffuse.a;
	}
	else if (stuffType == ST_BILLBOARD || stuffType == ST_BAND)
	{
		output.color.a = 1.f;
	}
	else if (stuffType == ST_PARTICLE)
	{
		output.color = input.vertexColor;
	}
	else
	{
		output.color.a = input.vertexColor.a;
	}

	return output;
}

vector PS_BASE_NOSHADOW(VS_OUTPUT_BASE_NOSHADOW In) : COLOR
{
	if (stuffType == ST_DISTORT || stuffType == ST_GLASS)
	{
		In.TexCoord0 /= In.TexCoord0.z;
	}

	float4 gl_FragColor = 0.f;
	float4 diffuse = In.color;
	float4 color_texture = tex2D(S0, In.TexCoord0);

	float alpha1 =
		uniform_stage0_alpha_arg1[1] * color_texture.a +
		uniform_stage0_alpha_arg1[2] * uniform_texture_factor.a;
	float alpha2 =
		uniform_stage0_alpha_arg2[0] * diffuse.a/*varing_diffuse.a*/ +
		uniform_stage0_alpha_arg2[2] * uniform_texture_factor.a;

	gl_FragColor.a =
		uniform_stage0_alpha_op[0] * alpha1 +
		uniform_stage0_alpha_op[4] * alpha1 * alpha2;

	//if (gl_FragColor.a <= uniform_alpha_test)
	//{
	//	discard;
	//	return color_texture;
	//}

	float3 color1 = uniform_stage0_color_arg1[1] * color_texture.rgb
		+ uniform_stage0_color_arg1[2] * uniform_texture_factor.rgb;
	float3 color2 = uniform_stage0_color_arg2[0] * diffuse.rgb/*varing_diffuse.rgb*/
		+ uniform_stage0_color_arg2[2] * uniform_texture_factor.rgb
		+ uniform_stage0_color_arg2[3] * color_texture.a;

	gl_FragColor.rgb =
		uniform_stage0_color_op[0] * color1 +
		uniform_stage0_color_op[4] * color1 * color2;

	if (uniform_texture_stage >= 2)
	{
		color_texture = tex2D(S1, In.TexCoord1);

		float4 color1_all =
			uniform_stage1_color_arg1[0] * gl_FragColor +
			uniform_stage1_color_arg1[1] * color_texture;

		float3 color1 = color1_all.rgb;
		float3 color2 =
			uniform_stage1_color_arg2[0] * gl_FragColor.rgb +
			uniform_stage1_color_arg2[1] * color_texture.rgb +
			uniform_stage1_color_arg2[2] * uniform_texture_factor.rgb;

		float coef1 =
			uniform_stage1_color_op[0] * 1.0 +
			uniform_stage1_color_op[5] * color_texture.a +
			uniform_stage1_color_op[6] * gl_FragColor.a;
		float coef2 =
			uniform_stage1_color_op[1] * 1.0 +
			uniform_stage1_color_op[10] * (1.0 - color_texture.a) +
			uniform_stage1_color_op[6] * (1.0 - gl_FragColor.a) +
			uniform_stage1_color_op[8] * (1.0 - uniform_texture_factor.a) +
			uniform_stage1_color_op[7] * (color1_all.a) +
			uniform_stage1_color_op[9] * (1.0 - color1_all.a);

		gl_FragColor.rgb =
			uniform_stage1_color_op[3] *
			(
			coef1 * color1
			+ coef2 * color2
			+ uniform_stage1_color_op[2]
		)
			+ uniform_stage1_color_op[4] * color1 * color2
			+ uniform_stage1_color_op[12] * (color1 * color2 + color1_all.a)
			+ uniform_stage1_color_op[13] * ((1.0 - color1) * color2 + color1_all.a);

		alpha1 =
			uniform_stage1_alpha_arg1[0] * gl_FragColor.a +
			uniform_stage1_alpha_arg1[1] * color_texture.a;
		alpha2 =
			uniform_stage1_alpha_arg2[0] * gl_FragColor.a +
			uniform_stage1_alpha_arg2[1] * color_texture.a;

		gl_FragColor.a =
			uniform_stage1_alpha_op[3] *
			(
			uniform_stage1_alpha_op[0] * alpha1 +
			uniform_stage1_alpha_op[1] * alpha2 +
			uniform_stage1_alpha_op[2]
		) +
			uniform_stage1_alpha_op[4] * alpha1 * alpha2;
	}

	//조명
	float4 lightColor = CalculateDirectLights(In.viewDir, In.reflection, In.diffuse);

	//라이트 켰을때만 라이트 연산
	if (uniform_lighting_enable)
	{
		gl_FragColor.rgb *= lightColor.rgb;
	}
	if (uniform_fog_enable)
	{
		gl_FragColor.rgb = lerp(uniform_fog_color.rgb, clamp(gl_FragColor.rgb, 0.0, 1.0), In.fog);
	}

	return gl_FragColor;
}

//이거 이넘이랑 순서 마춰야됨.
technique TECHNIQUE_BASE
{
	pass p0
	{
		VertexShader = compile vs_3_0 VS_BASE();
		PixelShader = compile ps_3_0 PS_BASE();
	}
}

technique TECHNIQUE_BASE_NOSHADOW
{
	pass p0
	{
		VertexShader = compile vs_3_0 VS_BASE_NOSHADOW();
		PixelShader = compile ps_3_0 PS_BASE_NOSHADOW();
	}
};