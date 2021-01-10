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
	~QuadTree();

	void AddAgent(SteeringAgent* pAgent);
	void Subdivide();
	void Merge();
	void RegisterNeighbours(SteeringAgent* pAgent, float queryRadius, std::vector<SteeringAgent*>& pNeighbours, int& nrOfNeighbours);

	void Update(SteeringAgent* pAgent);

	void Render();
private:
	Elite::Rect m_BoundingBox;

	size_t m_Capacity;

	std::vector<SteeringAgent*> m_pAgents;

	QuadTree* m_pTopLeft;
	QuadTree* m_pTopRight;
	QuadTree* m_pBottomLeft;
	QuadTree* m_pBottomRight;

	bool m_IsDivided;

	bool IsAgentInBounds(const Elite::Rect& r, const Elite::Vector2& p);
};
