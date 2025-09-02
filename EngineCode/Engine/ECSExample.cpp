#include "ECS.h"
/*
// ������ �����������
struct TransformComponent
{
	glm::vec3 position{0.0f};
	glm::vec3 rotation{0.0f};
	glm::vec3 scale{1.0f};
};

struct MeshComponent
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};

struct PhysicsComponent
{
	glm::vec3 velocity{0.0f};
	glm::vec3 acceleration{0.0f};
	float mass = 1.0f;
};

// ������ �������
class PhysicsSystem : public System
{
  public:
	Coordinator coordinator;

	PhysicsSystem()
	{
		requireComponent<TransformComponent>();
		requireComponent<PhysicsComponent>();
	}

	void update(float deltaTime) override
	{
		auto entities = coordinator.getEntitiesForSystem(this);

		for (auto& entity : entities)
		{
			auto& transform = entity.getComponent<TransformComponent>();
			auto& physics = entity.getComponent<PhysicsComponent>();

			// ��������� ������
			physics.velocity += physics.acceleration * deltaTime;
			transform.position += physics.velocity * deltaTime;
		}
	}
};

// ������������� � ����
int main()
{
	Coordinator coordinator;

	// ������������ ����������
	coordinator.registerComponent<TransformComponent>();
	coordinator.registerComponent<MeshComponent>();
	coordinator.registerComponent<PhysicsComponent>();

	// ������������ �������
	auto physicsSystem = coordinator.registerSystem<PhysicsSystem>();
	Signature physicsSignature;
	physicsSignature.set(Component<TransformComponent>::getTypeId());
	physicsSignature.set(Component<PhysicsComponent>::getTypeId());
	coordinator.setSystemSignature<PhysicsSystem>(physicsSignature);

	// ������� ��������
	EntityObject player = coordinator.createEntity();
	player.addComponent<TransformComponent>();
	player.addComponent<PhysicsComponent>(glm::vec3(0.0f), glm::vec3(0.0f, -9.8f, 0.0f), 1.0f);

	// ������� ����
	while (running)
	{
		float deltaTime = getDeltaTime();
		coordinator.updateSystems(deltaTime);

		// ��������� ����������
		if (player.hasComponent<TransformComponent>())
		{
			auto& transform = player.getComponent<TransformComponent>();
			std::cout << "Player position: " << transform.position.x << ", " << transform.position.y << ", "
					  << transform.position.z << std::endl;
		}
	}

	return 0;
}
*/