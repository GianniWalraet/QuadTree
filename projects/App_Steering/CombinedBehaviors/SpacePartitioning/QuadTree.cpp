#include "stdafx.h"
#include "QuadTree.h"
#include "projects\App_Steering\SteeringAgent.h"

QuadTree::QuadTree(float left, float bottom, float width, float height, size_t capacity)
	: m_BoundingBox{ Elite::Vector2{left, bottom}, width, height }
	, m_Capacity{ capacity }
	, m_pAgents{}
	, m_pTopLeft{}
	, m_pTopRight{}
	, m_pBottomLeft{}
	, m_pBottomRight{}
	, m_IsDivided{ false }
{
}

QuadTree::~QuadTree()
{
	SAFE_DELETE(m_pTopLeft);
	SAFE_DELETE(m_pTopRight);
	SAFE_DELETE(m_pBottomLeft);
	SAFE_DELETE(m_pBottomRight);
}

void QuadTree::AddAgent(SteeringAgent* agent)
{
	if (!IsAgentInBounds(m_BoundingBox, agent->GetPosition()))
		return;

	if (m_pAgents.size() < m_Capacity)
	{
		m_pAgents.push_back(agent);
		return;
	}
	else
	{
		if (!m_IsDivided)
			Subdivide();

		m_pTopLeft->AddAgent(agent);
		m_pTopRight->AddAgent(agent);
		m_pBottomLeft->AddAgent(agent);
		m_pBottomRight->AddAgent(agent);
	}
}

void QuadTree::Subdivide()
{
	float x = m_BoundingBox.bottomLeft.x;
	float y = m_BoundingBox.bottomLeft.y;
	float w = m_BoundingBox.width;
	float h = m_BoundingBox.height;

	m_pTopLeft = new QuadTree(x, y + h / 2, w / 2, h / 2, m_Capacity);
	m_pTopRight = new QuadTree(x + w / 2, y + h / 2, w / 2, h / 2, m_Capacity);
	m_pBottomLeft = new QuadTree(x, y, w / 2, h / 2, m_Capacity);
	m_pBottomRight = new QuadTree(x + w / 2, y, w / 2, h / 2, m_Capacity);

	m_IsDivided = true;
}
void QuadTree::Merge()
{
	delete m_pTopLeft;
	m_pTopLeft = nullptr;
	delete m_pTopRight;
	m_pTopRight = nullptr;
	delete m_pBottomLeft;
	m_pBottomLeft = nullptr;
	delete m_pBottomRight;
	m_pBottomRight = nullptr;

	m_IsDivided = false;
}

void QuadTree::RegisterNeighbours(SteeringAgent* pAgent, float queryRadius, std::vector<SteeringAgent*>& pNeighbours, int& nrOfNeighbours)
{
	if (m_pAgents.size() == 0)
		return;

	if (m_pAgents.size() == 1 && m_pAgents[0] == pAgent)
		return;

	Elite::Vector2 pos{ pAgent->GetPosition() };
	Elite::Vector2 bottomLeft = { pos.x - queryRadius, pos.y - queryRadius };
	Elite::Rect queryBox{ bottomLeft, queryRadius * 2, queryRadius * 2 };

	//DEBUGRENDERER2D->DrawSegment(Elite::Vector2(queryBox.bottomLeft.x, queryBox.bottomLeft.y), Elite::Vector2(queryBox.bottomLeft.x + queryBox.width, queryBox.bottomLeft.y), { 0,1,0 }, 0.1f);
	//DEBUGRENDERER2D->DrawSegment(Elite::Vector2(queryBox.bottomLeft.x + queryBox.width, queryBox.bottomLeft.y), Elite::Vector2(queryBox.bottomLeft.x + queryBox.width, queryBox.bottomLeft.y + queryBox.height), { 0,1,0 }, 0.1f);
	//DEBUGRENDERER2D->DrawSegment(Elite::Vector2(queryBox.bottomLeft.x + queryBox.width, queryBox.bottomLeft.y + queryBox.height), Elite::Vector2(queryBox.bottomLeft.x, queryBox.bottomLeft.y + queryBox.height), { 0,1,0 }, 0.1f);
	//DEBUGRENDERER2D->DrawSegment(Elite::Vector2(queryBox.bottomLeft.x, queryBox.bottomLeft.y + queryBox.height), Elite::Vector2(queryBox.bottomLeft.x, queryBox.bottomLeft.y), { 0,1,0 }, 0.1f);

	// return if agent query is not overlapping with the current quad
	if (!Elite::IsOverlapping(m_BoundingBox, queryBox))
		return;

	for (SteeringAgent* pCurAgent : m_pAgents)
	{
		if (IsAgentInBounds(queryBox, pCurAgent->GetPosition()))
		{
			if (pCurAgent != pAgent)
			{
				if (std::find(pNeighbours.begin(), pNeighbours.end(), pCurAgent) == pNeighbours.end())
				{
					pNeighbours[nrOfNeighbours] = pCurAgent;
					++nrOfNeighbours;
				}
			}
		}
	}

	if (m_IsDivided)
	{
		m_pTopLeft->RegisterNeighbours(pAgent, queryRadius, pNeighbours, nrOfNeighbours);
		m_pTopRight->RegisterNeighbours(pAgent, queryRadius, pNeighbours, nrOfNeighbours);
		m_pBottomLeft->RegisterNeighbours(pAgent, queryRadius, pNeighbours, nrOfNeighbours);
		m_pBottomRight->RegisterNeighbours(pAgent, queryRadius, pNeighbours, nrOfNeighbours);
	}
}

