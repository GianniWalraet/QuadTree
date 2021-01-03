#pragma once
#include <list>
#include <vector>
#include <iterator>
#include "framework\EliteMath\EVector2.h"
#include "framework\EliteGeometry\EGeometry2DTypes.h"

class SteeringAgent;

struct Quad
{

};

class QuadTree
{
public:
	QuadTree(float left, float bottom, float width, float height, size_t capacity);

	bool AddAgent(SteeringAgent* pAgent);
	void Subdivide();
	void RegisterNeighbours(SteeringAgent* pAgent, float queryRadius, std::vector<SteeringAgent*>& pNeighbours, int& nrOfNeighbours);

	void Render();

private:
	Elite::Rect m_BoundingBox;

	size_t m_Capacity;

	std::list<SteeringAgent*> m_pAgents;

	QuadTree* m_pTopLeft;
	QuadTree* m_pTopRight;
	QuadTree* m_pBottomLeft;
	QuadTree* m_pBottomRight;

	bool m_IsDivided;
};
