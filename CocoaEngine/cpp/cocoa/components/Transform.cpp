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
			data.LocalScale = glm::vec3(1, 1, 1);
			data.LocalEulerRotation = glm::vec3();

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
				const TransformData& parentTransform = NEntity::GetComponent<TransformData>(data.Parent);
				data.Position = parentTransform.Position + data.LocalPosition;
				data.Scale = parentTransform.Scale + data.LocalScale;
				data.EulerRotation = parentTransform.EulerRotation + data.LocalEulerRotation;
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
			json localScale = CMath::Serialize("LocalScale", transform.LocalScale);
			json localRotation = CMath::Serialize("LocalRotation", transform.LocalEulerRotation);
			int size = j["Components"].size();
			j["Components"][size] = {
				{"Transform", {
					{"Entity", NEntity::GetID(entity)},
					position,
					scale,
					rotation,
					{"Parent", NEntity::GetID(transform.Parent)},
					localPos,
					localScale,
					localRotation
				}}
			};
		}

		void Deserialize(const json& j, Entity entity, Entity parent)
		{
			TransformData transform = CreateTransform();
			transform.Position = CMath::DeserializeVec3(j["Transform"]["Position"]);
			transform.Scale = CMath::DeserializeVec3(j["Transform"]["Scale"]);
			transform.EulerRotation = CMath::DeserializeVec3(j["Transform"]["Rotation"]);
			transform.Parent = parent;
			transform.LocalPosition = CMath::DeserializeVec3(j["Transform"]["LocalPosition"]);

			if (j["Transform"].contains("LocalScale"))
			{
				transform.LocalScale = CMath::DeserializeVec3(j["Transform"]["LocalScale"]);
			}

			if (j["Transform"].contains("LocalRotation"))
			{
				transform.LocalEulerRotation = CMath::DeserializeVec3(j["Transform"]["LocalRotation"]);
			}
			NEntity::AddComponent<TransformData>(entity, transform);
		}

		void Serialize(json& j, const TransformData& transform)
		{
			json position = CMath::Serialize("Position", transform.Position);
			json scale = CMath::Serialize("Scale", transform.Scale);
			json rotation = CMath::Serialize("Rotation", transform.EulerRotation);
			json localPos = CMath::Serialize("LocalPosition", transform.LocalPosition);
			int size = j["Components"].size();
			j["Transform"] = {
				position,
				scale,
				rotation,
				{"Parent", NEntity::GetID(transform.Parent)},
				localPos
			};
		}

		void Deserialize(const json& j, TransformData& transform)
		{
			transform.Position = CMath::DeserializeVec3(j["Transform"]["Position"]);
			transform.Scale = CMath::DeserializeVec3(j["Transform"]["Scale"]);
			transform.EulerRotation = CMath::DeserializeVec3(j["Transform"]["Rotation"]);
			transform.LocalPosition = CMath::DeserializeVec3(j["Transform"]["LocalPosition"]);
		}
	}
}