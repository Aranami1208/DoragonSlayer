// -----------------------------------------------------------------------
// 
// �X�L�����b�V���@�J�X�P�[�h�V���h�E�}�b�s���O�@�f�B�X�v���[�X�����g�}�b�s���O�̃V�F�[�_�[
//                                                              2021.6.11
//  (�t�H�O�̏���������)
//                                             DisplaceSkinShadowMap.hlsli
// -----------------------------------------------------------------------
#define MAX_CASCADE_SHADOW 2
#define CASCADE_SPLIT      30
#define MAX_BONE 255

// �O���[�o��
Texture2D g_Texture: register(t0);         // �e�N�X�`���[�� ���W�X�^�[t(n)
Texture2D g_NormalTexture: register(t1);   // �@���}�b�v
Texture2D g_HeightTexture: register(t2);   // �n�C�g�}�b�v
Texture2D g_SpecularTexture: register(t3);   // �X�y�L�����[�}�b�v
Texture2D g_DepthTexture0 : register(t4);      // �[�x�e�N�X�`���[
Texture2D g_DepthTexture1 : register(t5);      // �[�x�e�N�X�`���[

SamplerState g_samLinear : register(s0);   // �T���v���[�̓��W�X�^�[s(n)
SamplerState g_samBorder : register(s1);  //�T���v���[�i�V���h�E�}�b�v�p�j

// �O���[�o��
// ���[���h����ˉe�܂ł̕ϊ��s��E��(b0)
cbuffer global_0 : register(b0)        // -- 2020.4.5
{
	matrix g_mW;         // ���[���h�s��
	matrix g_mWVP;       // ���[���h����ˉe�܂ł̕ϊ������s��
	matrix g_mWLP[MAX_CASCADE_SHADOW];       // ���[���h�E�h���C�g�r���[�h�E�v���W�F�N�V�����̍����B�J�X�P�[�h�̒i����
	float4 g_LightDir;   // ���C�g�̕����x�N�g��
	float4 g_EyePos;     // �J�����ʒu
	float4 g_Diffuse;    // �f�B�t���[�Y�F
	float4 g_DrawInfo;   // �e����(�g���Ă��Ȃ�)   // -- 2020.12.15
};
// �f�B�X�v���[�X�����g�}�b�s���O�p�̊e��f�[�^��n��  // -- 2020.1.24
cbuffer global_1: register(b1)
{
	float3 g_EyePosInv;     // ���f�����猩�����_�ʒu
	float  g_fMinDistance;  // �|���S�������̍ŏ�����
	float  g_fMaxDistance;  // �|���S�������̍ő召����
	int    g_iMaxDevide;    // �����ő吔
	float2 g_Height;        // �f�B�X�v���[�X�����g�}�b�s���O���̐���グ����
	float4 g_WaveMove;      // �g�̈ړ���(�g�̏������̂�)
	float4 g_Specular;      // ���ʔ���(�g�̏������̂�)
};

// �{�[���s��(b2)
cbuffer cbBones : register(b2)
{
	matrix BoneFramePose[MAX_BONE];		// �w��t���[���ł̃��b�V���ɉe����^����S�Ă̍��̃|�[�Y�s��
};

// �}�e���A���J���[�̃R���X�^���g�o�b�t�@
cbuffer global : register(b3)               // -- 2020.12.15
{
	float4 g_MatDiffuse = float4(0.5, 0.5, 0.5, 1);   // �f�B�t���[�Y�F	
	float4 g_MatSpecular = float4(0, 0, 0, 0);        // �X�y�L�����F
};

//  �R���X�^���g�o�b�t�@ �t�H�O�p        // -- 2019.4.14
cbuffer cbFog:register(b4)
{
	float4 g_vFogInfo = float4(0, 0, 0, 0);      // x:�t�H�O�L��(0:���� 1:�w���t�H�O 2:�O�����h�t�H�O)  y:�t�H�O��  z: �t�H�O���x  w:�t�H�O����
	float4 g_vFogColor = float4(0, 0, 0, 0);     // �t�H�O�̐F
};

// �X�L�j���O��̒��_�E�@��������
struct Skin
{
	float4 Pos4;
	float3 Normal;
	float3 Tangent; // �ڐ�      //-- 2021.6.11
	float3 Binormal; // �]�@��   //-- 2021.6.11
};

