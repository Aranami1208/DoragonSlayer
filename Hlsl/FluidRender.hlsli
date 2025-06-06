// -----------------------------------------------------------------------
// 
// ���́E�����_�����O�̃V�F�[�_�[
// 
//                                                              2021.1.11
//                                                      FluidRender.hlsli
// -----------------------------------------------------------------------

// �O���[�o��
Texture2D g_texA: register(t0);
Texture2D g_texB: register(t1);
Texture3D g_tex3D: register(t2);
SamplerState g_samLinear : register(s0);

cbuffer global
{
	matrix g_mW;          // ���[���h�s��
	matrix g_mWVP;        // ���[���h����ˉe�܂ł̕ϊ��s��
	float4 g_vLightDir;   // ���C�g�̕����x�N�g��
	float4 g_EyePos;      // ���_�ʒu
	float4 g_vOption;     // �I�v�V����(x:�\�������̂̑傫���@y:�F�R�[�h�@z:�Ȃ��@w:���������_����)
	float4 g_DrawInfo;    // �e����(�g���Ă��Ȃ�)
};

// �o�[�e�b�N�X�V�F�[�_�[�o�͗p�\����
struct VS_OUTPUT
{
	float4 Pos  : SV_POSITION;
	float4 CPos : POSITION;
};

// �s�N�Z���V�F�[�_�[�o�͗p�̍\����
struct PS_OUTPUT
{
	float4 ColorBuffer : SV_Target;   // �J���[�}�b�v
	float  DepthBuffer : SV_Depth;    // �[�x�}�b�v
};

// �Q�l�ƂȂ�T�C�g
//
// http:// maverickproj.web.fc2.com/d3d11_21.html
// https: //t-pot.com/program/98_BG-Map/index.html
// https: //qpp.bitbucket.io/translation/maximizing_depth_buffer_range_and/
// https: //enginetrouble.net/2016/10/reconstructing-world-position-from-depth-2016.html
//

//
// �o�[�e�b�N�X�V�F�[�_�[
//
VS_OUTPUT VS( float3 Pos : POSITION)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4 Pos4 = float4(Pos, 1);
	output.Pos  = mul(Pos4,g_mWVP);
	output.CPos = output.Pos;

	return output;
}