void QuadTree::Update(SteeringAgent* pAgent)
{
	// check if agent is inside the quad that's being checked
	if (!IsAgentInBounds(m_BoundingBox, pAgent->GetPosition()))
	{
		// check if agent is in the agent vector of the quad, if not return false, if yes erase agent from vector.
		if (std::find(m_pAgents.begin(), m_pAgents.end(), pAgent) == m_pAgents.end())
			return;
		else
			m_pAgents.erase(std::remove(m_pAgents.begin(), m_pAgents.end(), pAgent), m_pAgents.end());

		// if the amount of agents is less than the capacity and the quad is divided, merge the quad
		if (m_pAgents.size() < m_Capacity && m_IsDivided)
			Merge();

		// return false, the agent is not in this quad
		return;
	}

	// check if the quads' agents capacity is reached
	if (m_pAgents.size() < m_Capacity)
	{
		// if no and the quad is divided, merge the quad
		if (m_IsDivided)
			Merge();

		// add the agent to the current quad's agents vector if he is not already in the vector
		if (std::find(m_pAgents.begin(), m_pAgents.end(), pAgent) == m_pAgents.end())
			m_pAgents.push_back(pAgent);

		// return, the agent is in this quad
		return;
	}
	else
	{
		// if yes and the quad is not divided, subdivide the quad
		if (!m_IsDivided)
			Subdivide();

		// Recursively update the active child nodes of the tree
		m_pTopLeft->Update(pAgent);
		m_pTopRight->Update(pAgent);
		m_pBottomLeft->Update(pAgent);
		m_pBottomRight->Update(pAgent);
	}
}

void QuadTree::Render()
{
	float x = m_BoundingBox.bottomLeft.x;
	float y = m_BoundingBox.bottomLeft.y;
	float w = m_BoundingBox.width;
	float h = m_BoundingBox.height;

	DEBUGRENDERER2D->DrawSegment(Elite::Vector2(x, y), Elite::Vector2(x + w, y), { 1,1,1 }, 0.1f);
	DEBUGRENDERER2D->DrawSegment(Elite::Vector2(x + w, y), Elite::Vector2(x + w, y + h), { 1,1,1 }, 0.1f);
	DEBUGRENDERER2D->DrawSegment(Elite::Vector2(x + w, y + h), Elite::Vector2(x, y + h), { 1,1,1 }, 0.1f);
	DEBUGRENDERER2D->DrawSegment(Elite::Vector2(x, y + h), Elite::Vector2(x, y), { 1,1,1 }, 0.1f);

	if (m_IsDivided)
	{
		m_pTopLeft->Render();
		m_pTopRight->Render();
		m_pBottomLeft->Render();
		m_pBottomRight->Render();
	}
}

bool QuadTree::IsAgentInBounds(const Elite::Rect& r, const Elite::Vector2& p)
{
	return (p.x >= r.bottomLeft.x &&
		p.x <= r.bottomLeft.x + r.width &&
		p.y >= r.bottomLeft.y &&
		p.y <= r.bottomLeft.y + r.height);
}