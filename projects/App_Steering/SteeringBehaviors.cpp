//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "SteeringAgent.h"

//SEEK
//****
SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	steering.LinearVelocity = m_Target.Position - pAgent->GetPosition();
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0.f,1.f,0.f,0.5f }, 0.40f);

	return steering;
}

//WANDER (base> SEEK)
//******
SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	Elite::Vector2 circleCentre = pAgent->GetPosition() + pAgent->GetLinearVelocity().GetNormalized() * m_Offset;
	m_WanderAngle += randomFloat(-m_AngleChange / 2.f, m_AngleChange / 2.f);
	Elite::Vector2 wanderDir = { cosf(m_WanderAngle), sinf(m_WanderAngle)  };

	m_Target = TargetData{ circleCentre + wanderDir * m_Radius  };

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0.f,1.f,0.f,0.5f }, 0.40f);
		DEBUGRENDERER2D->DrawCircle(circleCentre, m_Radius, { 0,0,1,1 }, 0.5f);
		DEBUGRENDERER2D->DrawSegment(circleCentre, m_Target.Position, { 0,0,1,1 });
	}
		
	return Seek::CalculateSteering(deltaT, pAgent);
}

//FLEE
//****
SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	auto distanceToTarget = Distance(pAgent->GetPosition(), m_Target.Position);

	if (distanceToTarget > m_FleeRadius)
	{
		SteeringOutput steering;
		steering.IsValid = false;
		return steering;
	}

	auto steering = Seek::CalculateSteering(deltaT, pAgent);
	steering.LinearVelocity = steering.LinearVelocity * -1.0f;

	/*SteeringOutput steering = {};

	steering.LinearVelocity = pAgent->GetPosition() - m_Target.Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();*/

	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0.f,1.f,0.f,0.5f }, 0.40f);

	return steering;
}

//ARRIVE
//****
SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	steering.LinearVelocity = m_Target.Position - pAgent->GetPosition();
	float distance = steering.LinearVelocity.Magnitude();

	steering.LinearVelocity.Normalize();

	if (distance < m_SlowingRadius)
	{
		// Inside slowing area
		steering.LinearVelocity *= (pAgent->GetMaxLinearSpeed() * distance / m_SlowingSpeed);
	}
	else 
	{
		// Outside slowing area.
		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();
	}

	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0.f,1.f,0.f,0.5f }, 0.40f);

	return steering;
}

//FACE
//****
SteeringOutput Face::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	pAgent->SetAutoOrient(false);
	const float pi = { float(M_PI) };
	const float currentAngle = pAgent->GetOrientation();
	Elite::Vector2 targetDir{ m_Target.Position - pAgent->GetPosition() };
	targetDir.Normalize();
	float rotation = atan2(targetDir.y, targetDir.x) - currentAngle + (pi / 2.f);

	while (rotation > pi) rotation -= (2.f * pi);
	while (rotation < -pi) rotation += (2.f * pi);

	steering.AngularVelocity = Clamp(ToDegrees(rotation), -pAgent->GetMaxAngularSpeed(), pAgent->GetMaxAngularSpeed());
	
	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0.f,1.f,0.f,0.5f }, 0.40f);

	return steering;
}

//EVADE
//****
SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	Elite::Vector2& targetPos{ m_Target.Position };
	targetPos += m_Target.LinearVelocity;

	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(targetPos, steering.LinearVelocity, 5, { 0.f,1.f,0.f,0.5f }, 0.40f);

	return Flee::CalculateSteering(deltaT, pAgent);
}

//PURSUIT
//****
SteeringOutput Pursuit::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	Elite::Vector2 pursuitPoint = m_Target.Position + m_Target.LinearVelocity.GetNormalized() * m_Offset;
	m_Target = pursuitPoint;

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0.f,1.f,0.f,0.5f }, 0.40f);
		DEBUGRENDERER2D->DrawPoint(pursuitPoint, 5.f, { 1,0,0,1 });
	}

	return Seek::CalculateSteering(deltaT, pAgent);
}