#include "ASELoader.h"
#include "RenderMgr.h"
#include "Device.h"

using namespace std;


//int r = AseFileData.tellg();
//AseFileData.seekg(r, AseFileData.beg);
ASE_Loader g_aseLoader;

typedef char _AseKey[64];

_AseKey Keywords[] =
{
	"*SCENE {"				,	// SCENE
	"*SCENE_FIRSTFRAME"		,	// FRAME_FIRST
	"*SCENE_LASTFRAME"		,	// FRAME_LAST
	"*SCENE_FRAMESPEED"		,	// FRAME_SPEED
	"*SCENE_TICKSPERFRAME"	,	// FRAME_TICK

	"*MATERIAL_LIST {"		,	// MTRL_LST
	"*MATERIAL_COUNT"		,	// MTRL_CNT
	"*MATERIAL "			,	// MTRL_INF

	"*MAP_DIFFUSE {"		,	// MTRL_MAPDIF
	"*BITMAP"				,	// MTRL_TEXDIF

	"*GEOMOBJECT {"			,	// GEOOBJECT

	"*NODE_NAME"			,	// NODE_CUR
	"*NODE_PARENT"			,	// NODE_PRN

	"*MATERIAL_REF"			,	// MTRL_REF

	"*NODE_TM {"			,	// NODE_TM
	"*TM_ROW0"				,	// TM_ROW0
	"*TM_ROW1"				,	// TM_ROW1
	"*TM_ROW2"				,	// TM_ROW2
	"*TM_ROW3"				,	// TM_ROW3
	"*TM_POS"				,	// TM_POS
	"*TM_ROTAXIS"			,	// TM_ROT
	"*TM_ROTANGLE"			,	// TM_RTA
	"*TM_SCALE"				,	// TM_SCL

	"*MESH {"				,	// MESH_INFOS
	"*MESH_NUMVERTEX"		,	// MESH_NUMVTX
	"*MESH_NUMFACES"		,	// MESH_NUMFCE
	"*MESH_VERTEX_LIST {"	,	// MESH_VTXLST
	"*MESH_VERTEX"			,	// MESH_VTXREC
	"*MESH_FACE_LIST {"		,	// MESH_FCELST
	"*MESH_FACE"			,	// MESH_FCEREC

	"*MESH_NUMTVERTEX"		,	// MESH_NUMTVTX
	"*MESH_TVERTLIST {"		,	// MESH_TVTXLST
	"*MESH_TVERT"			,	// MESH_TVTXREC

	"*MESH_NUMTVFACES"		,	// MESH_NUMTFCE
	"*MESH_TFACELIST {"		,	// MESH_TFCELST
	"*MESH_TFACE"			,	// MESH_TFCEREC

	"*MESH_NORMALS {"		,	// MESH_NORMAL
	"*MESH_FACENORMAL"		,	// MESH_FCENRL
	"*MESH_VERTEXNORMAL"    ,	// MESH_VERTEXNORMAL

	"*TM_ANIMATION {"		,	// TM_ANIMATION
	"*CONTROL_ROT_TRACK {"	,	// ROT_TRACK
	"*CONTROL_ROT_SAMPLE"	,	// ROT_SAMPLE
	"*CONTROL_SCALE_TRACK {",	// SCALE_TRACK
	"*CONTROL_SCALE_SAMPLE"	,	// SCALE_SAMPLE
	"*CONTROL_POS_TRACK {"	,	// POS_TRACK
	"*CONTROL_POS_SAMPLE"	,	// POS_SAMPLE

	"*CONTROL_ROT_TCB {"	,	// ROT_TCB
	"*CONTROL_TCB_ROT_KEY"	,	// ROT_TCB_KEY
};

enum KeyID
{
	SCENE,
	SCENE_FIRSTFRAME,		
	SCENE_LASTFRAME,	
	SCENE_FRAMESPEED,	
	SCENE_TICKSPERFRAME,
	MATERIAL_LIST,	
	MATERIAL_COUNT,		
	MATERIAL,	

	MAP_DIFFUSE,	
	BITMAP_,				

	GEOMOBJECT,			

	NODE_NAME,			
	NODE_PARENT,			

	MATERIAL_REF,			

