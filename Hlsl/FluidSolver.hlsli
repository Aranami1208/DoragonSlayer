// -----------------------------------------------------------------------
// 
// ���́E�v�Z�iSlover�j�̃V�F�[�_�[
// 
//                                                              2021.1.11
//                                                      FluidSolver.hlsli
// -----------------------------------------------------------------------

// �}�N����`
#define FLUID_SIZE 64
#define FLUID_RSIZE 8
#define FLUID_TIME_STEP 0.01

#define ID(i,j,k) (i)+(j)*(FLUID_SIZE+2)+(k)*(FLUID_SIZE+2)*(FLUID_SIZE+2)

// �T���v���[�X�e�[�g
SamplerState g_samLinear : register(s1);

//	�X�g���N�`���[�h�o�b�t�@�t�H�[�}�b�g
struct Buff
{
	float f;
	float f0;
};

// 3D�e�N�X�`��	(���x�p)
Texture3D<float4> g_rDns : register(t0);     // �ǂݏo���p
RWTexture3D<float4> g_wDns : register(u0);   // �������ݗp

// 3D�e�N�X�`��	(���x�p)
Texture3D<float4> g_rVlc : register(t1);
RWTexture3D<float4> g_wVlc : register(u1);

Texture3D<float4> g_rVlc_s : register(t2);
RWTexture3D<float4> g_wVlc_s : register(u2);

Texture3D<float4> g_rVlc_bs : register(t3);
RWTexture3D<float4> g_wVlc_bs : register(u3);

// �X�g���N�`���[�h�o�b�t�@
RWStructuredBuffer<Buff> g_Prs : register(u3); // u3�_�u�� ????????
RWStructuredBuffer<Buff> g_Div : register(u4);

// 3D�e�N�X�`��(�摜�p)
RWTexture3D<float4> g_D_Tex3D : register(u5);

// �R���X�^���g�o�b�t�@
cbuffer global
{
	float4 g_AddVoxelPos;
	float4 g_AddDensity;
	float4 g_AddVelocity;
	float4 g_Option;       //�I�v�V����(x:�\�������̂̑傫���@y:�F�R�[�h�@z:�Ȃ��@w:���������_����)
}