// �o�[�e�b�N�X�V�F�[�_�[�̓��̓p�����[�^(���_�t�H�[�}�b�g�Ɠ���)
struct VS_IN
{
	float3 Pos      : POSITION;		// ���_���W
	float3 Normal   : NORMAL;		// �@��
	float2 Tex      : TEXCOORD;		// �e�N�Z��
	uint4  Bones    : BONE_INDEX;	// �{�[���̃C���f�b�N�X
	float4 Weights  : BONE_WEIGHT;	// �E�F�C�g
	float3 Tangent  : TANGENT;      // �ڐ�
	float3 Binormal : BINORMAL;     // �]�@��
};

struct VS_OUTPUT
{
	float3 pos    : POSITION;
	float2 uv     : TEXCOORD0;
	float3 normal : NORMAL;
};

struct HS_CONSTANT_OUTPUT
{
	float factor[3]  : SV_TessFactor;
	float inner_factor : SV_InsideTessFactor;
};

struct HS_OUTPUT
{
	float3 pos    : POSITION;
	float2 uv     : TEXCOORD0;
	float3 normal : NORMAL;    // -- 2018.4.16
};

struct DS_OUTPUT
{
	float4 pos : SV_POSITION;
	//float3 Light : TEXCOORD0;
	float3 EyeVector : TEXCOORD1;
	float4 PosWorld : TEXCOORD2;
	float2 uv:TEXCOORD3;
	float4 LighViewPos[MAX_CASCADE_SHADOW] : TEXCOORD4;
	float3 Normal : NORMAL;    // -- 2020.1.15
};

struct DS_OUTPUT_DEPTH
{
	float4 pos : SV_POSITION;
	float4 Depth : POSITION;
	float2 uv:TEXCOORD0;
	float3 normal : NORMAL;    // -- 2020.1.15
};

//
// ���_���X�L�j���O�i�{�[���ɂ��ό`�j����T�u�֐�    // -- 2021.6.11
// �i�o�[�e�b�N�X�V�F�[�_�[�Ŏg�p�j
//
Skin SkinVert(VS_IN In)
{
	Skin Out;
	Out.Pos4 = float4(0.0f, 0.0f, 0.0f, 0.0f);
	Out.Normal = float3(0.0f, 0.0f, 0.0f);
	Out.Tangent = float3(0.0f, 0.0f, 0.0f);
	Out.Binormal = float3(0.0f, 0.0f, 0.0f);

	float4 pos4 = float4(In.Pos, 1);
	float3 normal = In.Normal;
	float3 tangent = In.Tangent;
	float3 binormal = In.Binormal;


	// �{�[��0
	uint iBone = In.Bones.x;
	float fWeight = In.Weights.x;
	matrix m = BoneFramePose[iBone];
	Out.Pos4 += fWeight * mul(pos4, m);
	Out.Normal += fWeight * mul(normal, (float3x3)m);
	Out.Tangent += fWeight * mul(tangent, (float3x3)m);
	Out.Binormal += fWeight * mul(binormal, (float3x3)m);
	// �{�[��1
	iBone = In.Bones.y;
	fWeight = In.Weights.y;
	m = BoneFramePose[iBone];
	Out.Pos4 += fWeight * mul(pos4, m);
	Out.Normal += fWeight * mul(normal, (float3x3)m);
	Out.Tangent += fWeight * mul(tangent, (float3x3)m);
	Out.Binormal += fWeight * mul(binormal, (float3x3)m);
	// �{�[��2
	iBone = In.Bones.z;
	fWeight = In.Weights.z;
	m = BoneFramePose[iBone];
	Out.Pos4 += fWeight * mul(pos4, m);
	Out.Normal += fWeight * mul(normal, (float3x3)m);
	Out.Tangent += fWeight * mul(tangent, (float3x3)m);
	Out.Binormal += fWeight * mul(binormal, (float3x3)m);
	// �{�[��3
	iBone = In.Bones.w;
	fWeight = In.Weights.w;
	m = BoneFramePose[iBone];
	Out.Pos4 += fWeight * mul(pos4, m);
	Out.Normal += fWeight * mul(normal, (float3x3)m);
	Out.Tangent += fWeight * mul(tangent, (float3x3)m);
	Out.Binormal += fWeight * mul(binormal, (float3x3)m);

	return Out;
}

