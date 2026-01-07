// World/World.cpp
#include "World.h"
#include "Common/EngineSettings.h"

/* default */
/////////////////////////////////////////////////////////////////

World::World()
	: m_actors(),
	  m_mainCamera(std::make_unique<Camera>())
{
} // World

World::~World() {}


bool World::Init(ID3D11Device* device, ID3D11DeviceContext* context,
    ModelManager* modelManager, TexturesManager* texManager, int screenWidth, int screenHeight)
{
    m_mainCamera = std::make_unique<Camera>();
    m_mainCamera->InitProjection(screenWidth, screenHeight,
		EngineSettings::SCREEN_NEAR, EngineSettings::SCREEN_DEPTH);

    Model* stoneModel = modelManager->GetModel(device, context, texManager, EngineSettings::STONE_PATH);

    if (stoneModel)
    {
        auto stone1 = std::make_unique<Stone>();
        stone1->Init(stoneModel, "FirstStone");
        stone1->GetTransform()->SetPosition(0.0f, 0.0f, 5.0f);

        auto stone2 = std::make_unique<Stone>();
        stone2->Init(stoneModel, "SecondStone");
        stone2->GetTransform()->SetPosition(-2.0f, 0.0f, 10.0f);

        m_actors.push_back(std::move(stone1));
        m_actors.push_back(std::move(stone2));
    }

    return true;
} // Init


void World::Frame(float frameTime, bool canControlWorld)
{
    if (canControlWorld)
    {
		// TODO: 카메라 제어 코드 추가 예정
    }

    for (auto& actor : m_actors)
    {
        actor->Frame(frameTime);
    }
} // Frame


void World::Render(ID3D11DeviceContext* context, Shader* shader)
{
  
    XMMATRIX view = m_mainCamera->GetViewMatrix();
    XMMATRIX projectiom = m_mainCamera->GetProjectionMatrix();

    for (auto& actor : m_actors)
    {
        actor->Render(context, shader, view, projectiom);
    }
} // Render

/////////////////////////////////////////////////////////////////


/* get */
/////////////////////////////////////////////////////////////////

const std::vector<std::unique_ptr<ActorObject>>& World::GetActors() const
{ 
    return m_actors; 
} // GetActors


ActorObject* World::GetActor(size_t index) const
{
    if (index < m_actors.size()) 
        return m_actors[index].get();
    return nullptr;
} // GetActor