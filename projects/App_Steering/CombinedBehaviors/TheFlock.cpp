#include "stdafx.h"
#include "TheFlock.h"

#include "../SteeringAgent.h"
#include "../SteeringBehaviors.h"
#include "CombinedSteeringBehaviors.h"
#include "SpacePartitioning/QuadTree.h"

using namespace Elite;

//Constructor & Destructor
Flock::Flock(
	int flockSize /*= 50*/, 
	float worldSize /*= 100.f*/, 
	SteeringAgent* pAgentToEvade /*= nullptr*/, 
	bool trimWorld /*= false*/)

	: m_WorldSize{ worldSize }
	, m_FlockSize{ flockSize }
	, m_TrimWorld { trimWorld }
	, m_pAgentToEvade{pAgentToEvade}
	, m_NeighborhoodRadius{ 15 }
	, m_NrOfNeighbors{0}
{
	//SteeringBehaviors
	m_pCohesion = new Cohesion(this);
	m_pSeperation = new Seperation(this);
	m_pAllignment = new Allignment(this);
	m_pSeek = new Seek();
	m_pWander = new Wander();
	m_pBlendedSteering = new BlendedSteering({ {m_pCohesion, 0.2f}, {m_pSeperation, 0.2f}, {m_pAllignment, 0.2f}, {m_pSeek, 0.2f}, {m_pWander, 0.2f} });

	m_pEvade = new Evade();
	m_pPrioritySteering = new PrioritySteering({ m_pEvade, m_pBlendedSteering });

	// Resize Vectors (neighbours -> memorypool)
	m_pNeighbors.resize(m_FlockSize - 1);
	//m_OldPositions.resize(m_FlockSize);
	m_pAgents.resize(m_FlockSize);

	// QuadTree
	m_pQuadTree = new QuadTree(0, 0, m_WorldSize, m_WorldSize, 4);
	
	// Initialize Agents
	Elite::Vector2 randomPos{};
	for (int i = 0; i < m_FlockSize; i++)
	{
		randomPos.x = randomFloat(m_WorldSize);
		randomPos.y = randomFloat(m_WorldSize);
		m_pAgents[i] = new SteeringAgent();
		m_pAgents[i]->SetSteeringBehavior(m_pPrioritySteering);
		m_pAgents[i]->SetPosition(randomPos);
		m_pAgents[i]->SetMaxLinearSpeed(50.f);
		m_pAgents[i]->SetAutoOrient(true);
		m_pAgents[i]->SetMass(1.f);

		m_pQuadTree->AddAgent(m_pAgents[i]);
	}

	// Initialize AgentToEvade
	randomPos.x = randomFloat(m_WorldSize);
	randomPos.y = randomFloat(m_WorldSize);
	m_pAgentToEvade = new SteeringAgent();
	m_pAgentToEvade->SetSteeringBehavior(m_pWander);
	m_pAgentToEvade->SetPosition(randomPos);
	m_pAgentToEvade->SetMaxLinearSpeed(50.f);
	m_pAgentToEvade->SetAutoOrient(true);
	m_pAgentToEvade->SetMass(1.f);
	m_pAgentToEvade->SetBodyColor({ 0,0,1 });
}

Flock::~Flock()
{
	SAFE_DELETE(m_pCohesion);
	SAFE_DELETE(m_pSeperation);
	SAFE_DELETE(m_pAllignment);
	SAFE_DELETE(m_pSeek);
	SAFE_DELETE(m_pWander);
	SAFE_DELETE(m_pEvade);
	SAFE_DELETE(m_pBlendedSteering);
	SAFE_DELETE(m_pPrioritySteering);
	SAFE_DELETE(m_pAgentToEvade);
	SAFE_DELETE(m_pQuadTree);

	for (size_t i = 0; i < m_pAgents.size(); i++)
	{
		SAFE_DELETE(m_pAgents[i]);
	}
}

