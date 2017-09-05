#pragma once
#ifdef C_USE_SHADER_PIPELINE
const int GLOBAL_CONST_MAX_TEXTURE = 2;
const int GLOBAL_CONST_MAX_BONE_MATRIX = 4;

#define MAX_POINT_LIGHTS 8

#ifdef C_POSTEFFECT
const int MAX_SAMPLES = 16;
#endif //C_POSTEFFECT

struct ShaderInfo
{
	int technique;
	int pass;

	ShaderInfo() :technique(INVALID_VALUE), pass(INVALID_VALUE){}
	ShaderInfo(int tec, int pas)
	{
		technique = tec;
		pass = pas;
	}
};
typedef std::map<std::pair<int/*ST*/, int/*option*/>, ShaderInfo> ShaderIndexMap;

typedef std::map<int /*태크닉 번호*/, D3DXHANDLE /*태크닉 핸들*/> ShaderTechnique;
struct EffectBlock
{
	LPD3DXEFFECT effect;
	ShaderTechnique techniqueList;
};
typedef std::map<int /*이펙트 번호*/, EffectBlock > ShaderEffect;

enum SHADER_BASE_IDX//Shader Pass(State)
{
	TECHNIQUE_BASE = 0,
	TECHNIQUE_BASE_NOSHADOW,
	TECHNIQUE_END,
};

#ifdef C_POSTEFFECT
enum SHADER_HDR_IDX//Shader Pass(State)
{
	TECHNIQUE_HDR_BrightPassFilter,
	TECHNIQUE_DownSample,
	TECHNIQUE_BLUR_X,
	TECHNIQUE_BLUR_Y,
	TECHNIQUE_FINAL,
	TECHNIQUE_GreyScaleDownSample,
	TECHNIQUE_LuminanceDownSample,
	TECHNIQUE_HDR_END,
};
#endif //C_POSTEFFECT

enum Shaderffect_IDX//Shader effect(State)
{
	EFFECT_BASE = 0,
	EFFECT_TERRAIN,
	EFFECT_SHADOW,
	EFFECT_UI,
	EFFECT_REFLECT,
	EFFECT_WATER,

#ifdef C_POSTEFFECT
	EFFECT_HDR,
#endif //C_POSTEFFECT
	EFFECT_END,
};

enum SHADER_PASS
{
	PASS_0 = 0,
	PASS_1 = 1,
	PASS_2 = 2,
	PASS_3 = 3,
	PASS_4 = 4,
};

enum MatrixType
{
	MATRIX_WORLD,
	MATRIX_VIEW,
	MATRIX_PROJECTION,
	MATRIX_INVERSE_VIEW_WORLD,
	MATRIX_END,
};

struct TextureStageState
{
	DWORD color_op;
	DWORD color_arg1;
	DWORD color_arg2;
	DWORD alpha_op;
	DWORD alpha_arg1;
	DWORD alpha_arg2;

	DWORD texture_coord_index;
	DWORD texture_coord_flag;
	D3DXMATRIX texture_coord_matrix;

	TextureStageState() : color_op(0)
	{}
};

struct RenderState
{
	bool lighting;
	bool alpha_test_enable;
	DWORD alpha_test_ref;
	DWORD texture_factor;
	int vertex_blend;
	int texture_stage;
	bool fog_enable;
	float fog_start;
	float fog_end;
	DWORD fog_Color;

	TextureStageState texture[GLOBAL_CONST_MAX_TEXTURE];
	D3DXMATRIXA16 matBumpMat;

	RenderState() : vertex_blend(0)
	{}
};

struct BindState
{
	int stuffType;
	int blendBoneNum;
	int iEnv0;
	float env_dx0;
	float shadow_size;
	float shadow_size_LowQuality;
	float reflectLevel;
	D3DXVECTOR2 shadowPos;
	D3DXVECTOR2 shadowPos_LowQuality;
	D3DLIGHT9 dxlight;
	float userHeight;
	float waterWave;

	D3DMATERIAL9 material;
	bool isStepAble;
	bool isNormalMapFrame;
	bool isChangeLightingCheck;

	BindState() : blendBoneNum(-1), iEnv0(-1), stuffType(-1), isStepAble(true), isNormalMapFrame(true), env_dx0(-1)
	{}
};
#endif //C_USE_SHADER_PIPELINE

//////////////////////////////////////////////////////////////////////////
#pragma once
#include "ShaderType.h"
#ifdef C_USE_SHADER_PIPELINE

class CBaseStuff;

class CShaderDevice
{
public:
	CShaderDevice(void);
	virtual ~CShaderDevice(void);

	void DeleteDeviceObjects();
	void InvalidateDeviceObjects();
	void RestoreDeviceObjects();

	LPD3DXEFFECT GetShader() const { return pShader_; }
	D3DXHANDLE GetTechniqueHandle(int effectIdx ,int technique) const;
	HRESULT ChangeShaderEffect(int effectIdx);
	HRESULT Init();

	void bind(int bindType);
private:
	void SetDefaultSetting();

#ifdef C_POSTEFFECT
	void TechniqueHDR_Bind();
#endif //C_POSTEFFECT
	void TechniqueBase_Bind(int bindType);
	void TechniqueReflect_Bind(int bindType);
	void TechniqueShadow_Bind();
	void TechniqueTerrain_Bind();
	void TechniqueUI_Bind(int bindType);
	void TechniqueWater_Bind();

	//랜더스테이트 바인딩 분리
	void BindRS_Lighting(bool force = false);
	void BindRS_AlphaTest(bool force = false);
	void BindRS_TextureFactor(bool force = false);
	void BindRS_Fog(bool force = false);
	void BindRS_VertexBlend(bool force = false);
	void BindRS_TextureStage(bool force = false);
	void BindRS_Bump(bool force = false);

	void BindWVPMatrix();
	void BindUIMatrix();
	void BindMeterial();
	void BindFrameMaterialForOpticalMesh();
	void BindShadowInfo();
	void BindBoneInfo();
	void BindCamera();
	void BindLight();
	void BindReflectLevel();
	void BindStuffType(int bindType);
	void BindWaterInfo();
	void BindStepAble();
	void BindNormalMapFrame();
	void BindChangeLightingCheck();

	LPD3DXEFFECT pShader_; //쉐이더 소스 자체라고 보면된다.
	ShaderEffect shaderEffectList_;
	LPD3DXEFFECTPOOL pEffectPool_;

	BindState bindState_;

	//그림자용 변수 추가.
	D3DXHANDLE userHeight_; //그림자 맵 보정 용도.
	D3DXHANDLE shadow_pos_;
	D3DXHANDLE shadow_size_;
	D3DXHANDLE shadow_pos_LowQuality;
	D3DXHANDLE shadow_size_LowQuality;

	D3DXHANDLE mat_world_;
	D3DXHANDLE mat_view_;
	D3DXHANDLE mat_proj_;

	D3DXHANDLE cameraPos_;

	D3DXHANDLE blendBoneNum_;
	D3DXHANDLE boneMatrix_;

	//환경맵핑
	D3DXHANDLE env0_;
	D3DXHANDLE env_dx0_;

	D3DXHANDLE stuffType_;

	D3DXHANDLE fog_enable_;
	D3DXHANDLE fog_start_;
	D3DXHANDLE fog_end_;
	D3DXHANDLE fog_color_;

	D3DXHANDLE clipHeight_;
	D3DXHANDLE waterWave_;
	D3DXHANDLE isStepAble_;
	D3DXHANDLE isNormalMapFrame_;
	D3DXHANDLE isChangeLightingCheck_;

	D3DXHANDLE bumpMat11_;
	D3DXHANDLE bumpMat22_;

	//고정 기능 구현을 위한 변수
	D3DXHANDLE render_state_vertex_blend_;

	D3DXHANDLE color_op_[2];
	D3DXHANDLE color_arg1_[2];
	D3DXHANDLE color_arg2_[2];

	D3DXHANDLE alpha_op_[2];
	D3DXHANDLE alpha_arg1_[2];
	D3DXHANDLE alpha_arg2_[2];

	D3DXHANDLE texCoord_[2];
	D3DXHANDLE texCoordMat_[2];

	D3DXHANDLE lighting_;
	D3DXHANDLE textureFactor_;
	D3DXHANDLE alphaTest_; //픽셸 쉐이더에서 디스카드 가능.
	D3DXHANDLE textureStageCnt_;

	struct ligtHandle
	{
		D3DXHANDLE pos;
		D3DXHANDLE ambient;
		D3DXHANDLE diffuse;
		D3DXHANDLE specular;
		D3DXHANDLE radius;
	};

	ligtHandle directLight_;

	struct MaterialHandle
	{
		D3DXHANDLE ambient;
		D3DXHANDLE diffuse;
		D3DXHANDLE emissive;
		D3DXHANDLE specular;
		D3DXHANDLE shininess;
	};

	MaterialHandle meterial_;

#ifdef C_POSTEFFECT
	//HDR
	D3DXHANDLE brightThreshold;
	D3DXHANDLE sampleWeight_;
	D3DXHANDLE TextureSize_;

	D3DXHANDLE bloomWeights;
	D3DXHANDLE bloomOffsets;

	D3DXHANDLE rcp_bloom_tex_w;
	D3DXHANDLE rcp_bloom_tex_h;
	D3DXHANDLE Exposure;
	D3DXHANDLE GaussianScaler;
#endif // C_POSTEFFECT
};
#endif //C_USE_SHADER_PIPELINE

//////////////////////////////////////////////////////////////////////////////////////

#ifdef C_USE_SHADER_PIPELINE
CShaderDevice::CShaderDevice(void):
pShader_(NULL),
pEffectPool_(NULL)
{
} 