//
// ���x�Ƒ��x���w��ʒu�ɒ������A����������
//
[numthreads(1,1,1)]
void AddSource( uint3 id : SV_DispatchThreadID ,uint3 gid : SV_GroupID )
{
	float3 center = g_AddVoxelPos.xyz; // �w��ʒu
	int radius    = g_AddVoxelPos.w;   // �������a

	// ���a���ɒ���������������
	for(int i=-radius;i<radius;i++)
	{
		for(int j=-radius;j<radius;j++)
		{
			for(int k=-radius;k<radius;k++)
			{
				// HLSL�ɂ͗����֐����Ȃ����߁Adot��exp�ŋ^�������𔭐�������
				float3 p = float3(i,j,k);     // i,j,k���a�̒l���x�N�g���ɂ���
				float l = dot(p, p);          // ���ϒl�����߂�i�e�x�N�g�������o�̂Q��l�̌v�ƂȂ�j

				// exp��e���Ƃ���w�������߂�֐��ł���
				float rate = exp(-l/radius);  // 0�`1�̒l��Ԃ�
				float3 pos = center + p;      // �ʒu��i,j,k���a�̃x�N�g���𑫂�

				// �����ꏊ�𔼉~���ɁB�������x�������_����
				g_wDns[uint3(pos.x,pos.y,pos.z)] = g_rDns[uint3(pos.x,pos.y,pos.z)] + g_AddDensity*rate;

				// �����ꏊ�𔼉~���ɁB�������x�͎w������܂��̓����_����
				float  fRand = g_Option.w;  // �����_����
				float4 vAddVlc = g_AddVelocity;
				if (g_AddVelocity.x == 0 && g_AddVelocity.z == 0)	// �x�����ɔ������Ă���Ƃ�
				{
					vAddVlc.x = g_AddVelocity.y * (rate - 0.5f) * fRand * 3;
					vAddVlc.z = g_AddVelocity.y * (rate - 0.5f) * fRand * 3;
				}
				else if (g_AddVelocity.x == 0 && g_AddVelocity.y == 0)	// �y�����ɔ������Ă���Ƃ�
				 {	
					vAddVlc.x = g_AddVelocity.z * (rate - 0.5f) * fRand * 3;
					vAddVlc.y = g_AddVelocity.z * (rate - 0.5f) * fRand * 3;
				}
				//g_wVlc[uint3(pos.x,pos.y,pos.z)] = g_rVlc[uint3(pos.x,pos.y,pos.z)] + vAddVlc;		// �������x�i�����j�͈���
				g_wVlc[uint3(pos.x,pos.y,pos.z)] = g_rVlc[uint3(pos.x,pos.y,pos.z)] + vAddVlc*(1-fRand) + vAddVlc*(rate*fRand);	// �������x�i�����j�̓����_�����Ō��܂�
			}
		}
	}
}
//
// �ڗ����x�̌v�Z
//
[numthreads(FLUID_RSIZE,FLUID_RSIZE,FLUID_RSIZE)]
void AdvectDensity( uint3 id : SV_DispatchThreadID ,uint3 gid : SV_GroupID )
{
	// ��̒[��������FLUID_SIZE��������������B���̂��ߓY������i�߂�
	uint i = id.x+1;
	uint j = id.y+1;
	uint k = id.z+1;

	float3 uv = id+1;  // ���ꂩ�疧�x����������ꏊ�̈ʒu
	uv.x -= g_rVlc[uint3(i,j,k)].x*FLUID_TIME_STEP;  // ���x�������̂ڂ��Ĉ�O�̈ʒu
	uv.y -= g_rVlc[uint3(i,j,k)].y*FLUID_TIME_STEP;
	uv.z -= g_rVlc[uint3(i,j,k)].z*FLUID_TIME_STEP;

	uv=(uv+0.5)/(FLUID_SIZE+2);  // �ʒu���e�N�X�`���̂t�u���W�ɕϊ�����
	
	// �T���v�����O�ɂ��l�����o��
	// (�T���v�����O�ɂ���āA�g�����j�A�⊮�������I�ɍs����j
	g_wDns[uint3(i,j,k)]=g_rDns.SampleLevel(g_samLinear,uv,0);    // ���o���ꂽ�l�́Ag_wDns�i�������ݗ̈�j�ɏ�������

	// �l���R�c�e�N�X�`���[�ɏ�������
	g_D_Tex3D[uint3(id.x,id.y,id.z)] = g_rDns[uint3(i,j,k)];      //  2019.12.6 ���̓��e�@�@���o���Ƃ��́Ag_rDns(�ǂݍ��ݗ̈�)������o��
	//g_D_Tex3D[uint3(id.x,id.y,id.z)]= g_rDns.SampleLevel(g_samLinear, uv, 0);  // 2019.12.6 �ύX��


	// ��̌`�����������Ȃ��ꍇ�B��̒[�ł��傤�Ǐ�����悤�ɏ�����O���猸��������B
	float thresh = FLUID_SIZE*0.8f;
	float range = FLUID_SIZE*0.2f;
	float Disolve = 1;

	if( (int)i < FLUID_SIZE - thresh) Disolve =  min(Disolve, i / range);
	if( (int)j < FLUID_SIZE - thresh) Disolve =  min(Disolve, j / range);
	if( (int)k < FLUID_SIZE - thresh) Disolve =  min(Disolve, k / range);
	if( (int)i > thresh)        Disolve =  min(Disolve, (FLUID_SIZE-i) / range);
	if( (int)j > thresh)        Disolve =  min(Disolve, (FLUID_SIZE-j) / range);
	if( (int)k > thresh)        Disolve =  min(Disolve, (FLUID_SIZE-k) / range);
	if (Disolve < 1)
	{
		g_wDns[uint3(i,j,k)] = g_rDns.SampleLevel(g_samLinear,uv,0)*Disolve;
	}

}
//
// ���͂̌v�Z�P
//		���͂����߁A���x���甭�U�𔲂�
//
// �@�@���͂̔z��g_Prs���O�N�����[����
// �@�@���x�̋t�����̔��Ug_Div���v�Z����
//
[numthreads(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE)]
void Project1(uint3 id : SV_DispatchThreadID, uint3 gid : SV_GroupID)
{
	// ��̒[��������FLUID_SIZE��������������B���̂��ߓY������i�߂�
	uint i = id.x+1;
	uint j = id.y+1;
	uint k = id.z+1;

	// ���͂��[���N�����[����
	g_Prs[ID(i,j,k)].f=0;

	// ���x�̋t�����̔��UDiv���v�Z����
	g_Div[ID(i,j,k)].f = (g_rVlc[uint3(i+1,j,k)].x -g_rVlc[uint3(i-1,j,k)].x)/2+
						(g_rVlc[uint3(i,j+1,k)].y - g_rVlc[uint3(i,j-1,k)].y)/2+
						(g_rVlc[uint3(i,j,k+1)].z - g_rVlc[uint3(i,j,k-1)].z)/2;
}
//
// ���͂̌v�Z�Q
//		���͂����߁A���x���甭�U�𔲂�
//
// �A�@���Ug_Div�̃g�����j�A�⊮�����߁A���͂̔z��g_Prs�Ɋi�[����
//
[numthreads(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE)]
void Project2(uint3 id : SV_DispatchThreadID, uint3 gid : SV_GroupID)
{
	// ��̒[��������FLUID_SIZE��������������B���̂��ߓY������i�߂�
	uint i = id.x+1;
	uint j = id.y+1;
	uint k = id.z+1;

	// ���UDiv�̃g�����j�A�⊮�����߁A���͂̔z��Prs�Ɋi�[����
	g_Prs[ID(i,j,k)].f=(g_Prs[ID(i-1,j,k)].f+g_Prs[ID(i+1,j,k)].f+
						g_Prs[ID(i,j-1,k)].f+g_Prs[ID(i,j+1,k)].f+
						g_Prs[ID(i,j,k-1)].f+g_Prs[ID(i,j,k+1)].f-g_Div[ID(i,j,k)].f)/6;
}
//
// ���͂̌v�Z�R
//		���͂����߁A���x���甭�U�𔲂�
//
// �B�@����g_Prs���g���đ��xg_wVlc���v�Z����
//		(���x���爳�͒l�𔲂�)
//
[numthreads(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE)]
void Project3( uint3 id : SV_DispatchThreadID ,uint3 gid : SV_GroupID )
{
	// ��̒[��������FLUID_SIZE��������������B���̂��ߓY������i�߂�
	uint i = id.x+1;
	uint j = id.y+1;
	uint k = id.z+1;

	// ����Prs���g���đ��xvWlc���v�Z����
	float x=g_rVlc[uint3(i,j,k)].x-(g_Prs[ID(i+1,j,k)].f-g_Prs[ID(i-1,j,k)].f)/2;
	float y=g_rVlc[uint3(i,j,k)].y-(g_Prs[ID(i,j+1,k)].f-g_Prs[ID(i,j-1,k)].f)/2;
	float z=g_rVlc[uint3(i,j,k)].z-(g_Prs[ID(i,j,k+1)].f-g_Prs[ID(i,j,k-1)].f)/2;

	g_wVlc[uint3(i,j,k)]=float4(x,y,z,0);
}

