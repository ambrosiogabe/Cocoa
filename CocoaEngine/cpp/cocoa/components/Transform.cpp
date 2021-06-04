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
			data.position = position;
			data.scale = scale;
			data.eulerRotation = eulerRotation;
			data.orientation = glm::toQuat(glm::orientate3(data.eulerRotation));
			data.parent = parent;
			data.localPosition = glm::vec3();
			data.localScale = glm::vec3(1, 1, 1);
			data.localEulerRotation = glm::vec3();

			return data;
		}

		TransformData createTransform()
		{
			return Init(glm::vec3(0), glm::vec3(1), glm::vec3(0));
		}

		TransformData createTransform(glm::vec3 position, glm::vec3 scale, glm::vec3 eulerRotation)
		{
			return Init(position, scale, eulerRotation);
		}

		void update(TransformData& data, float dt)
		{
			if (!NEntity::IsNull(data.parent))
			{
				// TODO: This logic is probably flawed because it assumes that the parent is updated before the child
				const TransformData& parentTransform = NEntity::GetComponent<TransformData>(data.parent);
				data.position = parentTransform.position + data.localPosition;
				data.scale = parentTransform.scale + data.localScale;
				data.eulerRotation = parentTransform.eulerRotation + data.localEulerRotation;
			}

			data.modelMatrix = glm::translate(glm::mat4(1.0f), data.position);
			data.modelMatrix = data.modelMatrix * glm::toMat4(data.orientation);
			data.modelMatrix = glm::scale(data.modelMatrix, data.scale);
		}

		void serialize(json& j, Entity entity, const TransformData& transform)
		{
			json position = CMath::Serialize("Position", transform.position);
			json scale = CMath::Serialize("Scale", transform.scale);
			json rotation = CMath::Serialize("Rotation", transform.eulerRotation);
			json localPos = CMath::Serialize("LocalPosition", transform.localPosition);
			json localScale = CMath::Serialize("LocalScale", transform.localScale);
			json localRotation = CMath::Serialize("LocalRotation", transform.localEulerRotation);
			int size = j["Components"].size();
			j["Components"][size] = {
				{"Transform", {
					{"Entity", NEntity::GetID(entity)},
					position,
					scale,
					rotation,
					{"Parent", NEntity::GetID(transform.parent)},
					localPos,
					localScale,
					localRotation
				}}
			};
		}

		void deserialize(const json& j, Entity entity, Entity parent)
		{
			TransformData transform = createTransform();
			transform.position = CMath::DeserializeVec3(j["Transform"]["Position"]);
			transform.scale = CMath::DeserializeVec3(j["Transform"]["Scale"]);
			transform.eulerRotation = CMath::DeserializeVec3(j["Transform"]["Rotation"]);
			transform.parent = parent;
			transform.localPosition = CMath::DeserializeVec3(j["Transform"]["LocalPosition"]);

			if (j["Transform"].contains("LocalScale"))
			{
				transform.localScale = CMath::DeserializeVec3(j["Transform"]["LocalScale"]);
			}

			if (j["Transform"].contains("LocalRotation"))
			{
				transform.localEulerRotation = CMath::DeserializeVec3(j["Transform"]["LocalRotation"]);
			}
			NEntity::AddComponent<TransformData>(entity, transform);
		}

		void serialize(json& j, const TransformData& transform)
		{
			json position = CMath::Serialize("Position", transform.position);
			json scale = CMath::Serialize("Scale", transform.scale);
			json rotation = CMath::Serialize("Rotation", transform.eulerRotation);
			json localPos = CMath::Serialize("LocalPosition", transform.localPosition);
			int size = j["Components"].size();
			j["Transform"] = {
				position,
				scale,
				rotation,
				{"Parent", NEntity::GetID(transform.parent)},
				localPos
			};
		}

		void deserialize(const json& j, TransformData& transform)
		{
			transform.position = CMath::DeserializeVec3(j["Transform"]["Position"]);
			transform.scale = CMath::DeserializeVec3(j["Transform"]["Scale"]);
			transform.eulerRotation = CMath::DeserializeVec3(j["Transform"]["Rotation"]);
			transform.localPosition = CMath::DeserializeVec3(j["Transform"]["LocalPosition"]);
		}
	}
}