CShaderDevice::~CShaderDevice(void)
{
	SAFE_RELEASE(pShader_);
	SAFE_RELEASE(pEffectPool_);
}

void CShaderDevice::DeleteDeviceObjects()
{
	ShaderEffect::iterator iter;

	iter = shaderEffectList_.begin();

	for (; iter != shaderEffectList_.end(); ++iter)
	{
		SAFE_RELEASE(iter->second.effect)
	}
}

void CShaderDevice::InvalidateDeviceObjects()
{
	ShaderEffect::iterator iter;

	iter = shaderEffectList_.begin();

	for (; iter != shaderEffectList_.end(); ++iter)
	{
		iter->second.effect->OnLostDevice();
	}
}

void CShaderDevice::RestoreDeviceObjects()
{
	ShaderEffect::iterator iter;

	iter = shaderEffectList_.begin();

	for (; iter != shaderEffectList_.end(); ++iter)
	{
		iter->second.effect->OnResetDevice();
	}
}

void CShaderDevice::bind(int bindType)
{
	switch (g_ShaderRenderMgr.effectIdx)
	{
		case EFFECT_BASE:
		{
			TechniqueBase_Bind(bindType);
			break;
		}
		case EFFECT_REFLECT:
		{
			TechniqueReflect_Bind(bindType);
			break;
		}
		case EFFECT_SHADOW:
		{
			TechniqueShadow_Bind();
			break;
		}
		case EFFECT_TERRAIN:
		{
			TechniqueTerrain_Bind();
			break;
		}
		case EFFECT_UI:
		{
			TechniqueUI_Bind(bindType);
			break;
		}
#ifdef C_POSTEFFECT
		case EFFECT_HDR:
		{
			TechniqueHDR_Bind();
			break;
		}
#endif //C_POSTEFFECT
		case EFFECT_WATER:
		{
			TechniqueWater_Bind();
			break;
		}
	}

	g_ShaderRenderMgr.blendBoneNum = 0;
	g_ShaderRenderMgr.GetShader()->pShader_->CommitChanges();
}

#ifdef C_POSTEFFECT
void CShaderDevice::TechniqueHDR_Bind()
{
	//계속 바뀌는거라 딱히 변수로 구분할 필요 없을듯.
	pShader_->SetVectorArray(sampleWeight_, g_ShaderRenderMgr.postEffectRenderer.sampleOffsets
		, g_ShaderRenderMgr.postEffectRenderer.count);

	pShader_->SetFloat(brightThreshold, g_ShaderRenderMgr.postEffectRenderer.brightThreshold);

	pShader_->SetFloatArray(bloomWeights, g_ShaderRenderMgr.postEffectRenderer.bloomWeights, 9);
	pShader_->SetFloatArray(bloomOffsets, g_ShaderRenderMgr.postEffectRenderer.bloomOffsets, 9);

	pShader_->SetFloat(rcp_bloom_tex_w, g_ShaderRenderMgr.postEffectRenderer.rcp_bloom_tex_w);
	pShader_->SetFloat(rcp_bloom_tex_h, g_ShaderRenderMgr.postEffectRenderer.rcp_bloom_tex_h);
	pShader_->SetFloat(Exposure, g_ShaderRenderMgr.postEffectRenderer.Exposure);
	pShader_->SetFloat(GaussianScaler, g_ShaderRenderMgr.postEffectRenderer.GaussianScaler);
}
#endif //C_POSTEFFECT

void CShaderDevice::TechniqueBase_Bind(int bindType)
{
	BindChangeLightingCheck();
	BindRS_Lighting();
	BindRS_TextureFactor();
	BindRS_Fog();
	BindRS_VertexBlend();
	BindRS_TextureStage();

	BindStepAble();
	BindNormalMapFrame();
	BindStuffType(bindType);
	BindWVPMatrix();
	BindBoneInfo();
	BindMeterial();
	BindFrameMaterialForOpticalMesh();
	BindCamera();
	BindLight();
	BindShadowInfo();
}

void CShaderDevice::TechniqueReflect_Bind(int bindType)
{
	BindChangeLightingCheck();
	BindRS_Lighting();
	BindRS_TextureFactor();
	BindRS_VertexBlend();
	BindRS_TextureStage();

	BindStuffType(bindType);
	BindWVPMatrix();
	BindBoneInfo();
	BindMeterial();
	BindFrameMaterialForOpticalMesh();
	BindCamera();
	BindLight();
	BindReflectLevel();
}

void CShaderDevice::TechniqueShadow_Bind()
{	
	BindRS_AlphaTest();
	BindRS_VertexBlend();

	BindWVPMatrix();
	BindBoneInfo();
	BindCamera();
	BindLight();
	BindShadowInfo();
}

void CShaderDevice::TechniqueTerrain_Bind()
{
	BindChangeLightingCheck();
	BindRS_TextureFactor();
	BindRS_Fog();
	BindRS_TextureStage();

	BindWVPMatrix();
	BindBoneInfo();
	BindMeterial();
	BindFrameMaterialForOpticalMesh();
	BindCamera();
	BindLight();
	BindShadowInfo();
}

void CShaderDevice::TechniqueUI_Bind(int bindType)
{
	BindRS_TextureFactor();
	BindRS_TextureStage();

	BindStuffType(bindType);
	(bindType == ST_UI) ? BindUIMatrix() : BindWVPMatrix();
	BindMeterial();
}

void CShaderDevice::TechniqueWater_Bind()
{
	BindRS_TextureFactor();
	BindRS_Fog();
	BindRS_Bump();
	BindRS_TextureStage();

	BindWVPMatrix();
	BindCamera();
	BindShadowInfo();
	BindWaterInfo();
}

