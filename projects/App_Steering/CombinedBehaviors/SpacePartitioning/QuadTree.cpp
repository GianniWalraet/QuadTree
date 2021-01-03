#include "stdafx.h"
#include "QuadTree.h"
#include "projects\App_Steering\SteeringAgent.h"

QuadTree::QuadTree(float left, float bottom, float width, float height, int capacity)
	: m_Pos{left, bottom}
	, m_Width{width}
	, m_Height{height}
	, m_Capacity{capacity}
	, m_pAgents{}
	, m_pTopLeft{}
	, m_pTopRight{}
	, m_pBottomLeft{}
	, m_pBottomRight{}
	, m_IsDivided{false}
{

}

void QuadTree::AddAgent(SteeringAgent* agent)
{
	if (!IsAgentInRect(agent))
		return;

	if (m_pAgents.size() < m_Capacity)
	{
		m_pAgents.push_back(agent);
	}
	else
	{
		if (!m_IsDivided)
		{
			Subdivide();
		}

		m_pTopLeft->AddAgent(agent);
		m_pTopRight->AddAgent(agent);
		m_pBottomLeft->AddAgent(agent);
		m_pBottomRight->AddAgent(agent);
	}
}

void QuadTree::Subdivide()
{
	float x = m_Pos.x;
	float y = m_Pos.y;
	float w = m_Width;
	float h = m_Height;

	m_pTopLeft = new QuadTree(x - w/2, y - h/2, w/2, h/2, m_Capacity);
	m_pTopRight = new QuadTree(x + w / 2, y - h / 2, w / 2, h / 2, m_Capacity);
	m_pBottomLeft = new QuadTree(x - w / 2, y + h / 2, w / 2, h / 2, m_Capacity);
	m_pBottomRight = new QuadTree(x + w / 2, y + h / 2, w / 2, h / 2, m_Capacity);

	m_IsDivided = true;
}

bool QuadTree::IsAgentInRect(SteeringAgent* agent)
{
	Elite::Vector2 agentPos{ agent->GetPosition() };

	return ((agentPos.x > (m_Pos.x - m_Width)) &&
		(agentPos.x < (m_Pos.x + m_Width)) &&
		(agentPos.y > (m_Pos.y - m_Height)) &&
		(agentPos.y < (m_Pos.y + m_Height)));
}

void QuadTree::Render()
{
	DEBUGRENDERER2D->DrawSegment(Elite::Vector2(m_Pos.x - m_Width, m_Pos.y - m_Height), Elite::Vector2(m_Pos.x + m_Width, m_Pos.y - m_Height), { 0,1,0 }, 0.1f);
	DEBUGRENDERER2D->DrawSegment(Elite::Vector2(m_Pos.x - m_Width, m_Pos.y - m_Height), Elite::Vector2(m_Pos.x - m_Width, m_Pos.y + m_Height), { 0,1,0 }, 0.1f);

	if (m_IsDivided)
	{
		m_pTopLeft->Render();
		m_pTopRight->Render();
		m_pBottomLeft->Render();
		m_pBottomRight->Render();
	}
}