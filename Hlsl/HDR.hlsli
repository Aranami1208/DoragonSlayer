// -----------------------------------------------------------------------
// 
// �g�c�q�̃V�F�[�_�[
// 
//                                                              2020.1.24
//                                                         Effect3D.hlsli
// -----------------------------------------------------------------------

//�O���[�o��
Texture2D g_Texture: register(t0);
Texture2D g_BloomTexture: register(t1);
SamplerState g_Sampler : register(s0);

//�O���[�o��
cbuffer global_0:register(b0)
{
	float2 g_ViewPort;  // �r���[�|�[�g�i�X�N���[���j�T�C�Y
	float2 g_Info;      // �w����ix:�������l y:�u���[���̋����j
};
cbuffer global_1:register(b1)
{
	float4 g_OffsetWeight[15];        //  x,y:�I�t�Z�b�g  w:�E�F�C�g
};

//�\����
struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD;
};

//
// �o�[�e�b�N�X�V�F�[�_�[
//
PS_INPUT VS(float4 Pos : POSITION, float2 UV : TEXCOORD)
{
	PS_INPUT Out;

	Out.Pos = Pos;

	Out.Pos.x = (Out.Pos.x / g_ViewPort.x) * 2 - 1;
	Out.Pos.y = 1 - (Out.Pos.y / g_ViewPort.y) * 2;

	Out.UV = UV;

	return Out;
}

//
// �s�N�Z���V�F�[�_�[ �u���C�g�e�N�X�`���̍쐬	
//
float4 PS_BRIGHT(PS_INPUT In) : SV_Target
{
	float MinBright = g_Info.x; // �������l
	float4 Color;
	float MonoColor;

	Color = float4(max(0, g_Texture.Sample(g_Sampler, In.UV) - MinBright).rgb, 1); // �������l��薾�邢�Ƃ����I��
	MonoColor = min(1, (Color.r + Color.g + Color.b));    // ���m�g�[���ɂ���B�߂����ς����邭����
	Color = float4(MonoColor, MonoColor, MonoColor, Color.a);

	return Color;
}

//
// �s�N�Z���V�F�[�_�[ �u���[���e�N�X�`���̍쐬
//
float4 PS_BLOOM(PS_INPUT In) : SV_Target
{
	float4 Color = 0;
	for (int i = 0; i < 15; i++)
	{
		float2 UV = In.UV + g_OffsetWeight[i].xy;
		if (UV.x < 0.0f || UV.x > 1.0f || UV.y < 0 || UV.y > 1)  // �e�N�X�`�����W�̊O���ǂ���
		{
			;  // �e�N�X�`�����W�̊O�̓u���[����ǉ����Ȃ�
		}
		else {
			// ���̃e�N�X�`���Ƀu���[����ǉ�����
			Color += g_Texture.Sample(g_Sampler, UV) * g_OffsetWeight[i].w;
		}
	}

	return Color;
}

//
// �s�N�Z���V�F�[�_�[ �ŏI�o��
//
float4 PS(PS_INPUT In) : SV_Target
{
	float4 Color;
	float4 texColor = g_Texture.Sample(g_Sampler, In.UV); // �I���W�i���̉摜
	Color = texColor + g_BloomTexture.Sample(g_Sampler, In.UV) * g_Info.y; // �u���[���摜����������
	//Color = g_BloomTexture.Sample(g_Sampler, Input.UV);  // �u���[���e�N�X�`���݂̂�\������Ƃ�
	Color.a = texColor.a;
	return Color;
}