// -----------------------------------------------------------------------
// 
// �e�����X�^�e�B�b�N���b�V���@�J�X�P�[�h�V���h�E�}�b�s���O�̃V�F�[�_�[
//                                                              2021.6.11
// �i�t�H�O�̏���������j
//  (�����}�b�s���O)       2021.5.27
//                                                FbxStaticShadowMap.hlsli
// -----------------------------------------------------------------------
#define MAX_CASCADE_SHADOW 2
#define CASCADE_SPLIT      30

//  �O���[�o���ϐ�
Texture2D g_Texture : register(t0);             //�e�N�X�`���[
Texture2D g_NormalTexture : register(t1);       // Nomal�e�N�X�`���[
Texture2D g_HeightTexture: register(t2);   // �n�C�g�}�b�v                   // -- 2021.5.27
Texture2D g_SpecularTexture : register(t3);     // �X�y�L�����[�}�b�v
Texture2D g_DepthTexture0 : register(t4);       // �[�x�e�N�X�`���[[0]
Texture2D g_DepthTexture1 : register(t5);       // �[�x�e�N�X�`���[[1]


SamplerState g_samLinear : register(s0);  //�T���v���[
SamplerState g_samBorder : register(s1);  //�T���v���[�i�V���h�E�}�b�v�p�j
SamplerComparisonState g_samComp   : register(s2);  //�T���v���[�i�V���h�E�}�b�v�G�b�W�ڂ����p�j

