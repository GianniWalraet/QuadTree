#pragma once
#include <list>
#include <vector>
#include <iterator>
#include "framework\EliteMath\EVector2.h"
#include "framework\EliteGeometry\EGeometry2DTypes.h"

class SteeringAgent;

class QuadTree
{
public:
	QuadTree(float left, float bottom, float width, float height, int capacity);

	void AddAgent(SteeringAgent* agent);

	void Subdivide();

	void Render();

private:
	Elite::Vector2 m_Pos;
	float m_Width;
	float m_Height;

	int m_Capacity;

	std::list<SteeringAgent*> m_pAgents;

	QuadTree* m_pTopLeft;
	QuadTree* m_pTopRight;
	QuadTree* m_pBottomLeft;
	QuadTree* m_pBottomRight;

	bool m_IsDivided;

	bool IsAgentInRect(SteeringAgent* agent);
};
