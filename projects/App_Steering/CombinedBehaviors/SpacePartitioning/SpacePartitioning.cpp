#include "stdafx.h"
#include "SpacePartitioning.h"
#include "projects\App_Steering\SteeringAgent.h"

// --- Cell ---
// ------------
Cell::Cell(float left, float bottom, float width, float height)
{
	boundingBox.bottomLeft = { left, bottom };
	boundingBox.width = width;
	boundingBox.height = height;
}

std::vector<Elite::Vector2> Cell::GetRectPoints() const
{
	auto left = boundingBox.bottomLeft.x;
	auto bottom = boundingBox.bottomLeft.y;
	auto width = boundingBox.width;
	auto height = boundingBox.height;

	std::vector<Elite::Vector2> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(float width, float height, int rows, int cols, int maxEntities)
	: m_SpaceWidth(width)
	, m_SpaceHeight(height)
	, m_NrOfRows(rows)
	, m_NrOfCols(cols)
	, m_pNeighbors(maxEntities)
	, m_NrOfNeighbors(0)
	, m_CellWidth(width / cols)
	, m_CellHeight(height / rows)
	, m_OverlappedCellsIdx()
{
	m_pNeighbors.resize(maxEntities - 1);

	for (int i = 0; i < m_NrOfRows; i++)
	{
		for (int j = 0; j < m_NrOfCols; j++)
		{
			m_Cells.push_back({ m_CellWidth * j, m_CellHeight * i, m_CellWidth, m_CellHeight });
		}
	}
}

void CellSpace::AddAgent(SteeringAgent* agent)
{
	m_Cells[PositionToIndex(agent->GetPosition())].agents.push_back(agent);
}

void CellSpace::UpdateAgentCell(SteeringAgent* agent, const Elite::Vector2& oldPos)
{
	int oldIdx = PositionToIndex(oldPos);
	int newIdx = PositionToIndex(agent->GetPosition());

	if (oldIdx != newIdx)
	{
		m_Cells[newIdx].agents.push_back(agent);
		m_Cells[oldIdx].agents.erase(std::remove(m_Cells[oldIdx].agents.begin(), m_Cells[oldIdx].agents.end(), agent), m_Cells[oldIdx].agents.end());
	}
}

void CellSpace::RegisterNeighbors(SteeringAgent* pAgent, SteeringAgent* pFirstAgent, float queryRadius)
{
	m_NrOfNeighbors = 0;

	Elite::Vector2 pos{ pAgent->GetPosition() };
	Elite::Vector2 bottomLeft = { pos.x - queryRadius, pos.y - queryRadius };
	Elite::Rect queryBox{ bottomLeft, queryRadius * 2, queryRadius * 2 };

	int rowsToCheck{ 3 };
	int colsToCheck{ 3 };
	int index{};
	int bottomLeftIdx = PositionToIndex(pos) - (m_NrOfCols * int(rowsToCheck / 2)) - int(colsToCheck / 2);

	if (pAgent == pFirstAgent)
		m_OverlappedCellsIdx.clear();

	for (int i = 0; i < rowsToCheck; i++)
	{
		index = bottomLeftIdx + (m_NrOfCols * i);

		for (int i = 0; i < colsToCheck; i++)
		{
			if (((index) < (m_NrOfCols * m_NrOfRows)) && (index >= 0))
			{
				if (Elite::IsOverlapping(m_Cells[index].boundingBox, queryBox))
				{
					if (m_Cells[index].agents.size() > 0)
					{
						for (SteeringAgent* agent : m_Cells[index].agents)
						{
							if (pos != agent->GetPosition())
							{
								auto distanceToAgent = Distance(pos, agent->GetPosition());

								if (distanceToAgent < queryRadius)
								{
									m_pNeighbors[m_NrOfNeighbors] = agent;
									++m_NrOfNeighbors;
								}
							}
						}
					}
					// DEBUG
					if (pAgent == pFirstAgent)
						m_OverlappedCellsIdx.push_back(index);
					/////////
				}
			}
			++index;
		}
	}
}

void CellSpace::RenderCells(SteeringAgent* pFirstAgent, float queryRadius) const
{
	for (const Cell& element : m_Cells)
	{
		DEBUGRENDERER2D->DrawSegment(element.GetRectPoints()[0], element.GetRectPoints()[1], { 1,0,0 }, 0.3f);
		DEBUGRENDERER2D->DrawSegment(element.GetRectPoints()[1], element.GetRectPoints()[2], { 1,0,0 }, 0.3f);
	}

	Elite::Vector2 pos{ pFirstAgent->GetPosition() };
	Elite::Vector2 bottomLeft = { pos.x - queryRadius, pos.y - queryRadius };
	Elite::Rect boundingBox{ bottomLeft, queryRadius * 2, queryRadius * 2 };

	DEBUGRENDERER2D->DrawSegment({ bottomLeft.x, bottomLeft.y }, { bottomLeft.x, bottomLeft.y + queryRadius * 2 }, { 0,0,1 });
	DEBUGRENDERER2D->DrawSegment({ bottomLeft.x, bottomLeft.y + queryRadius * 2 }, { bottomLeft.x + queryRadius * 2, bottomLeft.y + queryRadius * 2 }, { 0,0,1 });
	DEBUGRENDERER2D->DrawSegment({ bottomLeft.x + queryRadius * 2, bottomLeft.y + queryRadius * 2 }, { bottomLeft.x + queryRadius * 2, bottomLeft.y }, { 0,0,1 });
	DEBUGRENDERER2D->DrawSegment({ bottomLeft.x + queryRadius * 2, bottomLeft.y }, { bottomLeft.x, bottomLeft.y }, { 0,0,1 });

	for (const int idx : m_OverlappedCellsIdx)
	{
		DEBUGRENDERER2D->DrawSegment(m_Cells[idx].GetRectPoints()[0], m_Cells[idx].GetRectPoints()[1], { 0,1,0 }, 0.1f);
		DEBUGRENDERER2D->DrawSegment(m_Cells[idx].GetRectPoints()[1], m_Cells[idx].GetRectPoints()[2], { 0,1,0 }, 0.1f);
		DEBUGRENDERER2D->DrawSegment(m_Cells[idx].GetRectPoints()[2], m_Cells[idx].GetRectPoints()[3], { 0,1,0 }, 0.1f);
		DEBUGRENDERER2D->DrawSegment(m_Cells[idx].GetRectPoints()[3], m_Cells[idx].GetRectPoints()[0], { 0,1,0 }, 0.1f);
	}
}

int CellSpace::PositionToIndex(const Elite::Vector2 pos) const
{
	int index{};

	if ((pos.x < m_SpaceWidth && pos.x > 0.f) && (pos.y < m_SpaceHeight && pos.y > 0.f))
	{
		int xIdx = int(pos.x / m_SpaceWidth * m_NrOfCols);
		int yIdx = int(pos.y / m_SpaceHeight * m_NrOfRows);

		index = xIdx + (yIdx * m_NrOfCols);
	}

	return index;
}