#include "cocoa/components/Transform.h"
#include "cocoa/util/CMath.h"

namespace Cocoa
{
	namespace Transform
	{
		// Internal Functions
		static TransformData Init(glm::vec3 position, glm::vec3 scale, glm::vec3 eulerRotation, Entity parent = NEntity::createNull())
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
			if (!NEntity::isNull(data.parent))
			{
				// TODO: This logic is probably flawed because it assumes that the parent is updated before the child
				const TransformData& parentTransform = NEntity::getComponent<TransformData>(data.parent);
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
			json position = CMath::serialize("Position", transform.position);
			json scale = CMath::serialize("Scale", transform.scale);
			json rotation = CMath::serialize("Rotation", transform.eulerRotation);
			json localPos = CMath::serialize("LocalPosition", transform.localPosition);
			json localScale = CMath::serialize("LocalScale", transform.localScale);
			json localRotation = CMath::serialize("LocalRotation", transform.localEulerRotation);
			int size = j["Components"].size();
			j["Components"][size] = {
				{"Transform", {
					{"Entity", NEntity::getId(entity)},
					position,
					scale,
					rotation,
					{"Parent", NEntity::getId(transform.parent)},
					localPos,
					localScale,
					localRotation
				}}
			};
		}

		void deserialize(const json& j, Entity entity, Entity parent)
		{
			TransformData transform = createTransform();
			transform.position = CMath::deserializeVec3(j["Transform"]["Position"]);
			transform.scale = CMath::deserializeVec3(j["Transform"]["Scale"]);
			transform.eulerRotation = CMath::deserializeVec3(j["Transform"]["Rotation"]);
			transform.parent = parent;
			transform.localPosition = CMath::deserializeVec3(j["Transform"]["LocalPosition"]);

			if (j["Transform"].contains("LocalScale"))
			{
				transform.localScale = CMath::deserializeVec3(j["Transform"]["LocalScale"]);
			}

			if (j["Transform"].contains("LocalRotation"))
			{
				transform.localEulerRotation = CMath::deserializeVec3(j["Transform"]["LocalRotation"]);
			}
			NEntity::addComponent<TransformData>(entity, transform);
		}

		void serialize(json& j, const TransformData& transform)
		{
			json position = CMath::serialize("Position", transform.position);
			json scale = CMath::serialize("Scale", transform.scale);
			json rotation = CMath::serialize("Rotation", transform.eulerRotation);
			json localPos = CMath::serialize("LocalPosition", transform.localPosition);
			int size = j["Components"].size();
			j["Transform"] = {
				position,
				scale,
				rotation,
				{"Parent", NEntity::getId(transform.parent)},
				localPos
			};
		}

		void deserialize(const json& j, TransformData& transform)
		{
			transform.position = CMath::deserializeVec3(j["Transform"]["Position"]);
			transform.scale = CMath::deserializeVec3(j["Transform"]["Scale"]);
			transform.eulerRotation = CMath::deserializeVec3(j["Transform"]["Rotation"]);
			transform.localPosition = CMath::deserializeVec3(j["Transform"]["LocalPosition"]);
		}
	}
}