//  �R���X�^���g�o�b�t�@ (b0)        // -- 2020.4.5
cbuffer global_0:register(b0)
{
	matrix g_mW;          // ���[���h�s��
	matrix g_mWVP;        // ���[���h����ˉe�܂ł̕ϊ������s��
	matrix g_mWLP[MAX_CASCADE_SHADOW];        // ���[���h�E�h���C�g�r���[�h�E�v���W�F�N�V�����̍����B�J�X�P�[�h�̒i����
	float4 g_LightDir;    // ���C�g�̕����x�N�g��
	float4 g_EyePos;      // �J�����ʒu
	float4 g_Diffuse;     // �f�B�t���[�Y�F
	float4 g_DrawInfo;     // �e����@y:�e�L�薳��(2:�e�L��)
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

//�o�[�e�b�N�X�V�F�[�_�[�o�͍\����
// (�s�N�Z���V�F�[�_�[�̓��͂ƂȂ�)
struct VS_OUTPUT_DEPTH  // �[�x�e�N�X�`���[�����_�[�p
{	
	float4 Pos : SV_POSITION;
	float4 Depth :POSITION;
	float2 Tex : TEXCOORD0;
};

//�o�[�e�b�N�X�V�F�[�_�[�o�͍\����
// (�s�N�Z���V�F�[�_�[�̓��͂ƂȂ�)
struct VS_OUTPUT  // �ʏ탌���_�[�p
{	
	float4 Pos : SV_POSITION;
	float3 Normal : TEXCOORD1;
	float3 EyeVector : TEXCOORD2;
	float4 PosWorld : TEXCOORD3;
	float2 Tex : TEXCOORD4;
	float4 LighViewPos[MAX_CASCADE_SHADOW] : TEXCOORD6;
	float4 lightTangentSpace : TEXCOORD10; // �ڋ�Ԃɕϊ����ꂽ���C�g�x�N�g��
	float4 EyePosTangentSpace : TEXCOORD11; // �ڋ�Ԃɕϊ����ꂽ�����x�N�g��   // -- 2021.5.27
};

// �ڋ�ԍs��̋t�s����Z�o
float4x4 InverseTangentMatrix(float3 tangent, float3 binormal, float3 normal)
{
	tangent = normalize(mul(tangent, (float3x3)g_mW));     // �ڐ������[���h�ϊ�����    // -- 2021.6.11
	binormal = normalize(mul(binormal, (float3x3)g_mW));   // �]�@�������[���h�ϊ�����
	normal = normalize(mul(normal, (float3x3)g_mW));       // �@�������[���h�ϊ�����

	float4x4 mat = { float4(tangent, 0.0f),
		float4(binormal, 0.0f),
		float4(normal, 0.0f),
		{ 0.0f, 0.0f, 0.0f, 1.0f } };

	return transpose(mat); // �]�u�s��ɂ���
}

// �t�H�O�̌v�Z������T�u�֐�                   // -- 2019.4.14
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
	float Bias = 0.00005f;  // �V���h�E�A�N�l�΍�̃o�C�A�X�l

	// ���C�g�r���[�ɂ�����ʒu��XY���W���e�N�X�`�����W�ɕϊ�
	float2 shadowMapTexCoord;
	shadowMapTexCoord.x = (1.0f + LighViewPos.x / LighViewPos.w) * 0.5f;
	shadowMapTexCoord.y = (1.0f - LighViewPos.y / LighViewPos.w) * 0.5f;

	// ���C�g���_�ɂ��Z�l�̎Z�o
	float LightLength = LighViewPos.z / LighViewPos.w;

	// �V���h�E�}�b�v�e�N�X�`����Z�l�𓾂�
	float shadowMap_Z;
	// �J�X�P�[�h�ԍ��ɂ���ă\�t�g�G�b�W��؂�ւ���
	if (Cn == 0)   // �J�X�P�[�h�̗v�f�ԍ��Ő[�x�e�N�X�`����I������
	{
		// �[�x�e�N�X�`��[0] �ߎ��_�̏ꍇ�̓G�b�W���\�t�g�ɂ���i�ڂ����j
		// �i�V���h�[�A�N�l�΍�Ƃ��Đ[�x�I�t�Z�b�g�Ƀo�C�A�X�l��ǉ�����j
		shadowMap_Z = g_DepthTexture0.SampleCmp(g_samComp, shadowMapTexCoord, LightLength - Bias).x;
		Color.rgb *= lerp(0.5f, 1.0f, shadowMap_Z);
	}
	else {
		// �[�x�e�N�X�`��[1] �����_�̏ꍇ�͏]���ʂ�G�b�W���V���[�v�ɂ���B�����Ȃ̂Ŗڗ����Ȃ�
		// ���R�́A�G�b�W�ڂ����v�Z�ŃV���h�E�A�N�l���������邽��
		shadowMap_Z = g_DepthTexture1.Sample(g_samBorder, shadowMapTexCoord).x;
		// �V���h�E�}�b�v��Z�l�Ɣ�r(���C�g�r���[�ł̒������Z���i���C�g�r���[�ł͎Օ���������))
		if (shadowMap_Z + Bias < LightLength) // ���C�g�r���[�ł̒������Z���i�V���h�[�A�N�l�΍�Ƃ��Đ[�x�I�t�Z�b�g�Ƀo�C�A�X�l��ǉ�����j
		{
			Color.rgb /= 2; // �e�̕�����萔�l�ňÂ�����
		}
	}

	return Color;
}

// ---------------------------------------------------------
// 
// �V���h�E�}�b�v�e�N�X�`���ւ̕`�揈��
// 
// ---------------------------------------------------------

//
// �[�x�e�N�X�`���[�p�o�[�e�b�N�X�V�F�[�_�[
//
VS_OUTPUT_DEPTH VS_Depth(float3 Pos3 : POSITION, float3 Norm : NORMAL, float2 Tex : TEXCOORD, float3 Tangent : TANGENT, float3 Binormal : BINORMAL)
{
	VS_OUTPUT_DEPTH output = (VS_OUTPUT_DEPTH)0;
	float4 Pos = float4(Pos3, 1);
	output.Pos = mul(Pos,g_mWVP);  // ���_�����[���h�E���C�g�r���[�E���C�g�v���W�F�N�V�����ϊ�����
	output.Depth = output.Pos;
	output.Tex = Tex;

	return output;
}
//
// �[�x�e�N�X�`���[�p�s�N�Z���V�F�[�_�[
//
float4 PS_Depth( VS_OUTPUT_DEPTH input ) : SV_Target
{
	float4 Color;

	uint width, height;
	g_Texture.GetDimensions(width, height);  // �f�B�t���[�Y�e�N�X�`���̃T�C�Y�𓾂�  // -- 2020.12.15

	float4 texColor;
	if (width == 0)  // �f�B�t���[�Y�e�N�X�`�����Ȃ��Ƃ�  // -- 2020.12.15
	{
		texColor = g_MatDiffuse;      // -- 2020.12.15
	}
	else {
		texColor = g_Texture.Sample(g_samLinear, input.Tex);
	}

	// �����F�̏���                            // -- 2020.9.13
	float Alpha = texColor.a - (1 - g_Diffuse.a);
	if (Alpha <= 0.0f)  discard;  // �����F�̂Ƃ���͐؂�̂Ă�  // -- 2020.9.13

	Color = input.Depth.z/input.Depth.w;

	return Color;
}