HRESULT CShaderDevice::Init()
{
	LPD3DXBUFFER	pErr;

	//쉐이더 이펙트 끼리 공유하는 메모리풀
	if (FAILED(D3DXCreateEffectPool(&pEffectPool_)))
	{
		return E_FAIL;
	}

#ifdef C_USE_FX_EFFECT_FILE
	string str = string(g_pszProgramPath) + "\\Snake\\HLSL_BASE.fx";
#else
	string str = string(g_pszProgramPath) + "\\Data\\Shader\\HLSL_BASE.obj";
#endif //DEBUG



	WCHAR wStr[256];


	MultiByteToWideChar(CP_ACP, 0, str.c_str(), strlen(str.c_str()) + 1, wStr, sizeof(wStr) / sizeof(wStr[0]));

	// 이펙트 프레임워크 생성
	if (FAILED(D3DXCreateEffectFromFileW(g_pd3dDevice,
		wStr, 0, 0, 0,
		pEffectPool_, &pShader_, &pErr)))
	{
		return E_FAIL;
	}

	SAFE_RELEASE(pErr);

	mat_view_ = pShader_->GetParameterByName(NULL, "matView");
	mat_proj_ = pShader_->GetParameterByName(NULL, "matProj");
	mat_world_ = pShader_->GetParameterByName(NULL, "matWorld");

	cameraPos_ = pShader_->GetParameterByName(NULL, "cameraPosition");

	for (DWORD i = 0; i < CShaderRenderMgr::MAX_TEXTURE_STAGE; ++i)
	{
		texCoord_[i] = pShader_->GetParameterByName(NULL,
			boost::str(boost::format("uniform_texture_coord_index_%d") % i).c_str());

		texCoordMat_[i] = pShader_->GetParameterByName(NULL,
			boost::str(boost::format("uniform_texture_coord_matrix%d") % i).c_str());

		color_op_[i] = pShader_->GetParameterByName(NULL,
			boost::str(boost::format("uniform_stage%d_color_op") % i).c_str());
		color_arg1_[i] = pShader_->GetParameterByName(NULL,
			boost::str(boost::format("uniform_stage%d_color_arg1") % i).c_str());
		color_arg2_[i] = pShader_->GetParameterByName(NULL,
			boost::str(boost::format("uniform_stage%d_color_arg2") % i).c_str());
		alpha_op_[i] = pShader_->GetParameterByName(NULL,
			boost::str(boost::format("uniform_stage%d_alpha_op") % i ).c_str());
		alpha_arg1_[i] = pShader_->GetParameterByName(NULL,
			boost::str(boost::format("uniform_stage%d_alpha_arg1") % i).c_str());
		alpha_arg2_[i] = pShader_->GetParameterByName(NULL,
			boost::str(boost::format("uniform_stage%d_alpha_arg2") % i).c_str());
	}

	lighting_ = pShader_->GetParameterByName(NULL, "uniform_lighting_enable");
	textureFactor_ = pShader_->GetParameterByName(NULL, "uniform_texture_factor");
	alphaTest_ = pShader_->GetParameterByName(NULL, "uniform_alpha_test");
	textureStageCnt_ = pShader_->GetParameterByName(NULL, "uniform_texture_stage");


	render_state_vertex_blend_ = pShader_->GetParameterByName(NULL, "uniform_vertex_blend");

	fog_enable_ = pShader_->GetParameterByName(NULL, "uniform_fog_enable");
	fog_start_ = pShader_->GetParameterByName(NULL, "uniform_fog_start");
	fog_end_ = pShader_->GetParameterByName(NULL, "uniform_fog_end");
	fog_color_ = pShader_->GetParameterByName(NULL, "uniform_fog_color");

	bumpMat11_ = pShader_->GetParameterByName(NULL, "bumpMat11");
	bumpMat22_ = pShader_->GetParameterByName(NULL, "bumpMat22");

	SetDefaultSetting();

	boneMatrix_ = pShader_->GetParameterByName(NULL, "boneMatrixArray");
	blendBoneNum_ = pShader_->GetParameterByName(NULL, "blendBoneNum");

	env0_ = pShader_->GetParameterByName(NULL, "env0");
	env_dx0_ = pShader_->GetParameterByName(NULL, "env_dx0");
	stuffType_ = pShader_->GetParameterByName(NULL, "stuffType");

	//그림자용 변수 추가.
	shadow_pos_ = pShader_->GetParameterByName(NULL, "uniform_shadow_pos");
	shadow_size_ = pShader_->GetParameterByName(NULL, "uniform_shadow_size");
	shadow_pos_LowQuality = pShader_->GetParameterByName(NULL, "uniform_shadow_pos_LowQuality");
	shadow_size_LowQuality = pShader_->GetParameterByName(NULL, "uniform_shadow_size_LowQuality");

	clipHeight_ = pShader_->GetParameterByName(NULL, "clipHeight");

	userHeight_ = pShader_->GetParameterByName(NULL, "userHeight");
	waterWave_ = pShader_->GetParameterByName(NULL, "waterWave");
	isStepAble_ = pShader_->GetParameterByName(NULL, "isStepAble");
	isNormalMapFrame_ = pShader_->GetParameterByName(NULL, "isNormalMapFrame");
	isChangeLightingCheck_ = pShader_->GetParameterByName(NULL, "isChangeLightingCheck");

	//다이렉트 라이트
	D3DXHANDLE hdirectLight = pShader_->GetParameterByName(NULL, "directlight");
	directLight_.pos = pShader_->GetParameterByName(hdirectLight, "pos");
	directLight_.ambient = pShader_->GetParameterByName(hdirectLight, "ambient");
	directLight_.diffuse = pShader_->GetParameterByName(hdirectLight, "diffuse");
	directLight_.specular = pShader_->GetParameterByName(hdirectLight, "specular");
	directLight_.radius = pShader_->GetParameterByName(hdirectLight, "radius");

	D3DXHANDLE hMaterial = pShader_->GetParameterByName(NULL, "material");
	meterial_.ambient = pShader_->GetParameterByName(hMaterial, "ambient");
	meterial_.diffuse = pShader_->GetParameterByName(hMaterial, "diffuse");
	meterial_.emissive = pShader_->GetParameterByName(hMaterial, "emissive");
	meterial_.specular = pShader_->GetParameterByName(hMaterial, "specular");
	meterial_.shininess = pShader_->GetParameterByName(hMaterial, "shininess");

#ifdef C_POSTEFFECT
	//HDR
	sampleWeight_ = pShader_->GetParameterByName(NULL, "tcDownSampleOffsets");

	brightThreshold = pShader_->GetParameterByName(NULL, "brightThreshold");

	bloomWeights = pShader_->GetParameterByName(NULL, "BloomWeights");
	bloomOffsets = pShader_->GetParameterByName(NULL, "BloomOffsets");

	rcp_bloom_tex_w = pShader_->GetParameterByName(NULL, "rcp_bloom_tex_w");
	rcp_bloom_tex_h = pShader_->GetParameterByName(NULL, "rcp_bloom_tex_h");
	Exposure = pShader_->GetParameterByName(NULL, "Exposure");;
	GaussianScaler = pShader_->GetParameterByName(NULL, "GaussianScaler");

#endif //C_POSTEFFECT

	EffectBlock block;

	block.effect = pShader_;

	//태크닉 설정.
	for (int i = 0; i < TECHNIQUE_END; ++i)
	{
#ifdef VS2015
		block.techniqueList.emplace(i, pShader_->GetTechnique(i));
#else //VS2015
		block.techniqueList.insert(std::make_pair<int, D3DXHANDLE>(i, pShader_->GetTechnique(i)));
#endif //VS2015
	}
	
#ifdef VS2015
	shaderEffectList_.emplace(0, block);
#else //VS2015
	shaderEffectList_.insert(std::make_pair<int, EffectBlock>(0, block));
#endif //VS2015

	block.techniqueList.clear();
	//이펙트 설정.
	string path[EFFECT_END];

	int cnt = 1;
#ifdef C_USE_FX_EFFECT_FILE
	path[cnt++] = string(g_pszProgramPath) + "\\Snake\\HLSL_TERRAIN.fx";
	path[cnt++] = string(g_pszProgramPath) + "\\Snake\\HLSL_Shadow.fx";
	path[cnt++] = string(g_pszProgramPath) + "\\Snake\\HLSL_UI.fx";
	path[cnt++] = string(g_pszProgramPath) + "\\Snake\\HLSL_Reflect.fx";
	path[cnt++] = string(g_pszProgramPath) + "\\Snake\\HLSL_Water.fx";
	path[cnt++] = string(g_pszProgramPath) + "\\Snake\\HLSL_HDR.fx";
#else
	path[cnt++] = string(g_pszProgramPath) + "\\Data\\Shader\\HLSL_TERRAIN.obj";
	path[cnt++] = string(g_pszProgramPath) + "\\Data\\Shader\\HLSL_Shadow.obj";
	path[cnt++] = string(g_pszProgramPath) + "\\Data\\Shader\\HLSL_UI.obj";
	path[cnt++] = string(g_pszProgramPath) + "\\Data\\Shader\\HLSL_Reflect.obj";
	path[cnt++] = string(g_pszProgramPath) + "\\Data\\Shader\\HLSL_Water.obj";
	path[cnt++] = string(g_pszProgramPath) + "\\Data\\Shader\\HLSL_HDR.obj";
#endif //DEBUG


	for (int i = 1; i < cnt; ++i)
	{
		//한글 경로 문제때문에 유니코드로 처리.
		MultiByteToWideChar(CP_ACP, 0, path[i].c_str(), strlen(path[i].c_str()) + 1, wStr, sizeof(wStr) / sizeof(wStr[0]));

		if (FAILED(D3DXCreateEffectFromFileW(g_pd3dDevice,
			wStr, 0, 0, 0,
			pEffectPool_, &pShader_, &pErr)))
		{
			return E_FAIL;
		}

		block.techniqueList.clear();

		block.effect = pShader_;

		int j = 0;

		while(true)
		{
			D3DXHANDLE h = pShader_->GetTechnique(j);

			if (h == NULL)
			{
				break;
			}

#ifdef VS2015
			block.techniqueList.emplace(j, h);
#else //VS2015
			block.techniqueList.insert(std::make_pair<int, D3DXHANDLE>(j, h));
#endif //VS2015

			j++;
		}

#ifdef VS2015
		shaderEffectList_.emplace(i, block);
#else //VS2015
		shaderEffectList_.insert(std::make_pair<int, EffectBlock>(i, block));
#endif //VS2015
	}
	
	return S_OK;
}

HRESULT CShaderDevice::ChangeShaderEffect(int effectIdx)
{
	ShaderEffect::iterator iter;

	iter = shaderEffectList_.find(effectIdx);

	if (iter != shaderEffectList_.end())
	{
		pShader_ = iter->second.effect;

		return S_OK;
	}
	
	return E_FAIL;
}

D3DXHANDLE CShaderDevice::GetTechniqueHandle(int effectIdx, int technique) const
{
	ShaderEffect::const_iterator iter;
	ShaderTechnique::const_iterator uter;

	iter = shaderEffectList_.find(effectIdx);

	if (iter != shaderEffectList_.end())
	{
		uter = iter->second.techniqueList.find(technique);

		if (uter != iter->second.techniqueList.end())
		{
			return uter->second;
		}
	}

	return NULL;
}

void CShaderDevice::SetDefaultSetting()
{
	BindRS_AlphaTest(true);
	BindRS_Lighting(true);
	BindRS_TextureFactor(true);
	BindRS_Fog(true);
	BindRS_VertexBlend(true);
	BindRS_Bump(true);
	BindRS_TextureStage(true);
}

void CShaderDevice::BindWVPMatrix()
{
	if (g_ShaderRenderMgr.mat_main_changed[0] == true)
	{
		pShader_->SetMatrix(mat_world_, &g_ShaderRenderMgr.mat_main[0]);
		g_ShaderRenderMgr.mat_main_changed[0] = false;
	}
	if (g_ShaderRenderMgr.mat_main_changed[1] == true)
	{
		pShader_->SetMatrix(mat_view_, &g_ShaderRenderMgr.mat_main[1]);
		g_ShaderRenderMgr.mat_main_changed[1] = false;
	}
	if (g_ShaderRenderMgr.mat_main_changed[2] == true)
	{
		pShader_->SetMatrix(mat_proj_, &g_ShaderRenderMgr.mat_main[2]);
		g_ShaderRenderMgr.mat_main_changed[2] = false;
	}

	g_ShaderRenderMgr.isUIMatrix = false;
}

void CShaderDevice::BindUIMatrix()
{
	if (!g_ShaderRenderMgr.isUIMatrix)
	{
		pShader_->SetMatrix(mat_world_, &g_matIdentity);
		pShader_->SetMatrix(mat_view_, &g_matIdentity);
		pShader_->SetMatrix(mat_proj_, &g_ShaderRenderMgr.mat_2d_project);
		g_ShaderRenderMgr.isUIMatrix = true;
		memset(g_ShaderRenderMgr.mat_main_changed, true, sizeof(g_ShaderRenderMgr.mat_main_changed));
	}
}