// 
// �t�H�O�̌v�Z������T�u�֐�                   // -- 2019.4.14
// 
float4 CalcFog(float4 color, float4 Pos, float4 PosWorld)
{
	// x:�t�H�O�L����0�̎��Ɠ����F�̎��ɂ́A�t�H�O���|���Ȃ�
	if (g_vFogInfo.x == 0 || color.a < 0.001f)
	{
		;
	}
	else {
		if (g_vFogInfo.x == 1)    // �t�H�O�̎��
		{
			// �t�H�O����(�w���t�H�O) ----------------------------------
			float d = Pos.z*Pos.w;          // �y�[�x�i���_����̋����j
			float density = g_vFogInfo.z;   // �t�H�O���x
			float e = 2.71828f;             // ���R�ΐ��̒�
			float f = pow(e, -d*density);   // �t�H�O�t�@�N�^�[  0�ɋ߂��قǃt�H�O���Z��
			f *= g_vFogInfo.y;              // �t�H�O�t�@�N�^�[�Ƀt�H�O��(�w��)���|����
			f = saturate(f);
			color = saturate(f*color + (1 - f)*g_vFogColor);  // �t�H�O�̌v�Z
		}
		else {
			// �t�H�O����(�O�����h�t�H�O) ------------------------------
			// �w���t�H�O�t�@�N�^�������߂�
			float d = Pos.z*Pos.w;          // �y�[�x�i���_����̋����j
			float density = g_vFogInfo.z;   // �t�H�O���x
			float e = 2.71828f;             // ���R�ΐ��̒�
			float f = pow(e, -d*density);   // �t�H�O�t�@�N�^�[  0�ɋ߂��قǃt�H�O���Z��

			// �O�����h�t�H�O�t�@�N�^��h�����߂�
			PosWorld /= PosWorld.w;
			float y = PosWorld.y;           // ��΍��W�ł̍���
			float h = g_vFogInfo.w;         // ��̍���
			float fh = saturate(y / h);     // �t�H�O�t�@�N�^�[  [0,1]�͈̔͂ɃN�����v

			// �w���t�H�O�ƃO�����h�t�H�O����������
			f = f + fh;                     // �w���t�H�O�ɃO�����h�t�H�O�t�@�N�^��������
			f *= g_vFogInfo.y;              // �t�H�O�t�@�N�^�[�Ƀt�H�O��(�w��)���|����
			f = saturate(f);
			color = saturate(f*color + (1 - f)*g_vFogColor);  // �t�H�O�̌v�Z
		}
	}

	return color;
}

//
// �X�y�L�����̌v�Z
//
float4 CalcSpecular(float4 Color, float4 texSpecular, float4 PosWorld, float3 Normal, float NMbright)
{
	if (texSpecular.w == 1.0f) // �X�y�L�����}�b�v�����邩
	{
		// �n�[�t�x�N�g�����g�����X�y�L�����̌v�Z
		float3 ViewDir = normalize( g_EyePos.xyz - PosWorld.xyz);  // PosWorld����g_EyePos�֌����������x�N�g���i���K���j
		float3 HalfVec = normalize( g_LightDir.xyz + ViewDir);     // �n�[�t�x�N�g���i�����x�N�g���ƌ����x�N�N�g���̒��ԃx�N�g���j�i���K���j

		float Gloss = 4;   // ����x�W���B�w���l���傫���قǃX�y�L���������������Ȃ�B�����ł�4�Ƃ��Ă���B
		float Spec = 2.5f * pow( saturate(dot(HalfVec, Normal)), Gloss );   // �X�y�L�����̌v�Z�B��ŃX�y�L�����}�b�v�l���|���邽��2.5�{�ɂ��Ă���
		Color.rgb = saturate(Color.rgb + Spec * texSpecular.rgb * NMbright);   // �X�y�L�����ɃX�y�L�����}�b�v�l���|���ăJ���[�ɉ�����B

	}
	return Color;
}