	NODE_TM,		
	TM_ROW0,				
	TM_ROW1,				
	TM_ROW2,				
	TM_ROW3,				
	TM_POS,			
	TM_ROTAXIS,			
	TM_ROTANGLE,			
	TM_SCALE,				

	MESH,			
	MESH_NUMVERTEX,
	MESH_NUMFACES,	
	MESH_VERTEX_LIST,
	MESH_VERTEX,		
	MESH_FACE_LIST,		
	MESH_FACE,	

	MESH_NUMTVERTEX,	
	MESH_TVERTLIST,		
	MESH_TVERT,	

	MESH_NUMTVFACES,
	MESH_TFACELIST,		
	MESH_TFACE,	

	MESH_NORMALS,
	MESH_FACENORMAL,
	MESH_VERTEXNORMAL,

	TM_ANIMATION,	
	CONTROL_ROT_TRACK,
	CONTROL_ROT_SAMPLE,	
	CONTROL_SCALE_TRACK,
	CONTROL_SCALE_SAMPLE,	
	CONTROL_POS_TRACK,	
	CONTROL_POS_SAMPLE,	

	CONTROL_ROT_TCB,
	CONTROL_TCB_ROT_KEY,
};



std::string modelPath = "..//woman//woman.ASE";

string ASE_Loader::parseDoubleQuotationMark()
{
	char s[1000];
	bool isbegin = false;

	int p = 0;
	int i = 0;

	while(1)
	{
		if(readLine_[p] == '"')
		{
			if(!isbegin)
			{
				isbegin = true;
				p++;
				continue;
			}

			break;
		}

		if(isbegin)
			s[i++] =  readLine_[p];
		p++;
	}

	s[i] = '\0';

	return string(s);
}

void ASE_Loader::Initialize()
{
	ifstream AseFileData;
	AseFileData.open(modelPath);

	ASE_MODEL model;

	SkipHeader(AseFileData);	

	string line;

	while(!AseFileData.eof())
	{
		AseFileData.getline(readLine_, MAX_LINE);
		line.assign(readLine_);

		size_t pos = line.find(Keywords[SCENE]);

		if(pos != string::npos)
		{
			Read_SCENE(AseFileData, model, modelPath); continue;
		}

		pos = line.find(Keywords[MATERIAL_LIST]);

		if(pos != string::npos)
		{
			Read_MaterialList(AseFileData, model); continue;
		}

		pos = line.find(Keywords[GEOMOBJECT]); 

		if(pos != string::npos)
		{
			Read_GeomObject(AseFileData, model); continue;
		}
		
	}

	map_Models_.insert(make_pair(0, model));
	
	MakeLink();
	SetVerTex_WorldToLocal();
	MakeVertex();
	CheckFaceMaterialId();
}

void ASE_Loader::MakeLink()
{
	auto iter = map_Models_.begin();

	for( ; iter != map_Models_.end(); ++iter)
	{
		iter->second.MakeLink();
	}
}

void ASE_Loader::SetVerTex_WorldToLocal()
{
	auto iter = map_Models_.begin();

	for( ; iter != map_Models_.end(); ++iter)
	{
		iter->second.SetVerTex_WorldToLocal();
	}
}

void ASE_Loader::CheckFaceMaterialId()
{
	auto iter = map_Models_.begin();

	for( ; iter != map_Models_.end(); ++iter)
	{
		iter->second.CheckFaceMaterialId();
	}
}

void ASE_Loader::MakeVertex()
{
	auto iter = map_Models_.begin();

	for( ; iter != map_Models_.end(); ++iter)
	{
		iter->second.MakeVertex();
	}
}

void ASE_Loader::SkipHeader(ifstream& AseFileData)
{
	AseFileData.getline(readLine_, MAX_LINE); //3DSMAX_ASCIIEXPORT
	AseFileData.getline(readLine_, MAX_LINE); //COMMENT
}

