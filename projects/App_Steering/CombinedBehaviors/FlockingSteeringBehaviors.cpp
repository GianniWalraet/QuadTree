#include "stdafx.h"
#include "FlockingSteeringBehaviors.h"
#include "TheFlock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"

//*******************
//COHESION (FLOCKING)
Cohesion::Cohesion(Flock* pFlock)
	: m_pFlock{pFlock}
{
}
Cohesion::~Cohesion()
{
}

SteeringOutput Cohesion::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	m_Target = m_pFlock->GetAverageNeighborPos();

	return Seek::CalculateSteering(deltaT, pAgent);
}

//*********************
//SEPARATION (FLOCKING)
Seperation::Seperation(Flock* pFlock)
	: m_pFlock{ pFlock }
{
}
Seperation::~Seperation()
{
}

SteeringOutput Seperation::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	Elite::Vector2 total{};
	Elite::Vector2 vector{};

	if (m_pFlock->GetNrOfNeighbors() == 0)
	{
		return steering;
	}

	for (int i = 0; i < m_pFlock->GetNrOfNeighbors(); i++)
	{
		vector = pAgent->GetPosition() - m_pFlock->GetNeighbors()[i]->GetPosition();
		float distance = vector.MagnitudeSquared();
		float inverseDistance = 1.f / distance;
		vector.Normalize();
		vector *= inverseDistance;
		total += vector;
	}

	steering.LinearVelocity = total.GetNormalized() * pAgent->GetMaxLinearSpeed();
	
	return steering;
}

//*************************
//VELOCITY MATCH (FLOCKING)
Allignment::Allignment(Flock* pFlock)
	: m_pFlock{ pFlock }
{
}
Allignment::~Allignment()
{
}

SteeringOutput Allignment::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	steering.LinearVelocity = m_pFlock->GetAverageNeighborVelocity();

	return steering;
}