//
// �ڗ����x�̌v�Z
//
[numthreads(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE)]
void AdvectVelocity( uint3 id : SV_DispatchThreadID ,uint3 gid : SV_GroupID )
{
	// ��̒[��������FLUID_SIZE��������������B���̂��ߓY������i�߂�
	uint i = id.x+1;
	uint j = id.y+1;
	uint k = id.z+1;

	float3 uv = id+1;  // ���ꂩ�瑬�x����������ꏊ�̈ʒu
	uv.x -= g_rVlc[uint3(i,j,k)].x*FLUID_TIME_STEP;  // ���x�������̂ڂ��Ĉ�O�̈ʒu
	uv.y -= g_rVlc[uint3(i,j,k)].y*FLUID_TIME_STEP;
	uv.z -= g_rVlc[uint3(i,j,k)].z*FLUID_TIME_STEP;

	uv = (uv+0.5)/(FLUID_SIZE+2);  // �ʒu���e�N�X�`���̂t�u���W�ɕϊ�����
	
	// �T���v�����O�ɂ��l(�Z�~�O�����W�A��)�����o��g_wVlc_s�Ɋi�[����
	// (�T���v�����O�ɂ���āA�g�����j�A�⊮�������I�ɍs����)
	g_wVlc_s[uint3(i,j,k)] = g_rVlc.SampleLevel(g_samLinear,uv,0);   // MacCormack���g�p����Ƃ�
	//g_wVlc[uint3(i,j,k)] = g_rVlc.SampleLevel(g_samLinear,uv,0);   // MacCormack���g�p���Ȃ��Ƃ�
}