// 
// �e�̌v�Z����
// 
float4 CalcShadow(float4 Color, float4 LighViewPos, uint Cn)
{

	// ���C�g�r���[�ɂ�����ʒu��XY���W���e�N�X�`�����W�ɕϊ�
	float2 shadowMapTexCoord;
	shadowMapTexCoord.x = (1.0f + LighViewPos.x / LighViewPos.w) * 0.5f;
	shadowMapTexCoord.y = (1.0f - LighViewPos.y / LighViewPos.w) * 0.5f;

	// �V���h�E�}�b�v�e�N�X�`����Z�l�𓾂�
	float shadowMap_Z;
	if (Cn == 0)   // �J�X�P�[�h�̗v�f�ԍ��Ő[�x�e�N�X�`����I������
	{
		shadowMap_Z = g_DepthTexture0.Sample(g_samBorder, shadowMapTexCoord).x;
	}
	else {
		shadowMap_Z = g_DepthTexture1.Sample(g_samBorder, shadowMapTexCoord).x;
	}

	// ���C�g���_�ɂ��Z�l�̎Z�o
	float LightLength = LighViewPos.z / LighViewPos.w;

	// �V���h�E�}�b�v��Z�l�Ɣ�r(���C�g�r���[�ł̒������Z���i���C�g�r���[�ł͎Օ���������))
	//if(shadowMap_Z + 0.0000005 < LightLength) // ���C�g�r���[�ł̒������Z���i�V���h�[�A�N�l�΍�Ƃ��Đ[�x�I�t�Z�b�g�ɒl��ǉ�����j
	if(shadowMap_Z + 0.00005 < LightLength) // ���C�g�r���[�ł̒������Z���i�V���h�[�A�N�l�΍�Ƃ��Đ[�x�I�t�Z�b�g�ɒl��ǉ�����j
	//if(shadowMap_Z < LightLength) // ���C�g�r���[�ł̒������Z���i�V���h�[�A�N�l����������j
	{
		Color.rgb /= 2; // �e�̕�����萔�l�ňÂ�����
	}
	return Color;
}


// ----------------------------------------------------------------------------
//
// �o�[�e�b�N�X�V�F�[�_�[
//
// ----------------------------------------------------------------------------
VS_OUTPUT VS( VS_IN In )
{
	VS_OUTPUT Out;

	Skin vSkinned = SkinVert(In);   // �X�L�j���O

	Out.pos   = vSkinned.Pos4.xyz;  // �X�L�j���O��̒��_
	Out.normal= vSkinned.Normal;    // �X�L�j���O��̖@��
	Out.uv    = In.Tex;             // �e�N�X�`�����W�͂��̂܂�

	return Out;
}

