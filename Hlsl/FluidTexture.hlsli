// -----------------------------------------------------------------------
// 
// ���́E�e�N�X�`���̃V�F�[�_�[
// 
//                                                              2021.1.11
//                                                     FluidTexture.hlsli
// -----------------------------------------------------------------------

// �O���[�o��
cbuffer global
{
	matrix g_mW;          // ���[���h�s��
	matrix g_mWVP;        // ���[���h����ˉe�܂ł̕ϊ��s��
	float4 g_vLightDir;   // ���C�g�̕����x�N�g��
	float4 g_EyePos;      // ���_�ʒu
	float4 g_vOption;     // �I�v�V����(x:�\�������̂̑傫���@y:�F�R�[�h�@z:�Ȃ��@w:���������_����)
	float4 g_DrawInfo;    // �e����@(�g���Ă��Ȃ�)
};

// �\����
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 LocalPos : POSITION;
};

//
// �o�[�e�b�N�X�V�F�[�_�[
//
VS_OUTPUT VS( float3 Pos : POSITION)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4 Pos4 = float4(Pos, 1);
	output.Pos = mul(Pos4, g_mWVP);

	// ���_�̃��[�J�����W�l���o�͂���
	output.LocalPos.xyz = Pos4.xyz / g_vOption.x;  // ��ӂ̑傫���𐳋K������
	output.LocalPos.w = Pos4.w;

	return output;
}

//
// �s�N�Z���V�F�[�_�[
//
float4 PS( VS_OUTPUT input ) : SV_Target
{
	// �\���ʒu�̃��[�J�����W�l���o�͂���i�n�_�܂��͏I�_�ƂȂ�j
	float4 ret = input.LocalPos;
	return ret;
}