void CShaderDevice::BindMeterial()
{
	if (g_ShaderRenderMgr.material->Diffuse.r != bindState_.material.Diffuse.r
		|| g_ShaderRenderMgr.material->Diffuse.g != bindState_.material.Diffuse.g
		|| g_ShaderRenderMgr.material->Diffuse.b != bindState_.material.Diffuse.b
		|| g_ShaderRenderMgr.material->Diffuse.a != bindState_.material.Diffuse.a)
	{
		pShader_->SetFloatArray(meterial_.diffuse, &g_ShaderRenderMgr.material->Diffuse.r, 4);
		bindState_.material.Diffuse = g_ShaderRenderMgr.material->Diffuse;
	}

	if (g_ShaderRenderMgr.material->Ambient.r != bindState_.material.Ambient.r
		|| g_ShaderRenderMgr.material->Ambient.g != bindState_.material.Ambient.g
		|| g_ShaderRenderMgr.material->Ambient.b != bindState_.material.Ambient.b
		|| g_ShaderRenderMgr.material->Ambient.a != bindState_.material.Ambient.a)
	{
		pShader_->SetFloatArray(meterial_.ambient, &g_ShaderRenderMgr.material->Ambient.r, 4);
		bindState_.material.Ambient = g_ShaderRenderMgr.material->Ambient;
	}

	if (g_ShaderRenderMgr.material->Emissive.r != bindState_.material.Emissive.r
		|| g_ShaderRenderMgr.material->Emissive.g != bindState_.material.Emissive.g
		|| g_ShaderRenderMgr.material->Emissive.b != bindState_.material.Emissive.b
		|| g_ShaderRenderMgr.material->Emissive.a != bindState_.material.Emissive.a)
	{
		pShader_->SetFloatArray(meterial_.emissive, &g_ShaderRenderMgr.material->Emissive.r, 4);
		bindState_.material.Emissive = g_ShaderRenderMgr.material->Emissive;
	}

	/*
	//일단 라이트 연산할때 사용안하니깐 주석. 필요하면 주석 풀고 사용 하자.

	if (g_ShaderRenderMgr.material->Specular.r != bindState_.material.Specular.r
		|| g_ShaderRenderMgr.material->Specular.g != bindState_.material.Specular.g
		|| g_ShaderRenderMgr.material->Specular.b != bindState_.material.Specular.b
		|| g_ShaderRenderMgr.material->Specular.a != bindState_.material.Specular.a)
	{
		pShader_->SetFloatArray(meterial_.specular, &g_ShaderRenderMgr.material->Specular.r, 4);
		bindState_.material.Specular = g_ShaderRenderMgr.material->Specular;
	}

	if (g_ShaderRenderMgr.material->Power != bindState_.material.Power)
	{
		pShader_->SetFloat(meterial_.shininess, g_ShaderRenderMgr.material->Power);
		bindState_.material.Power = g_ShaderRenderMgr.material->Power;
	}
	*/
}

void CShaderDevice::BindFrameMaterialForOpticalMesh()
{
	if (g_ShaderRenderMgr.frameMaterial)
	{
		if (g_ShaderRenderMgr.frameMaterial->iEnv0 != bindState_.iEnv0)
		{
			pShader_->SetInt(env0_, g_ShaderRenderMgr.frameMaterial->iEnv0);
			bindState_.iEnv0 = g_ShaderRenderMgr.frameMaterial->iEnv0;
		}
		if (g_ShaderRenderMgr.frameMaterial->dx0 != bindState_.env_dx0)
		{
			pShader_->SetFloat(env_dx0_, g_ShaderRenderMgr.frameMaterial->dx0);
			bindState_.env_dx0 = g_ShaderRenderMgr.frameMaterial->dx0;
		}
	}
	else
	{
		if (0 != bindState_.iEnv0)
		{
			pShader_->SetInt(env0_, 0);
			bindState_.iEnv0 = 0;
		}
		if (fabs(bindState_.env_dx0 - 0.f) > 0.001f)
		{
			pShader_->SetFloat(env_dx0_, 0.f);
			bindState_.env_dx0 = 0.f;
		}
	}
}

void CShaderDevice::BindShadowInfo()
{
	if (g_ShaderRenderMgr.shadow_size != bindState_.shadow_size)
	{
		pShader_->SetFloat(shadow_size_, g_ShaderRenderMgr.shadow_size);
		bindState_.shadow_size = g_ShaderRenderMgr.shadow_size;
	}

	if (g_ShaderRenderMgr.shadowPos != bindState_.shadowPos)
	{
		pShader_->SetFloatArray(shadow_pos_, g_ShaderRenderMgr.shadowPos, 2);
		bindState_.shadowPos = g_ShaderRenderMgr.shadowPos;
	}

	if (g_ShaderRenderMgr.shadow_size_LowQuality != bindState_.shadow_size_LowQuality)
	{
		pShader_->SetFloat(shadow_size_LowQuality, g_ShaderRenderMgr.shadow_size_LowQuality);
		bindState_.shadow_size_LowQuality = g_ShaderRenderMgr.shadow_size_LowQuality;
	}

	if (g_ShaderRenderMgr.shadowPos_LowQuality != bindState_.shadowPos_LowQuality)
	{
		pShader_->SetFloatArray(shadow_pos_LowQuality, g_ShaderRenderMgr.shadowPos_LowQuality, 2);
		bindState_.shadowPos_LowQuality = g_ShaderRenderMgr.shadowPos_LowQuality;
	}

	if (g_ShaderRenderMgr.userHeight != bindState_.userHeight)
	{
		pShader_->SetFloat(userHeight_, g_ShaderRenderMgr.userHeight);
		bindState_.userHeight = g_ShaderRenderMgr.userHeight;
	}
}

void CShaderDevice::BindBoneInfo()
{
	if (g_ShaderRenderMgr.blendBoneNum != bindState_.blendBoneNum)
	{
		pShader_->SetInt(blendBoneNum_, g_ShaderRenderMgr.blendBoneNum);
		bindState_.blendBoneNum = g_ShaderRenderMgr.blendBoneNum;
	}

	if (g_ShaderRenderMgr.bone_matrix_changed == true)
	{
		pShader_->SetMatrixArray(boneMatrix_, g_ShaderRenderMgr.bone_matrix, g_ShaderRenderMgr.blendBoneNum);
		g_ShaderRenderMgr.bone_matrix_changed = false;
	}
}

void CShaderDevice::BindCamera()
{
	if (g_ShaderRenderMgr.cameraChanged)
	{
		D3DXVECTOR3 camera = g_camera.GetEyePt();
		float camPos[3] = { camera.x, camera.y, camera.z };

		pShader_->SetFloatArray(cameraPos_, camPos, 3);
		g_ShaderRenderMgr.cameraChanged = false;
	}
}

void CShaderDevice::BindLight()
{
	if (g_ShaderRenderMgr.dxlight != NULL)
	{
		if (g_ShaderRenderMgr.dxlight->Diffuse.r != bindState_.dxlight.Diffuse.r
			|| g_ShaderRenderMgr.dxlight->Diffuse.g != bindState_.dxlight.Diffuse.g
			|| g_ShaderRenderMgr.dxlight->Diffuse.b != bindState_.dxlight.Diffuse.b
			|| g_ShaderRenderMgr.dxlight->Diffuse.a != bindState_.dxlight.Diffuse.a)
		{
			pShader_->SetFloatArray(directLight_.diffuse, &g_ShaderRenderMgr.dxlight->Diffuse.r, 4);
			bindState_.dxlight.Diffuse = g_ShaderRenderMgr.dxlight->Diffuse;
		}

		if (g_ShaderRenderMgr.dxlight->Ambient.r != bindState_.dxlight.Ambient.r
			|| g_ShaderRenderMgr.dxlight->Ambient.g != bindState_.dxlight.Ambient.g
			|| g_ShaderRenderMgr.dxlight->Ambient.b != bindState_.dxlight.Ambient.b
			|| g_ShaderRenderMgr.dxlight->Ambient.a != bindState_.dxlight.Ambient.a)
		{
			pShader_->SetFloatArray(directLight_.ambient, &g_ShaderRenderMgr.dxlight->Ambient.r, 4);
			bindState_.dxlight.Ambient = g_ShaderRenderMgr.dxlight->Ambient;
		}

		if (g_ShaderRenderMgr.dxlight->Specular.r != bindState_.dxlight.Specular.r
			|| g_ShaderRenderMgr.dxlight->Specular.g != bindState_.dxlight.Specular.g
			|| g_ShaderRenderMgr.dxlight->Specular.b != bindState_.dxlight.Specular.b
			|| g_ShaderRenderMgr.dxlight->Specular.a != bindState_.dxlight.Specular.a)
		{
			pShader_->SetFloatArray(directLight_.specular, &g_ShaderRenderMgr.dxlight->Specular.r, 4);
			bindState_.dxlight.Specular = g_ShaderRenderMgr.dxlight->Specular;
		}

		if (g_ShaderRenderMgr.dxlight->Position.x != bindState_.dxlight.Position.x
			|| g_ShaderRenderMgr.dxlight->Position.y != bindState_.dxlight.Position.y
			|| g_ShaderRenderMgr.dxlight->Position.z != bindState_.dxlight.Position.z)
		{
			pShader_->SetFloatArray(directLight_.pos, &g_ShaderRenderMgr.dxlight->Position.x, 3);
			bindState_.dxlight.Position = g_ShaderRenderMgr.dxlight->Position;
		}
	}
}

void CShaderDevice::BindReflectLevel()
{
	if(g_ENV.ReflectLevel != bindState_.reflectLevel)
	{
		pShader_->SetFloat(clipHeight_, g_ENV.ReflectLevel);
		bindState_.reflectLevel = g_ENV.ReflectLevel;
	}
}

void CShaderDevice::BindStuffType(int bindType)
{
	if (bindState_.stuffType != bindType)
	{
		pShader_->SetInt(stuffType_, bindType);
		bindState_.stuffType = bindType;
	}
}

