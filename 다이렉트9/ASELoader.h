#include "ConstHeader.h"

class ANIAMATION
{
	std::vector<D3DXVECTOR3>		pos_;
	std::vector<D3DXVECTOR3>		rot_;
	std::vector<D3DXVECTOR3>		scl_;
};

class ASE_MODEL;

class GEOM_OBJECT
{
public:
	int parentIdx_;
	int childIdx_;
public:
	std::string name_;
	std::string parentName_;

	//ĳ���� ��ü�� ����� ������ �޽��� ���ÿ��� ĳ����(����)�� �̵������ִ� ���
	D3DXMATRIXA16 mat_Tm_; 

	D3DXVECTOR3 tmPos_;

	D3DXQUATERNION tmRotQuaternion_;
	D3DXVECTOR3 tmRot_;
	float rotAngle_;

	D3DXMATRIXA16 mat_World_;		/// world TM ���(�Һ�)
	D3DXMATRIXA16 mat_Local_;		/// local TM ���(�Һ�)

	std::vector<D3DXVECTOR2> vec_tVertexList_; //uv
	std::vector<D3DXVECTOR3> vec_vertexList_;

	//ase������ ���̽� ���� �ؽ�ó�� �ٸ��� ���� �� �ֱ� ������ ���̽��� uv�� ���µ�.
	std::vector<TFACEINDEX> vec_tFaceList_; //vec_tVertexList_ �� �ε����Ѵ�.

	//��������� �̳༮�� �޽��� �׸��� ���� ��� ������ �� ��� �ִ�.
	std::vector<MYINDEX> vec_faceList_; //���⿡ ���� tFaceList�� �̿��ؼ� uv�� ���Ե� ����.


	LPDIRECT3DVERTEXBUFFER9	pVB_;
	LPDIRECT3DINDEXBUFFER9	pIB_;

	DWORD dwFVF_;		/// ������ fvf��

	int vertexCount_;
	int triangleCount_;
	int parentId_;
	int materialRefId_;

	ANIAMATION* ani_;

public:
	GEOM_OBJECT()
	{
		name_ = "";
		parentName_ = "";
	}

	void MakeVertexAndIndexBuffer();
	void Update(ASE_MODEL* model);
	void Render(ASE_MODEL* model);
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
	float firstFrame_;					//�ִ� ���� ������
	float lastFrame_;					//�ִ� ������ ������,�ִ� ������ �⺻ 100
	float frameSpeed_;					//�ʴ� �����Ӽ�
	float ticksPerFrame_;				//1�������� �� tick���� �̷�������� ����.

	std::string filePath_;

	int materialCount_;
	int vertexCount_;

public:
	std::map<int,ASE_MATERIAL> map_Materials_;
	std::map<int,GEOM_OBJECT> map_GeomObjects_;

public:
	void MakeLink();
	void SetVerTex_WorldToLocal();
	void CheckFaceMaterialId();
	void MakeVertex();
public:
	void Update();
	void Render();
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

	//������
	bool Read_SCENE(std::ifstream& AseFileData, ASE_MODEL& model, std::string& filePath);
	
	//���׸��� ����
	bool Read_MaterialList(std::ifstream& AseFileData, ASE_MODEL& model);
	bool Read_Material(std::ifstream& AseFileData, ASE_MATERIAL& material);
	bool Read_Diffuse(std::ifstream& AseFileData, ASE_MATERIAL& material);

	//������Ʈ�� ����
	bool Read_GeomObject(std::ifstream& AseFileData, ASE_MODEL& model);
	bool Read_Tm(std::ifstream& AseFileData, GEOM_OBJECT& mesh);
	bool Read_Mesh(std::ifstream& AseFileData, GEOM_OBJECT& mesh);
	bool Read_VertexList(std::ifstream& AseFileData, GEOM_OBJECT& mesh);
	bool Read_FaceList(std::ifstream& AseFileData, GEOM_OBJECT& mesh);
	bool Read_TVertexList(std::ifstream& AseFileData, GEOM_OBJECT& mesh); //uv��ǥ
	bool Read_TFaceList(std::ifstream& AseFileData, GEOM_OBJECT& mesh);
	bool Read_Normals(std::ifstream& AseFileData, GEOM_OBJECT& mesh);

public:
	void MakeLink();
	void SetVerTex_WorldToLocal();
	void CheckFaceMaterialId();
	void MakeVertex();
};

extern ASE_Loader g_aseLoader;