// ----------------------------------------------------------------------------
//
// �n���V�F�[�_�[�́A�p�b�`�萔�֐��ƃR���g���[���|�C���g�V�F�[�_�̂Q�̃t�F�[�Y����Ȃ��Ă���
//
// �p�b�`�Ƃ�	�v���~�e�B�u�E�g�|���W�[ IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST)
// �ȂǂŎw�肳�����̂ŁA���̗�ł̓R���g���[���|�C���g���R�������p�b�`�Ƃ������ƂɂȂ�B
// �Ȃ��A�R���g���[���|�C���g�́A�ő�R�Q�܂Ŏw��ł���B
//
// ----------------------------------------------------------------------------
// �n���V�F�[�_�[�̃p�b�`�萔�t�F�[�Y(�p�b�`�萔�֐�)
// 
// ���̊֐��́A�w��p�b�`���ƂɂP��N������܂��B�i�����ł́A�R���g���[���|�C���g�R���ɋN�������j
// ���̎� ip �̓Y���̓R���g���[���|�C���g�i���_�j�̂O�Ԃ���Q�ԂƂȂ�܂�
// �������e�́A���_���璸�_�܂ł̋����ɉ����ĕ����������肵�܂�
// �R���_���ꂼ��̕�������߂�l�ŕԂ��܂�
// 
//  ����  InputPatch<VS_OUTPUT, 3> ip   ���_�i�o�[�e�b�N�X�j�V�F�[�_�[����w��p�b�`�i�����ł͂R��j���̒l���󂯎��
//        uint pid                      �p�b�`�̂h�c
// 
// ----------------------------------------------------------------------------
HS_CONSTANT_OUTPUT HSConstant( InputPatch<VS_OUTPUT, 3> ip, uint pid : SV_PrimitiveID )
{
	HS_CONSTANT_OUTPUT Out;
	float divide = 0;

	float distance = length(g_EyePosInv - ip[0].pos);   // ���_�O�ԂƎ��_�Ƃ̋����𑪂�
	if(distance < g_fMinDistance) distance = g_fMinDistance;  // �ŏ������ƍő勗���̊ԂɂȂ�悤�ɃN�����v
	if(distance > g_fMaxDistance) distance = g_fMaxDistance;

	// x�́A�������ő勗���Ɠ����ꍇ��1.0�ɁA�ŏ������Ɠ����ꍇ��0.0�ɁA���̒��Ԃ̏ꍇ��0.0�`1.0�ɂȂ�B
	float x = (distance - g_fMinDistance) / (g_fMaxDistance - g_fMinDistance);
	// ��L�̌v�Z�ŁAx�͉��߂��t�ƂȂ��Ă��邽�߁A1�̕␔(1-x)���Ƃ�B
	divide = (1-x) * g_iMaxDevide + 1;   // ���������v�Z����

	// ��������factor�ɐݒ肷��
	Out.factor[0] = divide;
	Out.factor[1] = divide;
	Out.factor[2] = divide;

	Out.inner_factor = divide;

	return Out;
}
// ----------------------------------------------------------------------------
// �n���V�F�[�_�[�̃R���g���[���|�C���g�t�F�[�Y(�R���g���[���|�C���g�V�F�[�_)
// �i�����炪�n���V�F�[�_�[�̃V�F�[�_�[���ƂȂ�j
// 
// ���̊֐��́A�R���g���[���|�C���g�i���_�j���ƂɂP��N������܂��B
// 
//  ����
//  domain("tri")                    �h���C���^�C�v�̎w��iqiad:�l�p�h���C�� tri:�O�p�h���C�� isoline:���h���C���j
//  partitioning("integer")          �������͐��������������_�������w��
//  outputtopology("triangle_ccw")    �ŏI�I�ȕ�����̃|���S���ɂ��āitriangle_cw:���v��� triangle_ccw:�����v��� line:���|���S��)
//  outputcontrolpoints(3)           �n���V�F�[�_�[�̃R���g���[�� �|�C���g�t�F�[�Y���R�[�������񐔁i�����ł͂R��j
//  patchconstantfunc("HSConstant")  �Ή�����n���V�F�[�_�[�̃p�b�`�萔�֐���
// 
//  ����  InputPatch<VS_OUTPUT, 3> ip   ���_�i�o�[�e�b�N�X�j�V�F�[�_�[����w��p�b�`�i�����ł͂R��j���̒l���󂯎��
//        uint cpid                     �R���g���[���|�C���g�̔ԍ��i�Y���j
//        uint pid                      �p�b�`�̂h�c
// 
// ----------------------------------------------------------------------------
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("HSConstant")]
HS_OUTPUT HS( InputPatch<VS_OUTPUT, 3> ip, uint cpid : SV_OutputControlPointID, uint pid : SV_PrimitiveID )
{
	HS_OUTPUT Out;
	Out.pos    = ip[cpid].pos;
	Out.uv     = ip[cpid].uv;
	Out.normal = ip[cpid].normal;

	return Out;
}
// ----------------------------------------------------------------------------
// 
// �h���C���V�F�[�_�[
// 
//  �h���C���V�F�[�_�[�́A�e�Z���[�^�[�̕����ɂ���Đ������ꂽ���_���ɋN�����܂�
//  �h���C���V�F�[�_�[�́A������̎��ۂ̒��_���W���v�Z���܂��B 
//  �O�p�`�d�S�ʒu�iDomLoc)�͕������ 0 �` 1 �͈͓̔��̕⊮�W���ł��B
//  �R���g���[���|�C���g�̍��W���番����̒��_���W���v�Z���܂��B 
//  Hight�e�N�X�`���[���g�p���āA���_�̍������v�Z���܂��B
// 
//  ����
//  domain("tri")                           �h���C���^�C�v�̎w��iqiad:�l�p�h���C�� tri:�O�p�h���C�� isoline:���h���C���j
// 
//  ����  HS_CONSTANT_OUTPUT In             �n���V�F�[�_�[�萔�֐��̏o�͒l���󂯎��
//        float3 DomLoc                     ���Y���_�̈ʒu���i�O�p�`�̏d�S�ʒu�j�B�������ꂽ���_�̃p�b�`��̈ʒu�����߂邽�߂Ɏg�p����
//        OutputPatch<HS_OUTPUT, 3> patch   �S�ẴR���g���[���|�C���g�փA�N�Z�X�\
// 
// ----------------------------------------------------------------------------
[domain("tri")]
DS_OUTPUT DS( HS_CONSTANT_OUTPUT In, float3 DomLoc : SV_DomaInLocation, const OutputPatch<HS_OUTPUT, 3> patch )
{
	DS_OUTPUT Out;

	// �R�̃R���g���[���|�C���g�̃e�N�X�`�����W�ɎO�p�`�d�S�ʒu�ix,y,z)���|���āA���ۂ̃e�N�X�`�����W�����߂Ă���B
	// �i���̂Ƃ��A0�Ԗڂ̃e�N�X�`�����W�̏d�S�ʒu��DomLoc.x�ɁA1�Ԗڂ�DomLoc.y�ɁA2�Ԗڂ�DomLoc.z�ɑΉ�����j
	Out.uv = patch[0].uv*DomLoc.x + patch[1].uv*DomLoc.y + patch[2].uv*DomLoc.z;

	// Height�e�N�X�`���[���g�p���āA��قǋ��߂��e�N�X�`�����W�ʒu�̒��_�l�����߂�
	float3 texHeight = g_HeightTexture.SampleLevel(g_samLinear, Out.uv, 0 ).xyz;

	// �R�̃R���g���[���|�C���g�̖@�����W�ɎO�p�`�d�S�ʒu�ix,y,z)���|���āA�@�����W�����߂�     // -- 2020.1.15
	// �i���̂Ƃ��A0�Ԗڂ̖@�����W�̏d�S�ʒu��DomLoc.x�ɁA1�Ԗڂ�DomLoc.y�ɁA2�Ԗڂ�DomLoc.z�ɑΉ�����j
	Out.Normal = normalize( patch[0].normal*DomLoc.x + patch[1].normal*DomLoc.y + patch[2].normal*DomLoc.z);

	// �R�̃R���g���[���|�C���g�̍��W�ɎO�p�`�d�S�ʒu�ix,y,z)���|���āA���_���W�����߂Ă���B
	// �i���̂Ƃ��A0�Ԗڂ̒��_�̏d�S�ʒu��DomLoc.x�ɁA1�Ԗڂ�DomLoc.y�ɁA2�Ԗڂ�DomLoc.z�ɑΉ�����j
	float3 pos = patch[0].pos*DomLoc.x + patch[1].pos*DomLoc.y + patch[2].pos*DomLoc.z;
	if (DomLoc.x != 0 && DomLoc.y != 0 && DomLoc.z != 0)   // �d�S�ʒu�������O�̒��_�ʒu�̂Ƃ��́A�������Ȃ��B�|���S���̐؂�ڂ��􂯂Ȃ�����
	{
		pos = pos + ( Out.Normal * ((texHeight.x + texHeight.y + texHeight.z) / 3 * g_Height.y) );  // ���_�̍�����������(�@��������)
	}
	float4 pos4 = float4(pos, 1);


	Out.pos = mul(pos4, g_mWVP);  // ���[���h�E�r���[�E�v���W�F�N�V�����ϊ�������

	// ���C�g����
	//Out.Light = normalize(g_LightDir.xyz);

	// �@���̃��[���h�ϊ�������                               // -- 2021.6.11
	Out.Normal = normalize(mul(Out.Normal, (float3x3)g_mW));

	// �����x�N�g���@���[���h��ԏ�ł̒��_���王�_�֌������x�N�g��
	Out.PosWorld = mul( pos4, g_mW );
    Out.EyeVector = normalize( g_EyePosInv.xyz - Out.PosWorld.xyz );

	// ���C�g�r���[�ɂ�����ʒu(�ϊ���)
	for (uint i = 0; i < MAX_CASCADE_SHADOW; i++)
	{
		Out.LighViewPos[i] = mul(pos4, g_mWLP[i]);
	}

	return Out;
}
// ----------------------------------------------------------------------------
//
// �s�N�Z���V�F�[�_�[
//
// ----------------------------------------------------------------------------
float4 PS( DS_OUTPUT In ) : SV_Target
{
	float4 Color;      // �ŏI�o�͒l
	uint width, height;
	g_Texture.GetDimensions(width, height);  // �f�B�t���[�Y�e�N�X�`���̃T�C�Y�𓾂�  // -- 2020.12.15

	// �e�N�X�`���J���[
	float4 texCol;
	float4 texSpecular;

	if (width == 0)  // �f�B�t���[�Y�e�N�X�`�����Ȃ��Ƃ�  // -- 2020.12.15
	{
		texCol = g_MatDiffuse;      // -- 2020.12.15
		texSpecular = g_MatSpecular;      // -- 2020.12.15
	}
	else {
		texCol = g_Texture.Sample(g_samLinear, In.uv);  // �f�B�t���[�Y�e�N�X�`��
		texSpecular = g_SpecularTexture.SampleLevel(g_samLinear, In.uv, 0);  // �X�y�L�����e�N�X�`��
	}
	float4 texNM  = g_NormalTexture.SampleLevel(g_samLinear, In.uv, 0);  // �m�[�}���e�N�X�`��

	// �����̕������ݒ肳��Ă��邩�ǂ����̃`�F�b�N
	if (g_LightDir.x == 0 && g_LightDir.y == 0 && g_LightDir.z == 0)
	{
		// ���̕�����0,0,0�̏ꍇ�͌��̌v�Z�����Ȃ��ŕ`��
		Color.rgb = texCol.rgb * g_Diffuse.rgb;
		Color.a = texCol.a - (1 - g_Diffuse.a);
	}
	else {
		// ���̕������ݒ肳��Ă���ꍇ
		if (texNM.w == 1.0f)   // �@���}�b�v�����邩�ǂ����`�F�b�N
		{
			// �@���}�b�v������ꍇ�͖@���}�b�s���O�t���̃��C�e�B���O
			// �@���}�b�s���O�̖��邳�v�Z
			float3 Normal = -(texNM.xyz * 2 - 1);  // �m�[�}���}�b�v�e�N�X�`������-�@���𓾂�
			float  NMbright = max(0, dot(Normal, g_LightDir.xyz)) * 0.9f + 0.5f;  // �m�[�}���}�b�v�̌��v�Z

			// �����̕����v�Z
			float halflamb = max(0, dot(normalize(In.Normal), g_LightDir.xyz));  // �����o�[�g�̗]����
			halflamb = halflamb * 0.7f + 0.7f;   // �n�[�t�����o�[�g�B������Ɩ���߂�
			Color.rgb = texCol.rgb * NMbright * halflamb * g_Diffuse.rgb;  // �e�N�X�`���F�Ƀm�[�}���}�b�v�ƃn�[�t�����o�[�g�A�g�U���ːF���d�˂�

			// �X�y�L�����̌v�Z
			Color.rgb = CalcSpecular(Color, texSpecular, In.PosWorld, In.Normal, NMbright).rgb;

			// �e�̌v�Z����
			if (g_DrawInfo.y == 2)
			{
				// �J��������̋����ɂ���āA�g�p����J�X�P�[�h�ԍ������肷��
				uint Cn;
				if (In.pos.w < CASCADE_SPLIT)
				{
					Cn = 0;
				}
				else {
					Cn = MAX_CASCADE_SHADOW - 1;
				}
				Color = CalcShadow(Color, In.LighViewPos[Cn], Cn);
			}

			// �����F�̏���
			Color.a = texCol.a - (1 - g_Diffuse.a);   // �����x�̓e�N�X�`�������x�Ɋg�U���ːF�����x����������
		}
		else {
			// �@���}�b�v���Z�b�g����Ă��Ȃ��ꍇ�́A�n�[�t�����o�[�g�ŕ`��
			float halflamb = max(0, dot(normalize(In.Normal), g_LightDir.xyz));  // �}�C�i�X��0�ɕ␳
			//halflamb = halflamb * 0.5f + 0.8f;	// ������Ɩ���߂�
			halflamb = halflamb * 0.7f + 0.6f;	// ������Ɩ���߂�      // -- 2021.6.11
			Color.rgb = texCol.rgb * halflamb * g_Diffuse.rgb;

			// �X�y�L�����̌v�Z
			Color.rgb = CalcSpecular(Color, texSpecular, In.PosWorld, In.Normal, 1).rgb;

			// �e�̌v�Z����
			if (g_DrawInfo.y == 2)
			{
				// �J��������̋����ɂ���āA�g�p����J�X�P�[�h�ԍ������肷��
				uint Cn;
				if (In.pos.w < CASCADE_SPLIT)
				{
					Cn = 0;
				}
				else {
					Cn = MAX_CASCADE_SHADOW - 1;
				}
				Color = CalcShadow(Color, In.LighViewPos[Cn], Cn);
			}

			// �����F�̏���
			Color.a = texCol.a - (1 - g_Diffuse.a);
		}
	}
	// �t�H�O�̌v�Z
	Color = CalcFog(Color, In.pos, In.PosWorld);
	return saturate(Color);  // �ŏI�o�͒l��0�`�P�͈̔͂ɐ�������
}