void CShaderDevice::BindWaterInfo()
{
	if (bindState_.waterWave != g_ShaderRenderMgr.waterWave)
	{
		pShader_->SetFloat(waterWave_, g_ShaderRenderMgr.waterWave);
		bindState_.waterWave = g_ShaderRenderMgr.waterWave;
	}
}

#define nowRS g_ShaderRenderMgr.render_state
#define prevRS g_ShaderRenderMgr.prev_render_state
void CShaderDevice::BindRS_Lighting(bool force)
{
	if (nowRS.lighting != prevRS.lighting || force)
	{
		pShader_->SetInt(lighting_, g_ShaderRenderMgr.render_state.lighting ? 1 : 0);
		prevRS.lighting = nowRS.lighting;
	}
}

void CShaderDevice::BindRS_AlphaTest(bool force)
{
	if (nowRS.alpha_test_enable != prevRS.alpha_test_enable 
		|| nowRS.alpha_test_ref != prevRS.alpha_test_ref
		|| force)
	{
		if (nowRS.alpha_test_enable)
		{
			pShader_->SetFloat(alphaTest_, nowRS.alpha_test_ref / 255.f);
		}
		else
		{
			pShader_->SetFloat(alphaTest_, -1.f);
		}

		prevRS.alpha_test_enable = nowRS.alpha_test_enable;
		prevRS.alpha_test_ref = nowRS.alpha_test_ref;
	}
}

void CShaderDevice::BindRS_TextureFactor(bool force)
{
	if (nowRS.texture_factor != prevRS.texture_factor || force)
	{
		float rgba[4];

		g_ShaderRenderMgr.DirectX_Split_Color(nowRS.texture_factor, rgba);

		pShader_->SetFloatArray(textureFactor_, rgba, 4);

		prevRS.texture_factor = nowRS.texture_factor;
	}
}

void CShaderDevice::BindRS_Fog(bool force)
{
	if (nowRS.fog_enable != prevRS.fog_enable || force)
	{
		pShader_->SetInt(fog_enable_, nowRS.fog_enable ? 1 : 0);

		prevRS.fog_enable = nowRS.fog_enable;
	}
	if (nowRS.fog_enable || force)
	{
		if (nowRS.fog_start != prevRS.fog_start || force)
		{
			pShader_->SetFloat(fog_start_, nowRS.fog_start);

			prevRS.fog_start = nowRS.fog_start;
		}
		if (nowRS.fog_end != prevRS.fog_end || force)
		{
			pShader_->SetFloat(fog_end_, nowRS.fog_end);

			prevRS.fog_end = nowRS.fog_end;
		}
		if (nowRS.fog_Color != prevRS.fog_Color || force)
		{
			float rgba[4];
			g_ShaderRenderMgr.DirectX_Split_Color(nowRS.fog_Color, rgba);
			pShader_->SetFloatArray(fog_color_, rgba, 4);

			prevRS.fog_Color = nowRS.fog_Color;
		}
	}
}

void CShaderDevice::BindRS_VertexBlend(bool force)
{
	if ((nowRS.vertex_blend != 0 && prevRS.vertex_blend == 0) 
		|| (nowRS.vertex_blend == 0 && prevRS.vertex_blend != 0)
		|| force)
	{
		pShader_->SetInt(render_state_vertex_blend_, nowRS.vertex_blend);

		prevRS.vertex_blend = nowRS.vertex_blend;
	}
}

void CShaderDevice::BindRS_Bump(bool force)
{
	if (prevRS.matBumpMat._11 != nowRS.matBumpMat._11 || force)
	{
		pShader_->SetFloat(bumpMat11_, nowRS.matBumpMat._11);
		prevRS.matBumpMat._11 = nowRS.matBumpMat._11;
	}
	if (prevRS.matBumpMat._22 != nowRS.matBumpMat._22 || force)
	{
		pShader_->SetFloat(bumpMat22_, nowRS.matBumpMat._22);
		prevRS.matBumpMat._22 = nowRS.matBumpMat._22;
	}
}

void CShaderDevice::BindRS_TextureStage(bool force)
{
	//최초 초기화시 에는 스테이지 두개다 초기화 해줘야 한다.
	if(force)
		nowRS.texture_stage = GLOBAL_CONST_MAX_TEXTURE;
	else
		nowRS.texture_stage = g_ShaderRenderMgr.GetAvailableTextureStage();

	if (prevRS.texture_stage != nowRS.texture_stage || force)
	{
		pShader_->SetInt(textureStageCnt_, nowRS.texture_stage);

		prevRS.texture_stage = nowRS.texture_stage;
	}

	for (int Stage = 0; Stage < nowRS.texture_stage; ++Stage)
	{
		if (nowRS.texture[Stage].color_op != prevRS.texture[Stage].color_op || force)
		{
			pShader_->SetFloatArray(color_op_[Stage],
				g_ShaderRenderMgr.GetTextureStageStateOp(Stage, D3DTSS_COLOROP, nowRS.texture[Stage].color_op),
				CShaderRenderMgr::MAX_TEXTURE_STAGE_OP);

			prevRS.texture[Stage].color_op = nowRS.texture[Stage].color_op;
		}

		if (nowRS.texture[Stage].color_arg1 != prevRS.texture[Stage].color_arg1 || force)
		{
			pShader_->SetFloatArray(color_arg1_[Stage],
				g_ShaderRenderMgr.GetTextureStageStateArg(Stage, D3DTSS_COLORARG1, nowRS.texture[Stage].color_arg1),
				CShaderRenderMgr::MAX_TEXTURE_STAGE_ARG);

			prevRS.texture[Stage].color_arg1 = nowRS.texture[Stage].color_arg1;
		}

		if (nowRS.texture[Stage].color_arg2 != prevRS.texture[Stage].color_arg2 || force)
		{
			pShader_->SetFloatArray(color_arg2_[Stage],
				g_ShaderRenderMgr.GetTextureStageStateArg(Stage, D3DTSS_COLORARG2, nowRS.texture[Stage].color_arg2),
				CShaderRenderMgr::MAX_TEXTURE_STAGE_ARG);

			prevRS.texture[Stage].color_arg2 = nowRS.texture[Stage].color_arg2;
		}

		if (nowRS.texture[Stage].alpha_op != prevRS.texture[Stage].alpha_op || force)
		{
			pShader_->SetFloatArray(alpha_op_[Stage],
				g_ShaderRenderMgr.GetTextureStageStateOp(Stage, D3DTSS_ALPHAOP, nowRS.texture[Stage].alpha_op),
				CShaderRenderMgr::MAX_TEXTURE_STAGE_OP);

			prevRS.texture[Stage].alpha_op = nowRS.texture[Stage].alpha_op;
		}

		if (nowRS.texture[Stage].alpha_arg1 != prevRS.texture[Stage].alpha_arg1 || force)
		{
			pShader_->SetFloatArray(alpha_arg1_[Stage],
				g_ShaderRenderMgr.GetTextureStageStateArg(Stage, D3DTSS_ALPHAARG1, nowRS.texture[Stage].alpha_arg1),
				CShaderRenderMgr::MAX_TEXTURE_STAGE_ARG);

			prevRS.texture[Stage].alpha_arg1 = nowRS.texture[Stage].alpha_arg1;
		}

		if (nowRS.texture[Stage].alpha_arg2 != prevRS.texture[Stage].alpha_arg2 || force)
		{
			pShader_->SetFloatArray(alpha_arg2_[Stage],
				g_ShaderRenderMgr.GetTextureStageStateArg(Stage, D3DTSS_ALPHAARG2, nowRS.texture[Stage].alpha_arg2),
				CShaderRenderMgr::MAX_TEXTURE_STAGE_ARG);

			prevRS.texture[Stage].alpha_arg2 = nowRS.texture[Stage].alpha_arg2;
		}

		if (nowRS.texture[Stage].texture_coord_index != prevRS.texture[Stage].texture_coord_index || force)
		{
			pShader_->SetFloatArray(texCoord_[Stage],
				g_ShaderRenderMgr.GetTextureCoordinateIndex(nowRS.texture[Stage].texture_coord_index),
				CShaderRenderMgr::MAX_TEXTURE_COORD_INDEX);

			prevRS.texture[Stage].texture_coord_index = nowRS.texture[Stage].texture_coord_index;
		}

		if (nowRS.texture[Stage].texture_coord_flag != prevRS.texture[Stage].texture_coord_flag 
			|| nowRS.texture[Stage].texture_coord_flag != D3DTTFF_DISABLE
			|| force)
		{
			D3DXMATRIX* pMat = NULL;
			switch (nowRS.texture[Stage].texture_coord_flag)
			{
			case D3DTTFF_COUNT2:
			case D3DTTFF_COUNT3:
				pMat = &nowRS.texture[Stage].texture_coord_matrix;
				break;
			case D3DTTFF_DISABLE:
			default:
				pMat = &g_matIdentity;
				break;
			}

			pShader_->SetMatrix(texCoordMat_[Stage], pMat);

			prevRS.texture[Stage].texture_coord_flag = nowRS.texture[Stage].texture_coord_flag;
		}
	}
}

void CShaderDevice::BindStepAble()
{
	if (g_ShaderRenderMgr.isStepAble != bindState_.isStepAble)
	{
		pShader_->SetInt(isStepAble_, g_ShaderRenderMgr.isStepAble ? 1 : 0);
		bindState_.isStepAble = g_ShaderRenderMgr.isStepAble;
	}
}