bool ASE_Loader::Read_SCENE(ifstream& AseFileData, ASE_MODEL& model, string& filePath)
{
	model.filePath_ = filePath;
	string line;
	AseFileData.getline(readLine_, MAX_LINE); //SCENE_FILENAME skip

	//
	AseFileData.getline(readLine_, MAX_LINE);

	line.assign(readLine_);

	size_t pos = line.find(Keywords[SCENE_FIRSTFRAME]);

	if( pos == string::npos)
	{
		_ASSERT(0);
		return false;
	}

	sscanf(line.c_str(), "%s %f",readLine_, &model.firstFrame_); 
	//
	AseFileData.getline(readLine_, MAX_LINE);

	line.assign(readLine_);

	pos = line.find(Keywords[SCENE_LASTFRAME]);

	if( pos == string::npos)
	{
		_ASSERT(0);
		return false;
	}

	sscanf(line.c_str(), "%s %f",readLine_, &model.lastFrame_); 
	//
	AseFileData.getline(readLine_, MAX_LINE);

	line.assign(readLine_);

	pos = line.find(Keywords[SCENE_FRAMESPEED]);

	if( pos == string::npos)
	{
		_ASSERT(0);
		return false;
	}

	sscanf(line.c_str(), "%s %f",readLine_, &model.frameSpeed_); 
	//
	AseFileData.getline(readLine_, MAX_LINE);

	line.assign(readLine_);

	pos = line.find(Keywords[SCENE_TICKSPERFRAME]);

	if( pos == string::npos)
	{
		_ASSERT(0);
		return false;
	}

	sscanf(line.c_str(), "%s %f",readLine_, &model.ticksPerFrame_);

	AseFileData.getline(readLine_, MAX_LINE); //SCENE_BACKGROUND_STATIC skip
	AseFileData.getline(readLine_, MAX_LINE); //SCENE_AMBIENT_STATIC skip

	return true;
}


bool ASE_Loader::Read_MaterialList(ifstream& AseFileData, ASE_MODEL& model)
{
	string line;
	int number = 0;

	AseFileData.getline(readLine_, MAX_LINE);

	line.assign(readLine_);

	size_t pos = line.find(Keywords[MATERIAL_COUNT]);

	if( pos == string::npos)
	{
		_ASSERT(0);
		return false;
	}

	sscanf(line.c_str(), "%s %d",readLine_, &model.materialCount_);

	ASE_MATERIAL material;

	while(1)
	{
		AseFileData.getline(readLine_, MAX_LINE);
		line.assign(readLine_);

		pos = line.find("}");

		if( pos != string::npos)
			break;

		pos = line.find(Keywords[MATERIAL]);

		if( pos != string::npos)
		{
			Read_Material(AseFileData, material);

			model.map_Materials_.insert(make_pair(number++, material));
		}		
	}

	return true;
}

bool ASE_Loader::Read_Material(ifstream& AseFileData, ASE_MATERIAL& material)
{
	string line;
	size_t pos;

	while(1)
	{
		AseFileData.getline(readLine_, MAX_LINE);
		line.assign(readLine_);

		pos = line.find("}");

		if( pos != string::npos)
			break;

		pos = line.find(Keywords[MAP_DIFFUSE]);

		if( pos != string::npos)
		{
			Read_Diffuse(AseFileData, material);
		}
	}

	return true;
}

bool ASE_Loader::Read_Diffuse(ifstream& AseFileData, ASE_MATERIAL& material)
{
	string line;
	size_t pos;
	bool is = false;

	while(1)
	{
		AseFileData.getline(readLine_, MAX_LINE);
		line.assign(readLine_);

		pos = line.find("}");

		if( pos != string::npos)
			break;

		pos = line.find(Keywords[BITMAP_]);

		if( pos != string::npos && is == false)
		{
			material.texturePath_ = parseDoubleQuotationMark();
			is = true;
		}
	}

	return true;
}

bool ASE_Loader::Read_GeomObject(ifstream& AseFileData, ASE_MODEL& model)
{
	string line;
	size_t pos;

	GEOM_OBJECT mesh;

	while(1)
	{
		AseFileData.getline(readLine_, MAX_LINE);
		line.assign(readLine_);

		pos = line.find("}");

		if( pos != string::npos)
			break;

		pos = line.find(Keywords[NODE_NAME]);

		if( pos != string::npos)
		{
			mesh.name_ = parseDoubleQuotationMark();
		}

		pos = line.find(Keywords[NODE_PARENT]);

		if( pos != string::npos)
		{
			mesh.parentName_ = parseDoubleQuotationMark();
		}

		pos = line.find(Keywords[MATERIAL_REF]);

		if( pos != string::npos)
		{
			sscanf(line.c_str(), "%s %d",readLine_, &mesh.materialRefId_);
		}

		pos = line.find(Keywords[NODE_TM]);

		if( pos != string::npos)
		{
			Read_Tm(AseFileData, mesh);
		}

		pos = line.find(Keywords[MESH]);

		if( pos != string::npos)
		{
			Read_Mesh(AseFileData, mesh);
		}
	}

	static int number = 0;


	model.map_GeomObjects_.insert(make_pair(number++, mesh));

	return true;
}

