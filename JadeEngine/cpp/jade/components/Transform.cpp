#include "jade/components/Transform.h"
#include "jade/core/Application.h"
#include "jade/util/JMath.h"

namespace Jade
{
	void Transform::Serialize(entt::entity entity, const Transform& transform)
	{
		json& j = Application::Get()->GetScene()->GetSaveDataJson();

		json position = JMath::Serialize("Position", transform.m_Position);
		json scale = JMath::Serialize("Scale", transform.m_Scale);
		json rotation = JMath::Serialize("Rotation", transform.m_EulerRotation);
		int size = j["Size"];
		j["Components"][size] = {
			{"Transform", {
				{"Entity", entt::to_integral(entity)},
				position,
				scale,
				rotation
			}}
		};

		j["Size"] = size + 1;
	}
	void Transform::Deserialize(json& j, entt::registry& registry, entt::entity entity)
	{
		Transform transform;
		transform.m_Position = JMath::DeserializeVec3(j["Transform"]["Position"]);
		transform.m_Scale = JMath::DeserializeVec3(j["Transform"]["Scale"]);
		transform.m_EulerRotation = JMath::DeserializeVec3(j["Transform"]["Rotation"]);
		registry.emplace<Transform>(entity, transform);
	}
}