// ---------------------------------------------------------
// 
// �V���h�E�}�b�v�e�N�X�`�����Q�Ƃ��Ȃ���
// �@�@�@�@�@�@�@�@�@�@�@�@�ʏ�`����s������
// 
// ---------------------------------------------------------

//
// �o�[�e�b�N�X�V�F�[�_�[
//
VS_OUTPUT VS(float3 Pos3 : POSITION, float3 Norm : NORMAL, float2 Tex : TEXCOORD, float3 Tangent : TANGENT, float3 Binormal : BINORMAL)

{
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4 Pos = float4(Pos3, 1);
	float4x4 inverseTangentMat;

	// �ˉe�ϊ��i���[���h���r���[���v���W�F�N�V�����j
	output.Pos = mul(Pos,g_mWVP);

	// �ڐ��̌v�Z
	// �ڋ�ԍs��̋t�s����Z�o
	inverseTangentMat = InverseTangentMatrix(normalize(Tangent), normalize(Binormal), normalize(Norm));

	// ���C�g�x�N�g����ڋ�ԏ�ɕϊ�
	output.lightTangentSpace = mul(float4(g_LightDir.xyz, 1.0), inverseTangentMat);


	// �@�������f���̎p���ɍ��킹��(���f������]����Ζ@������]������K�v�����邽�߁j
	output.Normal = normalize(mul(Norm, (float3x3)g_mW));

	// �����x�N�g���@���[���h��ԏ�ł̒��_���王�_�֌������x�N�g��
	output.PosWorld = mul(Pos, g_mW);
    output.EyeVector = normalize(g_EyePos.xyz - output.PosWorld.xyz);

	// �����x�N�g����ڋ�ԏ�ɕϊ�                   // -- 2021.5.27
	output.EyePosTangentSpace = mul(g_EyePos - output.PosWorld, inverseTangentMat);

	// �e�N�X�`���[���W
	output.Tex = Tex;

	//���C�g�r���[�ɂ�����ʒu(�ϊ���)
	for (uint i = 0; i < MAX_CASCADE_SHADOW; i++)
	{
		output.LighViewPos[i] = mul(Pos, g_mWLP[i]);
	}

	return output;
}

