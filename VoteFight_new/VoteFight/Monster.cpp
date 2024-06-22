#include "pch.h"
#include "Monster.h"
#include "GameFramework.h"
#include "TimeManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Animator.h"
#include "StateMachine.h"
#include "Transform.h"
#include "MonsterStates.h"

CMonster::CMonster() :
	m_spineName("mixamorig:Spine"),
	m_spineAngle(),
	m_turnAngle()
{
	SetName("Monster");
}

CMonster::~CMonster()
{
}


string CMonster::GetSpineName()
{
	return m_spineName;
}

float CMonster::GetSpineAngle()
{
	return m_spineAngle;
}

void CMonster::SetSpineAngle(float angle)
{
	m_spineAngle = angle;
}

void CMonster::Init()
{
	CStateMachine* stateMachine = static_cast<CStateMachine*>(GetComponent(COMPONENT_TYPE::STATE_MACHINE));

	stateMachine->SetCurrentState(CMonsterWalkState::GetInstance());

	CAnimator* animator = static_cast<CAnimator*>(GetComponent(COMPONENT_TYPE::ANIMATOR));

	animator->SetWeight("Walk", 1.0f);
}

void CMonster::Update()
{
	CObject::Update();
	if (AimObejct)
	{
		XMFLOAT3 MyPostion = GetPostion();
		XMFLOAT3 TargetPostion = AimObejct->GetPostion();
		double dx = TargetPostion.x - MyPostion.x;
		double dz = TargetPostion.z - MyPostion.z;
		float angle = atan2(dz, dx); // 또는 M_PI
		cout << angle << endl;
		// Move the object towards the target by the specified distance
		MyPostion.x += m_fSpeed * cos(angle);
		MyPostion.z += m_fSpeed * sin(angle);

		SetPostion(MyPostion);

		XMFLOAT3 direction;
		direction.x = 0;
		direction.y = -angle * (180.0 / 3.14) + 90;
		direction.z = 0;

		SetRotate(direction);
	}
}

void CMonster::PlayerDiscovery(CObject* player)
{
	// 플레이어 발견 조건
	AimObejct = player;
}

void CMonster::OnCollisionEnter(CObject* collidedObject)
{

}

void CMonster::OnCollisionExit(CObject* collidedObject)
{
}

