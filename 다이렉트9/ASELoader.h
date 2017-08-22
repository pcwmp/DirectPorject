#include "ConstHeader.h"

class ANIAMATION
{
	std::vector<D3DXVECTOR3>		pos_;
	std::vector<D3DXVECTOR3>		rot_;
	std::vector<D3DXVECTOR3>		scl_;
};

class GEOM_OBJECT
{
public:
	std::string name_;
	std::string parentName_;

	//캐릭터 전체를 월드로 봤을때 메쉬를 로컬에서 캐릭터(월드)로 이동시켜주는 행렬
	D3DXMATRIXA16 mat_Tm_; 

	D3DXVECTOR3 tmPos_;

	D3DXQUATERNION tmRotQuaternion_;
	D3DXVECTOR3 tmRot_;
	float rotAngle_;

	D3DXMATRIXA16 mat_World_;		/// world TM 행렬(불변)
	D3DXMATRIXA16 mat_Local_;		/// local TM 행렬(불변)

	std::string nodeName_; //본이름

	LPDIRECT3DVERTEXBUFFER9	m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;

	int vertexCount_;
	int triangleCount_;
	int parentId_;
	int materialRefId_;

	ANIAMATION* ani_;
};

class ASE_MATERIAL
{
public:
	D3DXVECTOR3 ambiant_;
	D3DXVECTOR3 diffuse_;
	D3DXVECTOR3 specualr_;

	float shine_;
	float shineStrength_;
	float transParency_;

	std::string texturePath_;
	LPDIRECT3DTEXTURE9 pTexture_;
};

class ASE_MODEL
{
public:
	float firstFrame_;					//애니 시작 프레임
	float lastFrame_;					//애니 마지막 프레임,애니 없으면 기본 100
	float frameSpeed_;					//초당 프레임수
	float ticksPerFrame_;				//1프레임이 몇 tick으로 이루어졌는지 저장.

	std::string filePath_;

	int materialCount_;
	int vertexCount_;

public:
	std::map<int,ASE_MATERIAL> map_Materials_;
	std::map<int,GEOM_OBJECT> map_GeomObjects_;
};

const int MAX_LINE = 1000;

class ASE_Loader
{
public:
	std::map<int, ASE_MODEL> map_Models_;
private:
	char readLine_[MAX_LINE];
public:
	void Initialize();
	std::string parseDoubleQuotationMark();
	void SkipHeader(std::ifstream& AseFileData);

	//씬정보
	bool Read_SCENE(std::ifstream& AseFileData, ASE_MODEL& model, std::string& filePath);
	
	//마테리얼 정보
	bool Read_MaterialList(std::ifstream& AseFileData, ASE_MODEL& model);
	bool Read_Material(std::ifstream& AseFileData, ASE_MATERIAL& material);
	bool Read_Diffuse(std::ifstream& AseFileData, ASE_MATERIAL& material);

	//지오메트리 정보
	bool Read_GeomObject(std::ifstream& AseFileData, ASE_MODEL& model);
	bool Read_Tm(std::ifstream& AseFileData, GEOM_OBJECT& mesh);

};