void CShaderDevice::BindNormalMapFrame()
{
	if (g_ShaderRenderMgr.isNormalMapFrame != bindState_.isNormalMapFrame)
	{
		pShader_->SetInt(isNormalMapFrame_, g_ShaderRenderMgr.isNormalMapFrame ? 1 : 0);
		bindState_.isNormalMapFrame = g_ShaderRenderMgr.isNormalMapFrame;
	}
}

void CShaderDevice::BindChangeLightingCheck()
{
	bool isChecked = g_OptionEX.changeLighting ? true : false;
	if (isChecked != bindState_.isChangeLightingCheck)
	{
		pShader_->SetInt(isChangeLightingCheck_, isChecked ? 1 : 0);
		bindState_.isChangeLightingCheck = isChecked;
	}
}

#endif //C_USE_SHADER_PIPELINE

///////////////////////////////////////////////////////////////////////////////

#ifdef C_USE_SHADER_PIPELINE

CShaderRenderMgr g_ShaderRenderMgr;

CShaderRenderMgr::CShaderRenderMgr() :
	pShaderDevice_(NULL),
	blendBoneNum(0),
	frameMaterial(NULL),
	bone_matrix_changed(false),
	cameraChanged(false),
	isUIMatrix(false)
#ifdef C_POSTEFFECT
	, isHDR(false)
#endif //C_POSTEFFECT
	, shaderEnable_(false)
	, material(NULL)
	, dxlight(NULL)
	, isStepAble(false)
	, isNormalMapFrame(false)

{
	memset(useTexture, false, sizeof(useTexture));
	memset(mat_main_changed, false, sizeof(mat_main_changed));
}

CShaderRenderMgr::~CShaderRenderMgr()
{

}

void CShaderRenderMgr::DeleteDeviceObjects()
{
	if (!CheckShaderEnable())
	{
		return;
	}

	pShaderDevice_->DeleteDeviceObjects();
	shadowRenderer_.DeleteDeviceObjects();
#ifdef C_POSTEFFECT
	postEffectRenderer.DeleteDeviceObjects();
#endif //C_POSTEFFECT
}
void CShaderRenderMgr::InvalidateDeviceObjects()
{
	if (!CheckShaderEnable())
	{
		return;
	}

	shadowRenderer_.InvalidateDeviceObjects();
#ifdef C_POSTEFFECT
	postEffectRenderer.InvalidateDeviceObjects();
#endif //C_POSTEFFECT
	pShaderDevice_->InvalidateDeviceObjects();
}

void CShaderRenderMgr::RestoreDeviceObjects()
{
	if (!CheckShaderEnable())
	{
		return;
	}

	shadowRenderer_.RestoreDeviceObjects();
#ifdef C_POSTEFFECT
	postEffectRenderer.RestoreDeviceObjects();
#endif //C_POSTEFFECT
	pShaderDevice_->RestoreDeviceObjects();
}

////////////////////////////////////////////////////////
//다이렉트 함수 레핑.
////////////////////////////////////////////////////////
void CShaderRenderMgr::SetLight(int idx, D3DLIGHT9* light)
{
	if (!shaderEnable_)
	{
		g_pd3dDevice->SetLight(idx, light);
		return;
	}

	if (light)
	{
		dxlight = light;
		dxlight->Position = D3DXVECTOR3(-50000, 50000, -29000);
	}
}

void CShaderRenderMgr::SetMaterial(D3DMATERIAL9 * pMaterial)
{
	material = pMaterial;
}

float* CShaderRenderMgr::GetTextureStageStateArg(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) const
{
	static float texture_stage_state_arg_none[MAX_TEXTURE_STAGE_ARG] = { 0.f, 0.f, 0.f, 0.f, 0.f };
	static float texture_stage_state_arg_current[MAX_TEXTURE_STAGE_ARG] = { 1.f, 0.f, 0.f, 0.f, 0.f };
	static float texture_stage_state_arg_texture[MAX_TEXTURE_STAGE_ARG] = { 0.f, 1.f, 0.f, 0.f, 0.f };
	static float texture_stage_state_arg_tfactor[MAX_TEXTURE_STAGE_ARG] = { 0.f, 0.f, 1.f, 0.f, 0.f };
	static float texture_stage_state_arg_texture_alpha[MAX_TEXTURE_STAGE_ARG] = { 0.f, 0.f, 0.f, 1.f, 0.f };

	/* 계산식
	* arg0 * current +
	* arg1 * texture +
	* arg2 * tfactor +
	* arg3 * texture alpha_replicate
	*/

	if (Stage == 0 && (Type == D3DTSS_COLORARG1 || Type == D3DTSS_ALPHAARG1))
	{
		switch (Value)
		{
		case D3DTA_TEXTURE:
			return texture_stage_state_arg_texture;
		case D3DTA_TFACTOR:
			return texture_stage_state_arg_tfactor;
		default:
			break;
		}
	}
	else if (Stage == 0 && Type == D3DTSS_COLORARG2)
	{
		switch (Value)
		{
		case D3DTA_CURRENT:
		case D3DTA_DIFFUSE:
			return texture_stage_state_arg_current; // stage 0에서는 current는 diffuse와 동일함
		case D3DTA_TFACTOR:
			return texture_stage_state_arg_tfactor;
		case D3DTA_TEXTURE | D3DTA_ALPHAREPLICATE:
			return texture_stage_state_arg_texture_alpha;
		default:
			break;
		}
	}
	else if (Stage == 0 && Type == D3DTSS_ALPHAARG2)
	{
		switch (Value)
		{
		case D3DTA_CURRENT:
		case D3DTA_DIFFUSE:
			return texture_stage_state_arg_current; // stage 0에서는 current는 diffuse와 동일함
		case D3DTA_TFACTOR:
			return texture_stage_state_arg_tfactor;
		default:
			break;
		}
	}
	else if (Stage == 1 && (Type == D3DTSS_COLORARG1 || Type == D3DTSS_ALPHAARG1))
	{
		switch (Value)
		{
		case D3DTA_CURRENT:
			return texture_stage_state_arg_current;
		case D3DTA_TEXTURE:
			return texture_stage_state_arg_texture;
		default:
			break;
		}
	}
	else if (Stage == 1 && Type == D3DTSS_COLORARG2)
	{
		switch (Value)
		{
		case D3DTA_CURRENT:
			return texture_stage_state_arg_current;
		case D3DTA_TEXTURE:
			return texture_stage_state_arg_texture;
		case D3DTA_TFACTOR:
			return texture_stage_state_arg_tfactor;
		default:
			break;
		}
	}
	else if (Stage == 1 && Type == D3DTSS_ALPHAARG2)
	{
		switch (Value)
		{
		case D3DTA_CURRENT:
			return texture_stage_state_arg_current;
		case D3DTA_TEXTURE:
			return texture_stage_state_arg_texture;
		default:
			break;
		}
	}

	return texture_stage_state_arg_none;
}