bool ASE_Loader::Read_Tm(std::ifstream& AseFileData, GEOM_OBJECT& mesh)
{
	string line;
	size_t pos;

	D3DXMatrixIdentity(&mesh.mat_Tm_);

	while(1)
	{
		AseFileData.getline(readLine_, MAX_LINE);
		line.assign(readLine_);

		pos = line.find("}");

		if( pos != string::npos)
			break;


		//맥스 좌표계가 y랑 z축이 반전되어 있기때문에 그에 맞게 읽어주어야 한다
		//11, 13, 12
		//31, 33, 32
		//21, 23, 22
		//41, 43, 42

		pos = line.find(Keywords[TM_ROW0]);

		if( pos != string::npos)
		{
			sscanf(line.c_str(), "%s %f %f %f",readLine_, &mesh.mat_Tm_._11,&mesh.mat_Tm_._13,&mesh.mat_Tm_._12);
		}

		pos = line.find(Keywords[TM_ROW1]);

		if( pos != string::npos)
		{
			sscanf(line.c_str(), "%s %f %f %f",readLine_, &mesh.mat_Tm_._31,&mesh.mat_Tm_._33,&mesh.mat_Tm_._32);
		}

		pos = line.find(Keywords[TM_ROW2]);

		if( pos != string::npos)
		{
			sscanf(line.c_str(), "%s %f %f %f",readLine_, &mesh.mat_Tm_._21,&mesh.mat_Tm_._23,&mesh.mat_Tm_._22);
		}

		pos = line.find(Keywords[TM_ROW3]);

		if( pos != string::npos)
		{
			sscanf(line.c_str(), "%s %f %f %f",readLine_, &mesh.mat_Tm_._41,&mesh.mat_Tm_._43,&mesh.mat_Tm_._42);
		}
		//

		pos = line.find(Keywords[TM_POS]);

		if( pos != string::npos)
		{
			sscanf(line.c_str(), "%s %f %f %f",readLine_, &mesh.tmPos_.x,&mesh.tmPos_.z,&mesh.tmPos_.y);
		}

		//오일러 회전축, 회전각
		pos = line.find(Keywords[TM_ROTAXIS]);

		if( pos != string::npos)
		{
			sscanf(line.c_str(), "%s %f %f %f",readLine_, &mesh.tmRot_.x,&mesh.tmRot_.z,&mesh.tmRot_.y);
		}

		pos = line.find(Keywords[TM_ROTANGLE]);

		if( pos != string::npos)
		{
			sscanf(line.c_str(), "%s %f",readLine_, &mesh.rotAngle_);

			//오일러 회전을 사원수로
			mesh.tmRotQuaternion_.x = (float)sinf(mesh.rotAngle_ / 2.0f) * mesh.tmRot_.x;
			mesh.tmRotQuaternion_.y = (float)sinf(mesh.rotAngle_ / 2.0f) * mesh.tmRot_.y;
			mesh.tmRotQuaternion_.z = (float)sinf(mesh.rotAngle_ / 2.0f) * mesh.tmRot_.z;
			mesh.tmRotQuaternion_.w = (float)cosf(mesh.rotAngle_ / 2.0f);
		}
	}

	return true;
}