//
// �s�N�Z���V�F�[�_�[
//
// �@�R�c�e�N�X�`�������C�}�[�`���O�i�s�i�j�̎�@�Ń����_�����O����
// �A�[�x�o�b�^�@�����������A���̊O�s��[�x�ʂƂ���
//
PS_OUTPUT PS( VS_OUTPUT input )
{	
	PS_OUTPUT output = (PS_OUTPUT)0;

	const int Loops = 64;        // ���C�̍s�i�񐔁i�����قǍׂ����Ȃ�j

	const float Stride = 1.0f / Loops;   // ���C�̍s�i�̕�

	// ���_���W����e�N�X�`���[���W���쐬
	float2 Tex =  input.CPos.xy /= input.CPos.w;

    Tex.x      =  0.5f * Tex.x + 0.5f;
    Tex.y      = -0.5f * Tex.y + 0.5f;

	float3 Front = g_texA.Sample(g_samLinear,Tex).xyz;  // ���C�̍s�i�̎n�_
    float3 Back = g_texB.Sample(g_samLinear,Tex).xyz;   // ���C�̍s�i�̏I�_

	float3 Dir = normalize(Back-Front);                 // ���C�̕���	
    float3 Step = Stride * Dir;                         // 1��̍s�i��
    float4 Texel3D = float4(0,0,0,0);

	float   MaxDns = 0;			// ���C�̒��̍ő喧�x
	float   DnsZ   = 0;			// ���C�̒��ōŏ��ɖ��x���o�������ʒu�B�܂藬�̂̊O�s
	float3  FrontStart = Front;	// ���C�̎n�_��ۑ�
	for(int i=0; i<Loops; i++)	// ���C���n�_����I�_�Ɍ�������Step�ÂÐi�߂čő�l�����߂�
	{
		Texel3D = g_tex3D.Sample(g_samLinear, Front);	// �R�c�e�N�X�`������l�𓾂�
		Front += Step;									// �X�e�b�v��i�߂�

		// ���x��r�̂ݎg�p����Bgba�͎g�p���Ă��Ȃ�)
		MaxDns = max(MaxDns, Texel3D.r);				// ���C�̒��̍ő喧�x��MaxDns�ɋ��߂�

		if (DnsZ == 0 && Texel3D.r > 0.001f)    // �ŏ��ɖ��x���o�������ʒu��DnsZ�ɃZ�b�g����
		{
			DnsZ = length(Front-FrontStart) * g_vOption.x;   // ���̂̊O�s�̃��C�̎n�_����̈ʒu�i���j
		}
	}

	// �[�x�o�b�t�@�ɏ�������
	// 
	// input.Pos : SV_POSITION �̒l
	//  x = �����_�[�^�[�Q�b�g���ł�X���W (�Ⴆ�Ή�����512�s�N�Z���̃����_�[�^�[�Q�b�g�ɏ�������ł���Ȃ�A0�`511�̒l)
	//  y = �����_�[�^�[�Q�b�g���ł�Y���W(�Ⴆ�Ώc����512�s�N�Z���̃����_�[�^�[�Q�b�g�ɏ�������ł���Ȃ�A0�`511�̒l)
	//  z = �[�x�l(0.0�`1.0�B�܂�w�ŏ��Z�ς݂̒l)
	//  w = �v���W�F�N�V�����}�g���N�X�ŕϊ��������w�l���̂���(z�l�ɂ��g�k���B���̐ϖc��ݏ��)
	// 
	//output.DepthBuffer = input.Pos.z;   // �\���p�����̂̕\�ʈʒu�̐[�x(�]���̕��@)
	output.DepthBuffer = input.Pos.z +  DnsZ / input.Pos.w * (1-input.Pos.z);   // ���̂̊O�s�[�x�̐ݒ�A����ł����̂�??

	// �ŏI�I�ɖ��x���O�̂Ƃ��́A�\���s�N�Z����j������
	if (DnsZ == 0) discard;

	// �Â�DarkColor�Ɩ��邢BrightColor��2�F��p�ӂ������̒��ԐF���ŏI�F�Ƃ���
	float4 DarkColor   = float4(0, 0, 0, 1);
	float4 BrightColor = float4(0, 0, 0, 1);

	// �F�R�[�h�̎w��ɂ���ĕ\������F��I������
	switch (g_vOption.y)
	{
	case 0:
		// ���i�����Ȃ��j
		DarkColor = float4(0,-1,-2,0);			// �Â��ԐF(�����ɂ͂Ȃ�Ȃ�)
		BrightColor = float4(1,0.5,0.0,2);		// ���邢���F
		break;
	case 1:
		// ���i��������j
		DarkColor   = float4(-0.5f,-1,-1.5f,0);	// �Â��ԐF�i���������ɍ����ɂȂ�j
		BrightColor = float4(1,0.5,0.0,2);		// ���邢���F
		break;
	case 2:
		// ����
		DarkColor   = float4(-3.0f,-2.0f,-1.0f,0.0f)/2;	// �Â��F
		BrightColor = float4(0.5f,1.5f,1.5f,2);			// ���邢��F
		break;
	case 3:
		// �Z����
		DarkColor = float4(0.3f,0.3f,0.3f,0);			// �O���C
		BrightColor = float4(1.0f,1.0f,1.0f,1.0f);		// ���邢��
		break;
	case 4:
		// ������
		DarkColor = float4(0.25f,0.25f,0.25f,0);		// �O���C
		BrightColor = float4(0.4f,0.4f,0.4f,0.1f);		// ������
		break;
	}

	// �F�̃o�b�t�@�ɏ�������
	output.ColorBuffer = saturate( lerp(DarkColor, BrightColor, MaxDns) );  // MaxDns�̒l�ɂ���āADarkColor��BrightColor�̊Ԃ���`�⊮lerp����	

	return output;
}