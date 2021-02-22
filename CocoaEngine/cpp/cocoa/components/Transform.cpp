#include "cocoa/components/Transform.h"
#include "cocoa/util/CMath.h"

namespace Cocoa
{
	namespace Transform
	{
		// Internal Functions
		static TransformData Init(glm::vec3 position, glm::vec3 scale, glm::vec3 eulerRotation, Entity parent = NEntity::CreateNull())
		{
			TransformData data;
			data.Position = position;
			data.Scale = scale;
			data.EulerRotation = eulerRotation;
			data.Orientation = glm::toQuat(glm::orientate3(data.EulerRotation));
			data.Parent = parent;
			data.LocalPosition = glm::vec3();

			return data;
		}

		TransformData CreateTransform()
		{
			return Init(glm::vec3(0), glm::vec3(1), glm::vec3(0));
		}

		TransformData CreateTransform(glm::vec3 position, glm::vec3 scale, glm::vec3 eulerRotation)
		{
			return Init(position, scale, eulerRotation);
		}

		void Update(TransformData& data, float dt)
		{
			if (!NEntity::IsNull(data.Parent))
			{
				// TODO: This logic is probably flawed because it assumes that the parent is updated before the child
				const TransformData& parentPos = NEntity::GetComponent<TransformData>(data.Parent);
				data.Position = parentPos.Position + data.LocalPosition;
			}

			data.ModelMatrix = glm::translate(glm::mat4(1.0f), data.Position);
			data.ModelMatrix = data.ModelMatrix * glm::toMat4(data.Orientation);
			data.ModelMatrix = glm::scale(data.ModelMatrix, data.Scale);
		}

		void Serialize(json& j, Entity entity, const TransformData& transform)
		{
			json position = CMath::Serialize("Position", transform.Position);
			json scale = CMath::Serialize("Scale", transform.Scale);
			json rotation = CMath::Serialize("Rotation", transform.EulerRotation);
			json localPos = CMath::Serialize("LocalPosition", transform.LocalPosition);
			int size = j["Components"].size();
			j["Components"][size] = {
				{"Transform", {
					{"Entity", NEntity::GetID(entity)},
					position,
					scale,
					rotation,
					{"Parent", NEntity::GetID(transform.Parent)},
					localPos
				}}
			};
		}
		void Deserialize(json& j, Entity entity, Entity parent)
		{
			TransformData transform;
			transform.Position = CMath::DeserializeVec3(j["Transform"]["Position"]);
			transform.Scale = CMath::DeserializeVec3(j["Transform"]["Scale"]);
			transform.EulerRotation = CMath::DeserializeVec3(j["Transform"]["Rotation"]);
			transform.Parent = parent;
			transform.LocalPosition = CMath::DeserializeVec3(j["Transform"]["LocalPosition"]);
			NEntity::AddComponent<TransformData>(entity, transform);
		}
	}
}