bool ASE_Loader::Read_Mesh(std::ifstream& AseFileData, GEOM_OBJECT& mesh)
{
	string line;
	size_t pos;

	while(1)
	{
		AseFileData.getline(readLine_, MAX_LINE);
		line.assign(readLine_);

		pos = line.find("}");

		if( pos != string::npos)
			break;

		pos = line.find(Keywords[MESH_NUMVERTEX]);

		if( pos != string::npos)
		{
			sscanf(line.c_str(), "%s %d",readLine_, &mesh.vertexCount_);
		}

		pos = line.find(Keywords[MESH_NUMFACES]);

		if( pos != string::npos)
		{
			sscanf(line.c_str(), "%s %d",readLine_, &mesh.triangleCount_);
		}

		pos = line.find(Keywords[MESH_VERTEX_LIST]);

		if( pos != string::npos)
		{
			Read_VertexList(AseFileData, mesh);
		}

		pos = line.find(Keywords[MESH_FACE_LIST]);

		if( pos != string::npos)
		{
			Read_FaceList(AseFileData, mesh);
		}

		pos = line.find(Keywords[MESH_TVERTLIST]);

		if( pos != string::npos)
		{
			Read_TVertexList(AseFileData, mesh);
		}

		pos = line.find(Keywords[MESH_TFACELIST]);

		if( pos != string::npos)
		{
			Read_TFaceList(AseFileData, mesh);
		}

		pos = line.find(Keywords[MESH_NORMALS]);

		if( pos != string::npos)
		{
			Read_Normals(AseFileData, mesh);
		}
	}

	return true;
}

bool ASE_Loader::Read_VertexList(std::ifstream& AseFileData, GEOM_OBJECT& mesh)
{
	string line;
	size_t pos;

	D3DXVECTOR3 vertex;

	while(1)
	{
		AseFileData.getline(readLine_, MAX_LINE);
		line.assign(readLine_);

		pos = line.find("}");

		if( pos != string::npos)
			break;

		pos = line.find(Keywords[MESH_VERTEX]);

		if( pos != string::npos)
		{
			sscanf(line.c_str(), "%s %f %f %f",readLine_, &vertex.x, &vertex.z, &vertex.y);

			mesh.vec_vertexList_.push_back(vertex);
		}
	}

	return true;
}

bool ASE_Loader::Read_FaceList(std::ifstream& AseFileData, GEOM_OBJECT& mesh)
{
	int num;
	char s[128];
	char p[128];
	string line;
	size_t pos;

	MYINDEX index;

	while(1)
	{
		AseFileData.getline(readLine_, MAX_LINE);
		line.assign(readLine_);

		pos = line.find("}");

		if( pos != string::npos)
			break;

		pos = line.find(Keywords[MESH_FACE]);

		if( pos != string::npos)
		{
			sscanf(line.c_str(), "%s %s %s %d %s %d %s %d",
				s,s,s, &index.index[0], s, &index.index[2], s, &index.index[1]);

			pos = line.find("*MESH_SMOOTHING");

			if( pos != string::npos)
			{
				sscanf(&line[pos],"%s %d %s %d", s, &num, s, &index.subMaterialRef_);
			}
			else
			{
				pos = line.find("*MESH_MTLID");

				if( pos != string::npos)
				{
					sscanf(&line[pos],"%s %d", s, &index.subMaterialRef_);
				}
			}

			mesh.vec_faceList_.push_back(index);
		}
	}

	return true;
}

bool ASE_Loader::Read_TVertexList(std::ifstream& AseFileData, GEOM_OBJECT& mesh)
{
	int num, pos;
	string line;
	D3DXVECTOR2 uv;

	while(1)
	{
		AseFileData.getline(readLine_, MAX_LINE);
		line.assign(readLine_);

		pos = line.find("}");

		if( pos != string::npos)
			break;

		pos = line.find(Keywords[MESH_TVERT]);

		if( pos != string::npos)
		{
			sscanf(line.c_str(), "%s %d %f %f",readLine_, &num, &uv.x, &uv.y);

			//텍스처 매핑이 위/아래가 뒤집혀서 나오기때문에
			uv.y = 1.0f - uv.y;

			mesh.vec_tVertexList_.push_back(uv);
		}
	}

	return true;
}

