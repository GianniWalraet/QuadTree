#pragma once
#include "../SteeringHelpers.h"
#include "FlockingSteeringBehaviors.h"

class ISteeringBehavior;
class SteeringAgent;
class BlendedSteering;
class PrioritySteering;
class QuadTree;

class Flock
{
public:
	Flock(
		int flockSize = 50,
		float worldSize = 500.f,
		SteeringAgent* pAgentToEvade = nullptr,
		bool trimWorld = false);

	~Flock();

	void Update(float deltaT, const TargetData& target);
	void UpdateAndRenderUI();
	void Render(float deltaT);

	void RegisterNeighbors(SteeringAgent* pAgent);
	int GetNrOfNeighbors() const { return m_NrOfNeighbors; }
	const vector<SteeringAgent*>& GetNeighbors() const { return m_pNeighbors; }

	Elite::Vector2 GetAverageNeighborPos() const;
	Elite::Vector2 GetAverageNeighborVelocity() const;

private:
	// flock agents
	int m_FlockSize = 0;
	vector<SteeringAgent*> m_pAgents;
	//vector<Elite::Vector2> m_OldPositions;
		 
	// neighborhood agents
	vector<SteeringAgent*> m_pNeighbors;
	float m_NeighborhoodRadius = 10.f;
	int m_NrOfNeighbors = 0;

	// evade target
	SteeringAgent* m_pAgentToEvade = nullptr;

	// world info
	bool m_TrimWorld = true;
	float m_WorldSize = 0.f;

	//Debug
	bool m_CanDebugRender = false;
	bool m_UseSpatialPartitioning = true;
	
	// steering Behaviors
	BlendedSteering* m_pBlendedSteering = nullptr;
	Cohesion* m_pCohesion = nullptr;
	Seperation* m_pSeperation = nullptr;
	Allignment* m_pAllignment = nullptr;
	Seek* m_pSeek = nullptr;
	Wander* m_pWander = nullptr;

	PrioritySteering* m_pPrioritySteering = nullptr;
	Evade* m_pEvade = nullptr;

	// Spatial Partitioning
	QuadTree* m_QuadTree = nullptr;

	// private functions
	float* GetWeight(ISteeringBehavior* pBehaviour);

private:
	Flock(const Flock& other);
	Flock& operator=(const Flock& other);
};