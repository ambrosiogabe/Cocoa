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

			UpdateMatrices(data);
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

		void UpdateMatrices(TransformData& data)
		{
			data.ModelMatrix = glm::translate(glm::mat4(1.0f), data.Position);
			data.ModelMatrix = data.ModelMatrix * glm::toMat4(data.Orientation);
			data.ModelMatrix = glm::scale(data.ModelMatrix, data.Scale);
		}

		void Serialize(json& j, Entity entity, const TransformData& transform)
		{
			json position = CMath::Serialize("Position", transform.Position);
			json scale = CMath::Serialize("Scale", transform.Scale);
			json rotation = CMath::Serialize("Rotation", transform.EulerRotation);
			int size = j["Components"].size();
			j["Components"][size] = {
				{"Transform", {
					{"Entity", NEntity::GetID(entity)},
					position,
					scale,
					rotation
				}}
			};
		}
		void Deserialize(json& j, Entity entity)
		{
			TransformData transform;
			transform.Position = CMath::DeserializeVec3(j["Transform"]["Position"]);
			transform.Scale = CMath::DeserializeVec3(j["Transform"]["Scale"]);
			transform.EulerRotation = CMath::DeserializeVec3(j["Transform"]["Rotation"]);
			// TODO: Make this really work
			transform.Parent = NEntity::CreateNull();
			NEntity::AddComponent<TransformData>(entity, transform);
		}
	}
}