bool ASE_Loader::Read_TFaceList(std::ifstream& AseFileData, GEOM_OBJECT& mesh)
{
	int pos, num;
	string line;
	TFACEINDEX index;

	while(1)
	{
		AseFileData.getline(readLine_, MAX_LINE);
		line.assign(readLine_);

		pos = line.find("}");

		if( pos != string::npos)
			break;

		pos = line.find(Keywords[MESH_TFACE]);

		if( pos != string::npos)
		{
			sscanf(line.c_str(), "%s %d %d %d %d",readLine_, &num, &index._0, &index._2, &index._1);

			mesh.vec_tFaceList_.push_back(index);

			//읽은 텍스처 좌표 인덱스를 참조하여 Face에 텍스처 좌표를 대입
			mesh.vec_faceList_[num].texUV[0].u = mesh.vec_tVertexList_[index._0].x;
			mesh.vec_faceList_[num].texUV[0].v = mesh.vec_tVertexList_[index._0].y;

			mesh.vec_faceList_[num].texUV[2].u = mesh.vec_tVertexList_[index._1].x;
			mesh.vec_faceList_[num].texUV[2].v = mesh.vec_tVertexList_[index._1].y;

			mesh.vec_faceList_[num].texUV[2].u = mesh.vec_tVertexList_[index._2].x;
			mesh.vec_faceList_[num].texUV[2].v = mesh.vec_tVertexList_[index._2].y;
		}
	}

	return true;
}

bool ASE_Loader::Read_Normals(std::ifstream& AseFileData, GEOM_OBJECT& mesh)
{
	int pos, num;
	string line;

	float x,y,z;

	int idx[3] = {0,2,1};
	int t = 0;
	int faceIdx;

	while(1)
	{
		AseFileData.getline(readLine_, MAX_LINE);
		line.assign(readLine_);

		pos = line.find("}");

		if( pos != string::npos)
			break;

		pos = line.find(Keywords[MESH_FACENORMAL]);

		if( pos != string::npos)
		{
			sscanf(line.c_str(), "%s %d %f %f %f",readLine_, &faceIdx, &x, &z, &y);

			mesh.vec_faceList_[faceIdx]._faceNormal.x = x;
			mesh.vec_faceList_[faceIdx]._faceNormal.y = y;
			mesh.vec_faceList_[faceIdx]._faceNormal.z = z;
		}

		pos = line.find(Keywords[MESH_VERTEXNORMAL]);

		if( pos != string::npos)
		{
			sscanf(line.c_str(), "%s %d %f %f %f",readLine_, &num, &x, &z, &y);

			mesh.vec_faceList_[faceIdx]._vertexNormal[idx[t]].x = x;
			mesh.vec_faceList_[faceIdx]._vertexNormal[idx[t]].y = y;
			mesh.vec_faceList_[faceIdx]._vertexNormal[idx[t]].z = z;

			t++;
			if(t > 2)
				t = 0;
		}
	}

	return true;
}


void ASE_MODEL::Update()
{
	auto iter = map_GeomObjects_.begin();

	for(; iter != map_GeomObjects_.end(); ++iter)
	{
		iter->second.Update(this);
	}
}

void ASE_MODEL::Render()
{
	auto iter = map_GeomObjects_.begin();

	for(; iter != map_GeomObjects_.end(); ++iter)
	{
		iter->second.Render(this);
	}
}
void ASE_MODEL::MakeLink()
{
	auto uter = map_GeomObjects_.begin();

	//부모 자식 인덱스로 링크 시켜준다.
	for(; uter != map_GeomObjects_.end(); ++uter)
	{
		if(uter->second.parentName_ != "")
		{
			auto iter = map_GeomObjects_.begin();

			for(; iter != map_GeomObjects_.end(); ++iter)
			{
				if(iter->second.name_ == uter->second.parentName_)
				{
					uter->second.parentIdx_ = iter->first;
					iter->second.childIdx_ = uter->first;
				}
			}
		}
	}
}

void ASE_MODEL::SetVerTex_WorldToLocal()
{
	auto uter = map_GeomObjects_.begin();

	//버텍스가 월드 좌표로 뽑히므로 애니메이션을 위해서 로컬로 바꺼준다.
	D3DXMATRIX mat;
	D3DXVECTOR3 vTemp;

	for(; uter != map_GeomObjects_.end(); ++uter)
	{
		D3DXMatrixInverse(&mat, 0, &(uter->second.mat_Tm_));

		for(int i = 0; i < uter->second.vec_vertexList_.size(); ++i)
		{
			D3DXVec3TransformCoord(&vTemp, &uter->second.vec_vertexList_[i], &mat);
			uter->second.vec_vertexList_[i] = vTemp;
		}
	}
}

