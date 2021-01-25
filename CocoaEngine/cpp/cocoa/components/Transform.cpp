#include "cocoa/components/Transform.h"
#include "cocoa/core/Application.h"
#include "cocoa/util/CMath.h"

namespace Cocoa
{
	void Transform::Serialize(json& j, Entity entity, const Transform& transform)
	{
		json position = CMath::Serialize("Position", transform.m_Position);
		json scale = CMath::Serialize("Scale", transform.m_Scale);
		json rotation = CMath::Serialize("Rotation", transform.m_EulerRotation);
		int size = j["Components"].size();
		j["Components"][size] = {
			{"Transform", {
				{"Entity", entity.GetID()},
				position,
				scale,
				rotation
			}}
		};
	}
	void Transform::Deserialize(json& j, Entity entity)
	{
		Transform transform;
		transform.m_Position = CMath::DeserializeVec3(j["Transform"]["Position"]);
		transform.m_Scale = CMath::DeserializeVec3(j["Transform"]["Scale"]);
		transform.m_EulerRotation = CMath::DeserializeVec3(j["Transform"]["Rotation"]);
		entity.AddComponent<Transform>(transform);
	}
}