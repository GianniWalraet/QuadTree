#include "stdafx.h"
#include "QuadTree.h"
#include "projects\App_Steering\SteeringAgent.h"

QuadTree::QuadTree(float left, float bottom, float width, float height, size_t capacity)
	: m_BoundingBox{ Elite::Vector2{left, bottom}, width, height }
	, m_Capacity{capacity}
	, m_pAgents{}
	, m_pTopLeft{}
	, m_pTopRight{}
	, m_pBottomLeft{}
	, m_pBottomRight{}
	, m_IsDivided{false}
{
}

bool QuadTree::AddAgent(SteeringAgent* agent)
{
	if (!m_BoundingBox.IsPointInRect(agent->GetPosition()))
		return false;

	if (m_pAgents.size() < m_Capacity)
	{
		m_pAgents.push_back(agent);
		return true;
	}
	else
	{
		if (!m_IsDivided)
			Subdivide();

		if(m_pTopLeft->AddAgent(agent))
			return true;

		if (m_pTopRight->AddAgent(agent))
			return true;

		if (m_pBottomLeft->AddAgent(agent))
			return true;

		if(m_pBottomRight->AddAgent(agent))
			return true;
	}

	return false;
}

void QuadTree::Subdivide()
{
	float x = m_BoundingBox.bottomLeft.x;
	float y = m_BoundingBox.bottomLeft.y;
	float w = m_BoundingBox.width;
	float h = m_BoundingBox.height;

	m_pTopLeft = new QuadTree(x - w/2, y - h/2, w/2, h/2, m_Capacity);
	m_pTopRight = new QuadTree(x + w / 2, y - h / 2, w / 2, h / 2, m_Capacity);
	m_pBottomLeft = new QuadTree(x - w / 2, y + h / 2, w / 2, h / 2, m_Capacity);
	m_pBottomRight = new QuadTree(x + w / 2, y + h / 2, w / 2, h / 2, m_Capacity);

	m_IsDivided = true;
}

void QuadTree::RegisterNeighbours(SteeringAgent* pAgent, float queryRadius, std::vector<SteeringAgent*>& pNeighbours, int& nrOfNeighbours)
{
	Elite::Vector2 pos{ pAgent->GetPosition() };
	Elite::Vector2 bottomLeft = { pos.x - queryRadius, pos.y - queryRadius };
	Elite::Rect queryBox{ bottomLeft, queryRadius * 2, queryRadius * 2 };

	if (!Elite::IsOverlapping(m_BoundingBox, queryBox))
	{
		return;
	}
	else
	{
		for (SteeringAgent* pAgent : m_pAgents)
		{
			if (queryBox.IsPointInRect(pAgent->GetPosition()))
			{
				pNeighbours[nrOfNeighbours] = pAgent;
				++nrOfNeighbours;
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
}

void QuadTree::Render()
{
	float x = m_BoundingBox.bottomLeft.x;
	float y = m_BoundingBox.bottomLeft.y;
	float w = m_BoundingBox.width;
	float h = m_BoundingBox.height;

	DEBUGRENDERER2D->DrawSegment(Elite::Vector2(x - w, y - h), Elite::Vector2(x + w, y - h), { 1,1,1 }, 0.1f);
	DEBUGRENDERER2D->DrawSegment(Elite::Vector2(x + w, y - h), Elite::Vector2(x + w, y + h), { 1,1,1 }, 0.1f);
	DEBUGRENDERER2D->DrawSegment(Elite::Vector2(x + w, y + h), Elite::Vector2(x - w, y + h), { 1,1,1 }, 0.1f);
	DEBUGRENDERER2D->DrawSegment(Elite::Vector2(x - w, y + h), Elite::Vector2(x - w, y - h), { 1,1,1 }, 0.1f);

	if (m_IsDivided)
	{
		m_pTopLeft->Render();
		m_pTopRight->Render();
		m_pBottomLeft->Render();
		m_pBottomRight->Render();
	}
}