// ----------------------------------------------------------------------------
// 
// �h���C���V�F�[�_�[  �[�x�e�N�X�`���[�`��p
// 
//  �h���C���V�F�[�_�[�́A�e�Z���[�^�[�̕����ɂ���Đ������ꂽ���_���ɋN�����܂�
//  �h���C���V�F�[�_�[�́A������̎��ۂ̒��_���W���v�Z���܂��B 
//  uv���W�͕������ 0 �` 1 �͈͓̔��̕⊮�W���ł��B
//  �R���g���[���|�C���g�̍��W���番����̒��_���W���v�Z���܂��B 
//  Hight�e�N�X�`���[���g�p���āA���_�̍������v�Z���܂��B
// 
//  ����
//  domain("tri")                           �h���C���^�C�v�̎w��iqiad:�l�p�h���C�� tri:�O�p�h���C�� isoline:���h���C���j
// 
//  ����  HS_CONSTANT_OUTPUT In             �n���V�F�[�_�[�萔�֐��̏o�͒l���󂯎��
//        float3 DomLoc                         ���Y���_�̈ʒu���i���΍��W�j
//        OutputPatch<HS_OUTPUT, 3> patch   �S�ẴR���g���[���|�C���g�փA�N�Z�X�\
// 
// ----------------------------------------------------------------------------
[domain("tri")]
DS_OUTPUT_DEPTH DS_Depth(HS_CONSTANT_OUTPUT In, float3 DomLoc : SV_DomaInLocation, const OutputPatch<HS_OUTPUT, 3> patch)
{
	DS_OUTPUT_DEPTH Out;

	// �R�̃R���g���[���|�C���g�̃e�N�X�`�����W�ɎO�p�`�d�S�ʒu�ix,y,z)���|���āA���ۂ̃e�N�X�`�����W�����߂Ă���B
	// �i���̂Ƃ��A0�Ԗڂ̃e�N�X�`�����W�̏d�S�ʒu��DomLoc.x�ɁA1�Ԗڂ�DomLoc.y�ɁA2�Ԗڂ�DomLoc.z�ɑΉ�����j
	Out.uv = patch[0].uv*DomLoc.x + patch[1].uv*DomLoc.y + patch[2].uv*DomLoc.z;

	// Height�e�N�X�`���[���g�p���āA��قǋ��߂��e�N�X�`�����W�ʒu�̒��_�̍��������߂�
	float4 texHeight = g_HeightTexture.SampleLevel(g_samLinear, Out.uv, 0);

	// �R�̃R���g���[���|�C���g�̖@�����W�ɎO�p�`�d�S�ʒu�ix,y,z)���|���āA�@�����W�����߂�     // -- 2020.1.15
	// �i���̂Ƃ��A0�Ԗڂ̖@�����W�̏d�S�ʒu��DomLoc.x�ɁA1�Ԗڂ�DomLoc.y�ɁA2�Ԗڂ�DomLoc.z�ɑΉ�����j
	Out.normal = normalize(patch[0].normal*DomLoc.x + patch[1].normal*DomLoc.y + patch[2].normal*DomLoc.z);

	// �R�̃R���g���[���|�C���g�̍��W�ɎO�p�`�d�S�ʒu�ix,y,z)���|���āA���_���W�����߂Ă���B
	// �i���̂Ƃ��A0�Ԗڂ̒��_�̏d�S�ʒu��DomLoc.x�ɁA1�Ԗڂ�DomLoc.y�ɁA2�Ԗڂ�DomLoc.z�ɑΉ�����j
	float3 pos = patch[0].pos*DomLoc.x + patch[1].pos*DomLoc.y + patch[2].pos*DomLoc.z;
	if (DomLoc.x != 0 && DomLoc.y != 0 && DomLoc.z != 0)   // �d�S�ʒu�����̒��_�ʒu�̂Ƃ��́A�������Ȃ��B�|���S���̐؂�ڂ��􂯂Ȃ�����
	{
		//pos = pos + (((patch[0].normal + patch[1].normal + patch[2].normal) / 3) * ((texHeight.x + texHeight.y + texHeight.z) / 3 * g_Height.y));  // ���_�̍�����������(�@��������)    // -- 2018.4.16
		pos = pos + (Out.normal * ((texHeight.x + texHeight.y + texHeight.z) / 3 * g_Height.y));  // ���_�̍�����������(�@��������)
	}

	float4 Pos4 = float4(pos, 1);

	Out.pos = mul(Pos4, g_mWVP);  // ���[���h�E���C�g�r���[�E�v���W�F�N�V�����ϊ�������  !!!!!!!
	Out.Depth = Out.pos;

	return Out;
}


// ----------------------------------------------------------------------------
// �s�N�Z���V�F�[�_�[  �[�x�e�N�X�`���[�`��p
// ----------------------------------------------------------------------------
float4 PS_Depth(DS_OUTPUT_DEPTH In) : SV_Target
{
	float4 Color = float4(0,0,0,0);
	if (g_Texture.Sample(g_samLinear, In.uv).w > 0)  // �����F�łȂ��Ƃ���
	{
		Color = In.Depth.z / In.Depth.w;
	}
	return Color;

}