void ASE_MODEL::CheckFaceMaterialId()
{
	auto uter = map_GeomObjects_.begin();

	for(; uter != map_GeomObjects_.end(); ++uter)
	{
		for(int i = 0; i < uter->second.vec_faceList_.size(); ++i)
		{
			if(uter->second.vec_faceList_[i].subMaterialRef_ >= map_Materials_.size())
			{
				uter->second.vec_faceList_[i].subMaterialRef_ = 0;
				_ASSERT(0);
			}
		}
	}
}

void ASE_MODEL::MakeVertex()
{
	auto uter = map_GeomObjects_.begin();

	for(; uter != map_GeomObjects_.end(); ++uter)
	{
		uter->second.MakeVertexAndIndexBuffer();
	}
}

void GEOM_OBJECT::MakeVertexAndIndexBuffer()
{
	VOID*		pV;	
	VOID*		pI;

	dwFVF_ = RigidVertex::FVF;
	
	int faceCount = vec_faceList_.size();

	// 정점 버퍼 생성 (페이스마다 정점 3개) 
	//정점마다 uv, 노멀값이 다를 수 있기 때문에 페이스 마다 생성 한다.
	g_Device.pd3dDevice_->CreateVertexBuffer( faceCount * sizeof(RigidVertex), 0, dwFVF_, D3DPOOL_DEFAULT, &pVB_, NULL );
	pVB_->Lock( 0, faceCount * sizeof(RigidVertex), (void**)&pV, 0 );

	int pos = 0;

	for( int i = 0; i < faceCount; ++i)
	{
		for(int p = 0; p < 3; ++p, ++pos)
		{
			//버텍스 포지션
			((RigidVertex*)pV)[pos].p.x = vec_vertexList_[vec_faceList_[i].index[p]].x;
			((RigidVertex*)pV)[pos].p.y = vec_vertexList_[vec_faceList_[i].index[p]].y;
			((RigidVertex*)pV)[pos].p.z = vec_vertexList_[vec_faceList_[i].index[p]].z;

			//노멀
			((RigidVertex*)pV)[pos].n.x = vec_faceList_[i]._vertexNormal[p].x;
			((RigidVertex*)pV)[pos].n.y = vec_faceList_[i]._vertexNormal[p].y;
			((RigidVertex*)pV)[pos].n.z = vec_faceList_[i]._vertexNormal[p].z;

			//UV
			((RigidVertex*)pV)[pos].t.x = vec_faceList_[i].texUV[p].u;
			((RigidVertex*)pV)[pos].t.y = vec_faceList_[i].texUV[p].v;

			//버텍스 버퍼에 들어가면서 변경되는 인덱스
			vec_faceList_[i].vertexBufferIdx[p] = pos;
		}
	}

	pVB_->Unlock();

	g_Device.pd3dDevice_->CreateIndexBuffer( faceCount * sizeof(INDEX), 0, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &pIB_, NULL );
	pIB_->Lock( 0, faceCount * sizeof(INDEX), (void**)&pI, 0 );

	DWORD* pW = (DWORD*)pI;

	for(int i = 0 ; i < faceCount ; i++ )
	{
		((INDEX*)pW)[pos].i[0] = vec_faceList_[i].vertexBufferIdx[0];
		((INDEX*)pW)[pos].i[1] = vec_faceList_[i].vertexBufferIdx[1];
		((INDEX*)pW)[pos].i[2] = vec_faceList_[i].vertexBufferIdx[2];

		pW += 3;
	}

	pIB_->Unlock();

	//D3DCAPS9 caps;
	//m_pDev->GetDeviceCaps( &caps );

	// 인덱스가 32비트 인덱스를 지원하면 32비트 인덱스 버퍼 생성
	//if( caps.MaxVertexIndex > 0x0000ffff )
	//else // 아니라면 16비트 인덱스로 인덱스 버퍼 생성

	//인덱스 버퍼 생성.
}

void GEOM_OBJECT::Update(ASE_MODEL* model)
{

}

void GEOM_OBJECT::Render(ASE_MODEL* model)
{
	auto iter = model->map_Materials_.find(materialRefId_);

	if(iter == model->map_Materials_.end())
		return;

	g_renderMgr.SetIndices(pIB_);
	g_renderMgr.SetStreamSource(0, pVB_, 0, sizeof(RigidVertex) );

	//g_renderMgr.SetTexture(0, iter->second.pTexture_);

	g_renderMgr.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vec_faceList_.size() * 3, 0, vec_faceList_.size());
}