//
// �t�����̃Z�~�O�����W�A�����s�� (MacCormack���g�p����Ƃ�)
//
[numthreads(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE)]
void AdvectBack( uint3 id : SV_DispatchThreadID ,uint3 gid : SV_GroupID )
{
	// ��̒[��������FLUID_SIZE��������������B���̂��ߓY������i�߂�
	uint i = id.x+1;
	uint j = id.y+1;
	uint k = id.z+1;

	float3 uv = id+1;  // ���ꂩ�瑬�x����������ꏊ�̈ʒu

	uv.x += g_rVlc[uint3(i,j,k)].x*FLUID_TIME_STEP;  // ���x��i�߂Ď��̈ʒu
	uv.y += g_rVlc[uint3(i,j,k)].y*FLUID_TIME_STEP;
	uv.z += g_rVlc[uint3(i,j,k)].z*FLUID_TIME_STEP;

	uv = (uv+0.5)/(FLUID_SIZE+2);  // �ʒu���e�N�X�`���̂t�u���W�ɕϊ�����
	
	// �T���v�����O�ɂ��l(�t�����̃Z�~�O�����W�A��)�����o��g_wVlc_bs�Ɋi�[����
	// (�T���v�����O�ɂ���āA�g�����j�A�⊮�������I�ɍs����)
	g_wVlc_bs[uint3(i,j,k)] = g_rVlc_s.SampleLevel(g_samLinear,uv,0);
}
//
// �}�R�[�}�b�N�X�L�[����ǉ����� (MacCormack���g�p����Ƃ�)
//
[numthreads(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE)]
void MacCormack( uint3 id : SV_DispatchThreadID ,uint3 gid : SV_GroupID )
{
	// ��̒[��������FLUID_SIZE��������������B���̂��ߓY������i�߂�
	uint i = id.x+1;
	uint j = id.y+1;
	uint k = id.z+1;

	// �}�R�[�}�b�N�X�L�[���v�Z���s��
	float4 New = g_rVlc_s[uint3(i,j,k)] + 0.5*(g_rVlc[uint3(i,j,k)] - g_rVlc_bs[uint3(i,j,k)]);

	// �j�]�h�~���~�b�^�[
	float4 Max = 0, Min = 0;
	float4 v1 = g_rVlc_s[uint3(i,j,k)];
	float4 v2 = g_rVlc_s[uint3(i+1,j,k)];
	float4 v3 = g_rVlc_s[uint3(i,j+1,k)];
	float4 v4 = g_rVlc_s[uint3(i,j,k+1)];
	float4 v5 = g_rVlc_s[uint3(i-1,j,k)];
	float4 v6 = g_rVlc_s[uint3(i,j-1,k)];
	float4 v7 = g_rVlc_s[uint3(i,j,k-1)];
	float4 v8 = g_rVlc_s[uint3(i+1,j+1,k+1)];

	Max = max(max(max(max(max(max(max(v1,v2),v3),v4),v5),v6),v7),v8);
	Min = min(min(min(min(min(min(min(v1,v2),v3),v4),v5),v6),v7),v8);

	float4 Mac = max( min( New, Max ), Min);

	// �㏸�C���Ȃ�
	//g_wVlc[uint3(i, j, k)] = Mac;

	// �㏸�C��(�Ȉ�)������
	//float Force = 15.0 * g_rDns[uint3(i,j,k)].x;
	float Force = 0.5f * g_rDns[uint3(i,j,k)].x;
	g_wVlc[uint3(i,j,k)] = Mac + float4(0, Force, 0, 0);

}
//
// �[�܂ōs������܂�Ԃ��P
//
// �@�@��̒[�܂ōs��������O�̑��x�𔽓]������
//
[numthreads(FLUID_RSIZE, FLUID_RSIZE,1)]
void Boundary1(uint3 id : SV_DispatchThreadID, uint3 gid : SV_GroupID)
{
	// ��̒[��������FLUID_SIZE��������������B���̂��ߓY������i�߂�
	uint i = id.x+1;
	uint j = id.y+1;
	uint k = id.z+1;

	// ��̒[�܂ōs��������O�̑��x�𔽓]������
	g_wVlc[uint3(0,i,j)]=float4(-g_rVlc[uint3(1,i,j)].x,g_rVlc[uint3(0,i,j)].y,g_rVlc[uint3(0,i,j)].z,0);
	g_wVlc[uint3(FLUID_SIZE,i,j)]=float4(-g_rVlc[uint3(FLUID_SIZE-1,i,j)].x,g_rVlc[uint3(0,i,j)].y,g_rVlc[uint3(0,i,j)].z,0);

	g_wVlc[uint3(i,0,j)]=float4(g_rVlc[uint3(i,0,j)].x,-g_rVlc[uint3(i,1,j)].y,g_rVlc[uint3(i,0,j)].z,0);
	g_wVlc[uint3(i,FLUID_SIZE,j)]=float4(g_rVlc[uint3(i,FLUID_SIZE,j)].x,-g_rVlc[uint3(i,FLUID_SIZE-1,j)].y,g_rVlc[uint3(i,FLUID_SIZE,j)].z,0);

	g_wVlc[uint3(i,j,0)]=float4(g_rVlc[uint3(i,j,0)].x,g_rVlc[uint3(i,j,0)].y,-g_rVlc[uint3(i,j,1)].z,0);
	g_wVlc[uint3(i,j,FLUID_SIZE)]=float4(g_rVlc[uint3(i,j,FLUID_SIZE)].x,g_rVlc[uint3(i,j,FLUID_SIZE)].y,-g_rVlc[uint3(i,j,FLUID_SIZE-1)].z,0);

}