//
// �s�N�Z���V�F�[�_�[
//
float4 PS( VS_OUTPUT In ) : SV_Target
{
	float4 Color;
	uint width, height;
	g_Texture.GetDimensions(width, height);  // �f�B�t���[�Y�e�N�X�`���̃T�C�Y�𓾂�  // -- 2020.12.15

	// �����iParallax�j�}�b�s���O                   // -- 2021.5.27
	float4 texHeight = g_HeightTexture.Sample(g_samLinear, In.Tex);  // �n�C�g�}�b�v
	if (texHeight.w == 1.0f)
	{
		float scale = 0.02f;           // �����X�P�[��
		float h = (texHeight.x + texHeight.y + texHeight.z) / 3 * scale;
		In.Tex = In.Tex + h * normalize(In.EyePosTangentSpace).xy;    // �����ɂ���ăe�N�X�`�����W�����炷
	}

	// �e�N�X�`���J���[
	float4 texColor;
	float4 texSpecular;

	if (width == 0)  // �f�B�t���[�Y�e�N�X�`�����Ȃ��Ƃ�  // -- 2020.12.15
	{
		texColor = g_MatDiffuse;      // -- 2020.12.15
		texSpecular = g_MatSpecular;      // -- 2020.12.15
	}
	else {
		texColor = g_Texture.Sample(g_samLinear, In.Tex);
		texSpecular = g_SpecularTexture.Sample(g_samLinear, In.Tex);
	}
	float4 texNormal   = g_NormalTexture.Sample(g_samLinear, In.Tex);

	// �����̕������ݒ肳��Ă��邩�ǂ����̃`�F�b�N
	if (g_LightDir.x == 0 && g_LightDir.y == 0 && g_LightDir.z == 0)
	{
		// ���̕�����0,0,0�̏ꍇ�͌��̌v�Z�����Ȃ��ŕ`��
		Color.rgb = texColor.rgb * g_Diffuse.rgb;
		Color.a = texColor.a - (1 - g_Diffuse.a);
	}
	else {
		// ���̕������ݒ肳��Ă���ꍇ
		if (texNormal.w == 1.0f)   // �@���}�b�v�����邩�ǂ����`�F�b�N
		{
			// �@���}�b�v������ꍇ�͖@���}�b�s���O�t���̃��C�e�B���O
			// �@���}�b�s���O�̖��邳�v�Z
			float4 normalVector = normalize(2.0 * texNormal - 1.0f);  // �x�N�g���֕ϊ�(�@���}�b�v�̐F��0�`1.0�B�����-1.0�`1.0�̃x�N�g���͈͂ɂ���)
			float  NMbright = max( 0, dot(normalVector, normalize(In.lightTangentSpace)));// ���C�g�̌����Ɩ@���}�b�v�̖@���ƂŖ��x�Z�o�B�}�C�i�X��0�ɕ␳�B

			// �����̕����v�Z
			float halflamb = max(0, dot(normalize(In.Normal), g_LightDir.xyz));
			halflamb = halflamb * 0.6f + 0.7f;   // ������Ɩ���߂�
			Color.rgb = texColor.rgb * NMbright * halflamb * g_Diffuse.rgb;

			// �X�y�L�����̌v�Z
			Color.rgb = CalcSpecular(Color, texSpecular, In.PosWorld, In.Normal, NMbright).rgb;

			// �e�̌v�Z����
			if (g_DrawInfo.y == 2)
			{
				// �J��������̋����ɂ���āA�g�p����J�X�P�[�h�ԍ������肷��
				uint Cn;
				if (In.Pos.w < CASCADE_SPLIT)
				{
					Cn = 0;
				}
				else {
					Cn = MAX_CASCADE_SHADOW - 1;
				}
				Color = CalcShadow(Color, In.LighViewPos[Cn], Cn);
			}

			// �����F�̏���
			Color.a = texColor.a - (1 - g_Diffuse.a);
		}
		else {
			// �@���}�b�v���Z�b�g����Ă��Ȃ��ꍇ�́A�n�[�t�����o�[�g�ŕ`��
			float halflamb = max(0, dot(normalize(In.Normal), g_LightDir.xyz));
			//halflamb = halflamb * 0.5f + 0.8f;
			halflamb = halflamb * 0.7f + 0.6f;      // -- 2021.6.11
			Color.rgb = texColor.rgb * halflamb * g_Diffuse.rgb;

			// �X�y�L�����̌v�Z
			Color.rgb = CalcSpecular(Color, texSpecular, In.PosWorld, In.Normal, 1).rgb;

			// �e�̌v�Z����
			if (g_DrawInfo.y == 2)
			{
				// �J��������̋����ɂ���āA�g�p����J�X�P�[�h�ԍ������肷��
				uint Cn;
				if (In.Pos.w < CASCADE_SPLIT)
				{
					Cn = 0;
				}
				else {
					Cn = MAX_CASCADE_SHADOW - 1;
				}
				Color = CalcShadow(Color, In.LighViewPos[Cn], Cn);
			}

			// �����F�̏���
			Color.a = texColor.a - (1 - g_Diffuse.a);
		}
	}

	// �t�H�O�̌v�Z
	Color = CalcFog(Color, In.Pos, In.PosWorld);

	return saturate(Color);  // �ŏI�o�͒l��0�`�P�͈̔͂ɐ�������
}