void Flock::Update(float deltaT, const TargetData& target)
{
	// loop over all the boids
	// register its neighbors
	// update it
	// trim it to the world

	m_pSeek->SetTarget(target);

	for (SteeringAgent* pAgent : m_pAgents)
	{
		m_pQuadTree->Update(pAgent);

		m_NrOfNeighbors = 0;
		m_pQuadTree->RegisterNeighbours(pAgent, m_NeighborhoodRadius, m_pNeighbors, m_NrOfNeighbors);

		pAgent->Update(deltaT);
		pAgent->TrimToWorld({0,0}, {m_WorldSize, m_WorldSize});
	}
	
	m_pEvade->SetTarget(m_pAgentToEvade->GetPosition());
	m_pAgentToEvade->Update(deltaT);
	m_pAgentToEvade->TrimToWorld({ 0,0 }, { m_WorldSize, m_WorldSize });
}

void Flock::Render(float deltaT)
{
	// DEBUG
	if (m_CanDebugRender)
	{
		DEBUGRENDERER2D->DrawCircle(m_pAgents[0]->GetPosition(), m_NeighborhoodRadius, { 0,0,1 }, -0.5f);
		m_pAgents[0]->Render(deltaT);
	}
	m_pAgents[0]->SetRenderBehavior(m_CanDebugRender);

	if (m_RenderQuadTree)
		m_pQuadTree->Render();
	////////////
}

void Flock::UpdateAndRenderUI()
{
	//Setup
	int menuWidth = 235;
	int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
	int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
	bool windowActive = true;
	ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
	ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 20));
	ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::PushAllowKeyboardFocus(false);

	//Elements
	ImGui::Text("CONTROLS");
	ImGui::Indent();
	ImGui::Text("LMB: place target");
	ImGui::Text("RMB: move cam.");
	ImGui::Text("Scrollwheel: zoom cam.");
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("STATS");
	ImGui::Indent();
	ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Flocking");
	ImGui::Spacing();

	// Implement checkboxes and sliders here
	ImGui::Checkbox("Debug Rendering", &m_CanDebugRender);
	ImGui::Checkbox("Debug QuadTree", &m_RenderQuadTree);
	/*ImGui::Checkbox("Trim World", &m_TrimWorld);
	if (m_TrimWorld)
	{
		ImGui::SliderFloat("Trim Size", &m_WorldSize, 0.f, 500.f, "%1.");
	}*/
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("Behavior Weights");
	ImGui::Spacing();

	ImGui::SliderFloat("Cohesion", &m_pBlendedSteering->m_WeightedBehaviors[0].weight, 0.f, 1.f, "%.2f");
	ImGui::SliderFloat("Seperation", &m_pBlendedSteering->m_WeightedBehaviors[1].weight, 0.f, 1.f, "%.2f");
	ImGui::SliderFloat("Allignment", &m_pBlendedSteering->m_WeightedBehaviors[2].weight, 0.f, 1.f, "%.2f");
	ImGui::SliderFloat("Seek", &m_pBlendedSteering->m_WeightedBehaviors[3].weight, 0.f, 1.f, "%.2f");
	ImGui::SliderFloat("Wander", &m_pBlendedSteering->m_WeightedBehaviors[4].weight, 0.f, 1.f, "%.2f");
	
	//End
	ImGui::PopAllowKeyboardFocus();
	ImGui::End();
	
}

Elite::Vector2 Flock::GetAverageNeighborPos() const
{
	Elite::Vector2 positionsSum{};

	if (m_NrOfNeighbors == 0)
		return positionsSum;

	for (int i = 0; i < m_NrOfNeighbors; i++)
		positionsSum += m_pNeighbors[i]->GetPosition();

	return (positionsSum / float(m_NrOfNeighbors));
}

Elite::Vector2 Flock::GetAverageNeighborVelocity() const
{
	Elite::Vector2 velocitiesSum{};

	if (m_NrOfNeighbors == 0)
		return velocitiesSum;

	for (int i = 0; i < m_NrOfNeighbors; i++)
		velocitiesSum += m_pNeighbors[i]->GetLinearVelocity();

	return (velocitiesSum / float(m_NrOfNeighbors));
}


float* Flock::GetWeight(ISteeringBehavior* pBehavior)
{
	if (m_pBlendedSteering)
	{
		auto& weightedBehaviors = m_pBlendedSteering->m_WeightedBehaviors;
		auto it = find_if(weightedBehaviors.begin(),
			weightedBehaviors.end(),
			[pBehavior](BlendedSteering::WeightedBehavior el)
			{
				return el.pBehavior == pBehavior;
			}
		);

		if(it!= weightedBehaviors.end())
			return &it->weight;
	}

	return nullptr;
}