float* CShaderRenderMgr::GetTextureStageStateOp(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) const
{
	static float texture_stage_state_op_none[MAX_TEXTURE_STAGE_OP] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	static float texture_stage_state_op_selectarg1[MAX_TEXTURE_STAGE_OP] = { 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	static float texture_stage_state_op_selectarg2[MAX_TEXTURE_STAGE_OP] = { 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	static float texture_stage_state_op_add[MAX_TEXTURE_STAGE_OP] = { 1.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	static float texture_stage_state_op_addsigned[MAX_TEXTURE_STAGE_OP] = { 1.f, 1.f, -0.5f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	static float texture_stage_state_op_addsigned2x[MAX_TEXTURE_STAGE_OP] = { 1.f, 1.f, -0.5f, 2.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	static float texture_stage_state_op_subtract[MAX_TEXTURE_STAGE_OP] = { 1.f, -1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	static float texture_stage_state_op_addsmooth[MAX_TEXTURE_STAGE_OP] = { 1.f, 1.f, 0.f, 1.f, -1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	static float texture_stage_state_op_modulate[MAX_TEXTURE_STAGE_OP] = { 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	static float texture_stage_state_op_modulate2x[MAX_TEXTURE_STAGE_OP] = { 0.f, 0.f, 0.f, 0.f, 2.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	static float texture_stage_state_op_modulate4x[MAX_TEXTURE_STAGE_OP] = { 0.f, 0.f, 0.f, 0.f, 4.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	static float texture_stage_state_op_blend_texture[MAX_TEXTURE_STAGE_OP] = { 0.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f };
	static float texture_stage_state_op_blend_texture_pre[MAX_TEXTURE_STAGE_OP] = { 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f };
	static float texture_stage_state_op_blend_factor[MAX_TEXTURE_STAGE_OP] = { 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	static float texture_stage_state_op_blend_current[MAX_TEXTURE_STAGE_OP] = { 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	static float texture_stage_state_op_modulate_alpha[MAX_TEXTURE_STAGE_OP] = { 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	static float texture_stage_state_op_modulate_inverse_alpha[MAX_TEXTURE_STAGE_OP] = { 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f };
	static float texture_stage_state_op_disable[MAX_TEXTURE_STAGE_OP] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f };
	static float texture_stage_state_op_modulate_color[MAX_TEXTURE_STAGE_OP] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f };
	static float texture_stage_state_op_modulate_inverse_color[MAX_TEXTURE_STAGE_OP] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f };


	/* 계산식
	* Stage 0
	* (
	*      arg(0) * color1 +
	*      arg(1) * color2 +
	*      arg(2)
	*  ) * arg(3) +
	* arg(4) * color1 * color2
	*
	* Stage 1
	* coef1 = arg(0) * 1.0 +
	*      arg(5) * texture.a +
	*      arg(8) * tfactor.a +
	*      arg(6) * current.a
	* coef2 = arg(1) * 1.0 +
	*      arg(10) * (1.0 - texture.a) +
	*      arg(6) * (1.0 - current.a) +
	*      arg(7) * color1.a +
	*      arg(9) * (1.0 - color1.a) +
	*
	* (
	*      coef1 * color1 +
	*      coef2 * color2 +
	*      arg(2)
	*  ) * arg(3) +
	* arg(4) * color1 * color2 +
	*/

	if (Stage == 0)
	{
		switch (Value)
		{
		case D3DTOP_SELECTARG1:
			return texture_stage_state_op_selectarg1;
		case D3DTOP_MODULATE:
			return texture_stage_state_op_modulate;
		case D3DTOP_MODULATE4X:
			return texture_stage_state_op_modulate4x;
		case D3DTOP_ADDSIGNED2X:
			return texture_stage_state_op_addsigned2x;
		default:
			break;
		}
	}
	else if (Stage == 1 && Type == D3DTSS_COLOROP)
	{
		switch (Value)
		{
		case D3DTOP_DISABLE:
			return texture_stage_state_op_none;
		case D3DTOP_SELECTARG1:
			return texture_stage_state_op_selectarg1;
		case D3DTOP_SELECTARG2:
			return texture_stage_state_op_selectarg2;
		case D3DTOP_MODULATE:
			return texture_stage_state_op_modulate;
		case D3DTOP_MODULATE2X:
			return texture_stage_state_op_modulate2x;
		case D3DTOP_MODULATE4X:
			return texture_stage_state_op_modulate4x;
		case D3DTOP_ADD:
			return texture_stage_state_op_add;
		case D3DTOP_ADDSIGNED:
			return texture_stage_state_op_addsigned;
		case D3DTOP_ADDSIGNED2X:
			return texture_stage_state_op_addsigned2x;
		case D3DTOP_SUBTRACT:
			return texture_stage_state_op_subtract;
		case D3DTOP_ADDSMOOTH:
			return texture_stage_state_op_addsmooth;
		case D3DTOP_BLENDDIFFUSEALPHA:
			return texture_stage_state_op_none;
		case D3DTOP_BLENDTEXTUREALPHA:
			return texture_stage_state_op_blend_texture;
		case D3DTOP_BLENDFACTORALPHA:
			return texture_stage_state_op_blend_factor;
		case D3DTOP_BLENDTEXTUREALPHAPM:
			return texture_stage_state_op_blend_texture_pre;
		case D3DTOP_BLENDCURRENTALPHA:
			return texture_stage_state_op_blend_current;
		case D3DTOP_MODULATEALPHA_ADDCOLOR:
			return texture_stage_state_op_modulate_alpha;
		case D3DTOP_MODULATEINVALPHA_ADDCOLOR:
			return texture_stage_state_op_modulate_inverse_alpha;
#if defined(_WIN32) || defined(__MACOSX__)
		case D3DTOP_MODULATECOLOR_ADDALPHA:
			return texture_stage_state_op_modulate_color;
		case D3DTOP_MODULATEINVCOLOR_ADDALPHA:
			return texture_stage_state_op_modulate_inverse_color;
#endif
		default:
			break;
		}
	}
	else if (Stage == 1 && Type == D3DTSS_ALPHAOP)
	{
		switch (Value)
		{
		case D3DTOP_DISABLE:
			/* D3DTSS_COLOROP가 disable 되지 않은 상태에서 D3DTSS_ALPHAOP가 disable 된 경우는
			* 정의되어 있지 않으나 그냥 D3DTA_CURRENT 값이 사용되는 것으로 보인다.
			* 일반적으로 D3DTSS_ALPHAARG2가 D3DTA_CURRENT이기 때문에 D3DTOP_SELECTARG2로
			* 처리한다. 문제가 있는 부분이 있으면 D3DTSS_ALPHAOP를 제대로 수정해야 될 것이다.
			*/
			return texture_stage_state_op_selectarg2;
		case D3DTOP_SELECTARG1:
			return texture_stage_state_op_selectarg1;
		case D3DTOP_MODULATE:
			return texture_stage_state_op_modulate;
		case D3DTOP_MODULATE4X:
			return texture_stage_state_op_modulate4x;
		case D3DTOP_ADDSIGNED2X:
			return texture_stage_state_op_addsigned2x;
		default:
			break;
		}
	}

	return texture_stage_state_op_none;
}


void CShaderRenderMgr::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix) const
{
	switch ((int)State)
	{
	case D3DTS_VIEW:
		*pMatrix = mat_main[MATRIX_VIEW];
		break;
	case D3DTS_PROJECTION:
		*pMatrix = mat_main[MATRIX_PROJECTION];
		break;
	}
}

void CShaderRenderMgr::SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
	const D3DMATRIX* pMat = pMatrix ? pMatrix : &g_matIdentity;

	switch ((int)State)
	{
	case D3DTS_WORLD:
	case D3DTS_BONEMATRIX_BEGIN:
		mat_main[MATRIX_WORLD] = *pMat;
		mat_main_changed[MATRIX_WORLD] = true;
		break;
	case D3DTS_VIEW:
		mat_main[MATRIX_VIEW] = *pMat;
		mat_main_changed[MATRIX_VIEW] = true;
		break;
	case D3DTS_PROJECTION:
		mat_main[MATRIX_PROJECTION] = *pMat;
		mat_main_changed[MATRIX_PROJECTION] = true;
		break;
	case D3DTS_TEXTURE0:
		render_state.texture[0].texture_coord_matrix = *pMat;
		break;
	case D3DTS_TEXTURE1:
		render_state.texture[1].texture_coord_matrix = *pMat;
		break;
	}

	if (D3DTS_BONEMATRIX_BEGIN <= State)
	{
		int i = static_cast<int>(State) - D3DTS_BONEMATRIX_BEGIN;

		if (i >= 0 && i < GLOBAL_CONST_MAX_BONE_MATRIX)
		{
			bone_matrix[blendBoneNum++] = *pMat;
			bone_matrix_changed = true;
		}
	}
}

HRESULT CShaderRenderMgr::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	if (Stage >= CShaderRenderMgr::MAX_TEXTURE_STAGE)
	{
		return D3DERR_INVALIDCALL;
	}

	switch (Type)
	{
	case D3DTSS_COLOROP:
		render_state.texture[Stage].color_op = Value;
		break;
	case D3DTSS_COLORARG1:
		render_state.texture[Stage].color_arg1 = Value;
		break;
	case D3DTSS_COLORARG2:
		render_state.texture[Stage].color_arg2 = Value;
		break;
	case D3DTSS_ALPHAOP:
		render_state.texture[Stage].alpha_op = Value;
		break;
	case D3DTSS_ALPHAARG1:
		render_state.texture[Stage].alpha_arg1 = Value;
		break;
	case D3DTSS_ALPHAARG2:
		render_state.texture[Stage].alpha_arg2 = Value;
		break;
	case D3DTSS_TEXCOORDINDEX:
		render_state.texture[Stage].texture_coord_index = Value;
		break;
	case D3DTSS_TEXTURETRANSFORMFLAGS:
		render_state.texture[Stage].texture_coord_flag = Value;
		break;
	case D3DTSS_BUMPENVMAT00:
		render_state.matBumpMat._11 = *((float*)&Value);
		break;
	case D3DTSS_BUMPENVMAT01:
		render_state.matBumpMat._12 = *((float*)&Value);
		break;
	case D3DTSS_BUMPENVMAT10:
		render_state.matBumpMat._21 = *((float*)&Value);
		break;
	case D3DTSS_BUMPENVMAT11:
		render_state.matBumpMat._22 = *((float*)&Value);
		break;
	case D3DTSS_BUMPENVLSCALE:
	case D3DTSS_BUMPENVLOFFSET:
	default:
		break;
	}

	return D3D_OK;
}

float* CShaderRenderMgr::GetTextureCoordinateIndex(DWORD value) const
{
	static float texture_coord_index_select_0[MAX_TEXTURE_COORD_INDEX] = { 1.f, 0.f, 0.f, 0.f };
	static float texture_coord_index_select_1[MAX_TEXTURE_COORD_INDEX] = { 0.f, 1.f, 0.f, 0.f };
	static float texture_coord_index_view_position[MAX_TEXTURE_COORD_INDEX] = { 0.f, 0.f, 1.f, 0.f };
	static float texture_coord_index_view_reflect[MAX_TEXTURE_COORD_INDEX] = { 0.f, 0.f, 0.f, 1.f };

	switch (value)
	{
	case D3DTSS_TCI_PASSTHRU | 0:
		return texture_coord_index_select_0;
	case D3DTSS_TCI_PASSTHRU | 1:
		return texture_coord_index_select_1;
	case D3DTSS_TCI_CAMERASPACEPOSITION:
		return texture_coord_index_view_position;
	case D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR:
		return texture_coord_index_view_reflect;
	}

	return texture_coord_index_select_0;
}


void CShaderRenderMgr::DirectX_Split_Color(D3DCOLOR Color, float argb[]) const
{
	argb[3] = ((Color >> 24) & 0xFF) / 255.0f;
	argb[0] = ((Color >> 16) & 0xFF) / 255.0f;
	argb[1] = ((Color >> 8) & 0xFF) / 255.0f;
	argb[2] = ((Color >> 0) & 0xFF) / 255.0f;
}

void CShaderRenderMgr::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture)
{
	if (!CheckShaderEnable())
	{
		return;
	}

	if (Stage < CShaderRenderMgr::MAX_TEXTURE_STAGE)
	{
		useTexture[Stage] = (pTexture != NULL);
	}
}

void CShaderRenderMgr::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	if (!CheckShaderEnable())
	{
		return;
	}

	switch (State)
	{
	case D3DRS_VERTEXBLEND:
		render_state.vertex_blend = Value;
		break;
	case D3DRS_TEXTUREFACTOR:
		render_state.texture_factor = Value;
		break;
	case D3DRS_ALPHATESTENABLE:
		render_state.alpha_test_enable = (Value == TRUE);
		break;
	case D3DRS_ALPHAREF:
		render_state.alpha_test_ref = Value;
		break;
	case D3DRS_FOGENABLE:
		render_state.fog_enable = (Value == TRUE);
		break;
	case D3DRS_FOGSTART:
		render_state.fog_start = *((float*)&Value);
		break;
	case D3DRS_FOGEND:
		render_state.fog_end = *((float*)&Value);
		break;
	case D3DRS_FOGCOLOR:
		render_state.fog_Color = Value;
		break;
	case D3DRS_LIGHTING:
		render_state.lighting = (Value == TRUE);
		break;
	}
}

int CShaderRenderMgr::GetAvailableTextureStage() const
{
	if (render_state.texture[1].color_op == D3DTOP_DISABLE ||
		render_state.texture[1].color_op == 0)
	{
		return 1;
	}

	/* 1번 스테이지를 쓰지 않을 때 COLOROP를 DISABLE 하지 않고 텍스처를 NULL로 넣는 식으로
	* 구현되어 있기 때문에 이 부분도 고려하여 최대 스테이지를 구한다.
	*/

	if ((render_state.texture[1].color_arg1 == D3DTA_TEXTURE ||
		render_state.texture[1].color_arg2 == D3DTA_TEXTURE) &&
		useTexture[1] == false)
	{
		return 1;
	}

	return 2;
}

///////////////////////////////////////////////////////

void CShaderRenderMgr::InitShaderObj()
{
	pShaderDevice_ = new CShaderDevice();

	if (S_OK != pShaderDevice_->Init())
	{
		return;
	}
}

void CShaderRenderMgr::Initialize3DEnvironment(UINT width, UINT height)
{
	D3DXMatrixOrthoOffCenterLH(&mat_2d_project, 0.0f, (float)width, (float)height, 0.f, 0.f, 1.f);
}

#ifdef C_POSTEFFECT
void CShaderRenderMgr::ChangeSceneRenderTarget()
{
	postEffectRenderer.ChangeSceneRenderTarget();
}

void CShaderRenderMgr::RenderPostEffect()
{
	postEffectRenderer.Render();
}
#endif //C_POSTEFFECT

void CShaderRenderMgr::RenderDepthTexture() const
{
	if (!CheckShaderEnable())
	{
		return;
	}

	shadowRenderer_.RenderDepthTexture();
	postEffectRenderer.RenderTexture();
}

void CShaderRenderMgr::RenderShadowMap()
{
	if (!g_ShaderRenderMgr.CheckShaderEnable())
	{
		return;
	}

	shadowRenderer_.RenderShadowMap();
}

void CShaderRenderMgr::InitNewWorld(CTerrainMesh* terrain)
{
	if (!g_ShaderRenderMgr.CheckShaderEnable())
	{
		return;
	}

	shadowRenderer_.InitNewWorld(terrain);
}

UINT CShaderRenderMgr::BeginShader(int effectIdx, int technique,  bool push/*true*/)
{
	if (!g_ShaderRenderMgr.CheckShaderEnable())
	{
		return 0;
	}

	UINT numPasses = 0;

	if (!usedShaderStack_.empty())
	{
		pShaderDevice_->GetShader()->EndPass();
		pShaderDevice_->GetShader()->End();
	}

	this->effectIdx = effectIdx;

	pShaderDevice_->ChangeShaderEffect(effectIdx);
	D3DXHANDLE handle = pShaderDevice_->GetTechniqueHandle(effectIdx, technique);

	if (!pShaderDevice_->GetShader())
	{
		return 0;
	}

	pShaderDevice_->GetShader()->SetTechnique(handle);

	if (FAILED(pShaderDevice_->GetShader()->Begin(&numPasses, D3DXFX_DONOTSAVESAMPLERSTATE)))
	{
		return numPasses;
	}

	if (FAILED(pShaderDevice_->GetShader()->BeginPass(0)))
	{
		return numPasses;
	}

	if (push)
	{
		ShaderBlock block;
		block.effectIdx = effectIdx;
		block.techniqueIdx = technique;

		usedShaderStack_.push(block);

		ASSERT(usedShaderStack_.size() < 10);
	}

	return numPasses;
}

void CShaderRenderMgr::EndShader()
{
	if (!g_ShaderRenderMgr.CheckShaderEnable() || !pShaderDevice_->GetShader())
	{
		return;
	}

	ASSERT(usedShaderStack_.size());

	if (usedShaderStack_.size() == 1)
	{
		pShaderDevice_->GetShader()->EndPass();
		pShaderDevice_->GetShader()->End();

		usedShaderStack_.pop();
	}
	else if (usedShaderStack_.size() > 1)
	{
		pShaderDevice_->GetShader()->EndPass();
		pShaderDevice_->GetShader()->End();
		usedShaderStack_.pop();

		ShaderBlock block = usedShaderStack_.top();
		BeginShader(block.effectIdx, block.techniqueIdx, false);
	}
}
#endif // C_USE_SHADER_PIPELINE


////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ShaderShadow.h"
#include "PostEffect.h"

#ifdef C_USE_SHADER_PIPELINE
#define D3DTS_BONEMATRIX_BEGIN 300
#define D3DTS_BONEMATRIX(index) (D3DTRANSFORMSTATETYPE)(index + D3DTS_BONEMATRIX_BEGIN)

class CShaderDevice;
class CEntity;
class CBaseStuff;
class CTerrainManager;
class CTerrainMesh;

struct ShaderBlock
{
	int effectIdx;
	int techniqueIdx;
};

class CShaderRenderMgr
{
public:
	CShaderRenderMgr();
	~CShaderRenderMgr();

	void InitShaderObj();
	void Initialize3DEnvironment(UINT width, UINT height);

	bool IsEnableShader() const { return (pShaderDevice_ && !usedShaderStack_.empty()); }
	CShaderDevice* GetShader() const { return pShaderDevice_; }
	void SetFrameMaterial(Material* material) { frameMaterial = material; }

	UINT BeginShader(int effectIdx, int technique, bool push = true);
	void EndShader();

	float* GetTextureStageStateArg(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) const;
	float* GetTextureStageStateOp(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) const;
	int GetAvailableTextureStage() const;
	float* GetTextureCoordinateIndex(DWORD value) const;

	void DirectX_Split_Color(D3DCOLOR Color, float argb[]) const;

	void DeleteDeviceObjects();
	void InvalidateDeviceObjects();
	void RestoreDeviceObjects();

	void SetLight(int idx, D3DLIGHT9* light);
	void SetMaterial(D3DMATERIAL9 * pMaterial);
	void GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix) const;
	void SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix);
	void SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
	void SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture);
	HRESULT SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);

	void RenderShadowMap();
	void RenderDepthTexture() const;
	void InitNewWorld(CTerrainMesh* terrain);
#ifdef C_POSTEFFECT
	void ChangeSceneRenderTarget();
	void RenderPostEffect();
#endif //C_POSTEFFECT

	void SetShaderEnable(bool enable) { shaderEnable_ = enable; }
	bool CheckShaderEnable() const { return shaderEnable_; }

	CPostEffectRenderer postEffectRenderer;
#ifdef C_FIX_FXSHADOW_FIXEDPIPELINE
	CShadowRenderer GetShadowRenderer() const { return shadowRenderer_; }
#endif //C_FIX_FXSHADOW_FIXEDPIPELINE

	enum
	{
		MAX_TEXTURE_STAGE = GLOBAL_CONST_MAX_TEXTURE,
		MAX_TEXTURE_STAGE_OP = 14,
		MAX_TEXTURE_STAGE_ARG = 5,
		MAX_TEXTURE_COORD_INDEX = 4,
	};

	int blendBoneNum;
	int effectIdx;

	D3DMATERIAL9* material;
	Material* frameMaterial;//오브젝트 매니저에서 설정하는값들
	D3DLIGHT9*	dxlight; //쉐이더 광원 설정.

	D3DXMATRIX mat_2d_project;
	D3DXMATRIX mat_main[MATRIX_END];
	D3DXMATRIX bone_matrix[GLOBAL_CONST_MAX_BONE_MATRIX];

	bool useTexture[MAX_TEXTURE_STAGE];

	bool bone_matrix_changed;
	bool mat_main_changed[MATRIX_END];
	bool cameraChanged;
	bool isUIMatrix;
	bool isStepAble;
	bool isNormalMapFrame;

	RenderState render_state;
	RenderState prev_render_state;

#ifdef C_POSTEFFECT
	bool isHDR;
#endif //C_POSTEFFECT

	//그림자 변수 추가.
	D3DXVECTOR2	shadowPos;
	float shadow_size;
	D3DXVECTOR2	shadowPos_LowQuality;
	float shadow_size_LowQuality;
	float userHeight;
	float waterWave;

private:
	bool shaderEnable_;

	CShaderDevice* pShaderDevice_;
	CShadowRenderer shadowRenderer_;
	std::stack<ShaderBlock> usedShaderStack_;
};

extern CShaderRenderMgr g_ShaderRenderMgr;

#endif //C_USE_SHADER_PIPELINE


//////////////////////////////////////////////////////////////////////////////////////////






