#pragma once
#include "../SteeringBehaviors.h"

class Flock;

//COHESION - FLOCKING
//*******************
class Cohesion : public Seek
{
public:
	Cohesion(Flock* pFlock);
	virtual ~Cohesion();

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
private:
	Flock* m_pFlock;
};

//SEPARATION - FLOCKING
//*********************
class Seperation : public ISteeringBehavior
{
public:
	Seperation(Flock* pFlock);
	virtual ~Seperation();

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
private:
	Flock* m_pFlock;
};


//VELOCITY MATCH - FLOCKING
//************************
class Allignment : public Pursuit
{
public:
	Allignment(Flock* pFlock);
	virtual ~Allignment();

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
private:
	Flock* m_pFlock;
};