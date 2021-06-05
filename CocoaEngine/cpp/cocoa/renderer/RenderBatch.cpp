#include "externalLibs.h"

#include "cocoa/systems/RenderSystem.h"
#include "cocoa/renderer/Shader.h"
#include "cocoa/core/Application.h"
#include "cocoa/core/AssetManager.h"
#include "cocoa/util/CMath.h"

namespace Cocoa
{
	namespace RenderBatch
	{
		// Forward declarations
		static void LoadVertexProperties(RenderBatchData& data, const TransformData& transform, const SpriteRenderer& spr);

		static void LoadVertexProperties(
			RenderBatchData& data,
			const glm::vec3& position,
			const glm::vec3& scale,
			const glm::vec2* texCoords,
			float rotationDegrees,
			const glm::vec4& color,
			int texId,
			uint32 entityId = -1);

		static void LoadVertexProperties(
			RenderBatchData& data,
			const glm::vec2* vertices,
			const glm::vec2* texCoords,
			const glm::vec4& color,
			const glm::vec2& position,
			int texId,
			int numVertices,
			uint32 entityId = -1);

		static void LoadEmptyVertexProperties(RenderBatchData& data);
		static void LoadElementIndices(RenderBatchData& data, int index);
		static void GenerateIndices(RenderBatchData& data);

		RenderBatchData createRenderBatch(int maxBatchSize, int zIndex, Handle<Shader> shader, bool batchOnTop)
		{
			RenderBatchData data;
			data.numTextures = 0;
			data.numUsedElements = 0;

			data.batchShader = shader;
			data.zIndex = zIndex;
			data.maxBatchSize = maxBatchSize;
			data.vertexBufferBase = (Vertex*)AllocMem(sizeof(Vertex) * data.maxBatchSize);
			data.vertexStackPointer = data.vertexBufferBase;
			data.indices = (uint32*)AllocMem(sizeof(uint32) * data.maxBatchSize * 2);

			for (int i = 0; i < RenderBatch::TEXTURE_SIZE; i++)
			{
				data.textures[i] = {};
			}

			data.vao = UINT32_MAX;
			data.vbo = UINT32_MAX;
			data.ebo = UINT32_MAX;

			data.batchOnTop = batchOnTop;
			return data;
		}

		void free(RenderBatchData& data)
		{
			data.batchShader = NHandle::createHandle<Shader>();
			data.vertexBufferBase = nullptr;
			data.vertexStackPointer = nullptr;
			data.indices = nullptr;
			data.vao = UINT32_MAX;
			data.vbo = UINT32_MAX;
			data.ebo = UINT32_MAX;

			if (data.vertexBufferBase)
			{
				FreeMem(data.vertexBufferBase);
				data.vertexBufferBase = nullptr;
			}
			else
			{
				Logger::Warning("Failed to free render batches vertex data, invalid pointer.");
			}

			if (data.indices)
			{
				FreeMem(data.indices);
				data.indices = nullptr;
			}
			else
			{
				Logger::Warning("Failed to free render batches indices, invalid pointer.");
			}

			if (data.vao != -1)
			{
				glDeleteBuffers(1, &data.vbo);
				glDeleteBuffers(1, &data.ebo);
				glDeleteVertexArrays(1, &data.vao);
			}
			else
			{
				Logger::Warning("Destroyed render batch, but it did not have any valid vao, vbo, or ebo");
			}
		}

		void start(RenderBatchData& data)
		{
			GenerateIndices(data);

			glGenVertexArrays(1, &data.vao);
			glGenBuffers(1, &data.vbo);
			glGenBuffers(1, &data.ebo);

			glBindVertexArray(data.vao);

			glBindBuffer(GL_ARRAY_BUFFER, data.vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * data.maxBatchSize, nullptr, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 2 * data.maxBatchSize, data.indices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, sizeof(Vertex().position) / sizeof(float), GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, position));
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, sizeof(Vertex().color) / sizeof(float), GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, color));
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(2, sizeof(Vertex().texCoords) / sizeof(float), GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
			glEnableVertexAttribArray(2);

			glVertexAttribPointer(3, sizeof(Vertex().texId) / sizeof(float), GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, texId));
			glEnableVertexAttribArray(3);

			// As long as our struct contains 32-bit members only, the second parameter should be correct
			glVertexAttribIPointer(4, sizeof(Vertex().texId) / sizeof(float), GL_UNSIGNED_INT, sizeof(Vertex), (void*)offsetof(Vertex, entityId));
			glEnableVertexAttribArray(4);
		}

		void add(RenderBatchData& data, const TransformData& transform, const SpriteRenderer& spr)
		{
			// 6 elments per sprite
			data.numUsedElements += 6;

			const Sprite& sprite = spr.sprite;
			Handle<Texture> tex = sprite.texture;
			if (!tex.isNull())
			{
				if (!hasTexture(data, tex))
				{
					data.textures[data.numTextures] = tex;
					data.numTextures++;
				}
			}

			LoadVertexProperties(data, transform, spr);
		}

		void add(RenderBatchData& data, const TransformData& transform, const FontRenderer& fontRenderer)
		{
			const Font& font = AssetManager::getFont(fontRenderer.font.assetId);
			Handle<Texture> tex = font.fontTexture;
			const Texture& texture = AssetManager::getTexture(tex.assetId);

			if (!tex.isNull())
			{
				if (!hasTexture(data, tex))
				{
					data.textures[data.numTextures] = tex;
					data.numTextures++;
				}
			}

			int texId = 0;
			for (int i = 0; i < RenderBatch::TEXTURE_SIZE; i++)
			{
				if (data.textures[i] == tex)
				{
					texId = i + 1;
					break;
				}
			}

			Entity res = NEntity::fromComponent<TransformData>(transform);
			uint32 entityId = NEntity::getId(res);

			const std::string& str = fontRenderer.text;
			float x = transform.position.x;
			float y = transform.position.y;
			int strLength = str.size();
			for (int i = 0; i < strLength; i++)
			{
				// 6 elements per sprite
				data.numUsedElements += 6;
				const CharInfo& charInfo = font.getCharacterInfo(str[i]);
				float scaleX = transform.scale.x * fontRenderer.fontSize;
				float scaleY = transform.scale.y * fontRenderer.fontSize;
				float x0 = x + (charInfo.bearingX * scaleX);
				float y0 = y + charInfo.bearingY * scaleY;
				float x1 = x + (charInfo.bearingX * scaleX) + (charInfo.chScaleX * scaleX);
				float y1 = y - (charInfo.chScaleY - charInfo.bearingY) * scaleY;

				glm::vec2 vertices[4] = {
					{x1, y0},
					{x1, y1},
					{x0, y1},
					{x0, y0}
				};

				glm::vec2 texCoords[4] = {
					{charInfo.ux1, charInfo.uy1},
					{charInfo.ux1, charInfo.uy0},
					{charInfo.ux0, charInfo.uy0},
					{charInfo.ux0, charInfo.uy1}
				};
				glm::vec2 quadSize = { charInfo.chScaleX * fontRenderer.fontSize, charInfo.chScaleY * fontRenderer.fontSize };
				//glm::vec3 pos = { xPos, yPos, 0.0f };

				LoadVertexProperties(data, vertices, texCoords, fontRenderer.color, { 0.0f, 0.0f }, texId, 4, entityId);

				x += charInfo.advance * fontRenderer.fontSize * transform.scale.x;
			}
		}

		void add(RenderBatchData& data, const glm::vec2* vertices, const glm::vec3& color, const glm::vec2& position, int numVertices, int numElements)
		{
			data.numUsedElements += numElements;
			std::array<glm::vec2, 4> texCoords{
				glm::vec2 {1, 1},
				glm::vec2 {1, 0},
				glm::vec2 {0, 0},
				glm::vec2 {0, 1}
			};
			glm::vec4 vec4Color{ color.x, color.y, color.z, 1.0f };
			int texId = 0;

			LoadVertexProperties(data, vertices, &texCoords[0], vec4Color, position, texId, numVertices);
		}

		void add(
			RenderBatchData& data,
			Handle<Texture> textureHandle,
			const glm::vec3& position,
			const glm::vec3& scale,
			const glm::vec3& color,
			const glm::vec2& texCoordMin,
			const glm::vec2& texCoordMax,
			float rotation)
		{
			// 6 elements per sprite,
			data.numUsedElements += 6;
			std::array<glm::vec2, 4> texCoords{
				glm::vec2 {texCoordMax.x, texCoordMax.y},
				glm::vec2 {texCoordMax.x, texCoordMin.y},
				glm::vec2 {texCoordMin.x, texCoordMin.y},
				glm::vec2 {texCoordMin.x, texCoordMax.y}
			};
			glm::vec4 vec4Color{ color.x, color.y, color.z, 1.0f };
			glm::vec3 vec3Pos{ position.x, position.y, 0.0f };


			if (!hasTexture(data, textureHandle))
			{
				data.textures[data.numTextures] = textureHandle;
				data.numTextures++;
			}

			int texId = 0;
			for (int i = 0; i < data.numTextures; i++)
			{
				if (data.textures[i] == textureHandle)
				{
					texId = i + 1;
					break;
				}
			}

			LoadVertexProperties(data, position, scale, &texCoords[0], rotation, vec4Color, texId);
		}

		void LoadVertexProperties(RenderBatchData& data, const TransformData& transform, const SpriteRenderer& spr)
		{
			glm::vec4 color = spr.color;
			const Sprite& sprite = spr.sprite;
			const glm::vec2* texCoords = spr.sprite.texCoords;
			float rotation = transform.eulerRotation.z;

			int texId = 0;
			if (!sprite.texture.isNull())
			{
				for (int i = 0; i < RenderBatch::TEXTURE_SIZE; i++)
				{
					if (data.textures[i] == sprite.texture)
					{
						texId = i + 1;
						break;
					}
				}
			}

			Entity res = NEntity::fromComponent<TransformData>(transform);
			LoadVertexProperties(data, transform.position, transform.scale, texCoords, rotation, color, texId, NEntity::getId(res));
		}

		void LoadVertexProperties(
			RenderBatchData& data,
			const glm::vec3& position,
			const glm::vec3& scale,
			const glm::vec2* texCoords,
			float rotationDegrees,
			const glm::vec4& color,
			int texId,
			uint32 entityId)
		{
			bool isRotated = rotationDegrees != 0.0f;
			glm::mat4 matrix = glm::mat4(1.0f);
			if (isRotated)
			{
				matrix = glm::translate(matrix, position);
				matrix = glm::rotate(matrix, glm::radians(rotationDegrees), glm::vec3(0, 0, 1));
				matrix = glm::scale(matrix, scale);
			}

			float xAdd = 0.5f;
			float yAdd = -0.5f;
			for (int i = 0; i < 4; i++)
			{
				if (i == 1)
				{
					yAdd = 0.5f;
				}
				else if (i == 2)
				{
					xAdd = -0.5f;
				}
				else if (i == 3)
				{
					yAdd = -0.5f;
				}

				glm::vec4 currentPos = glm::vec4(position.x + (xAdd * scale.x),
					position.y + (yAdd * scale.y), 0.0f, 1.0f);
				if (isRotated)
				{
					currentPos = matrix * glm::vec4(xAdd, yAdd, 0.0f, 1.0f);
				}

				// Load Attributes
				data.vertexStackPointer->position = glm::vec3(currentPos);
				data.vertexStackPointer->color = glm::vec4(color);
				data.vertexStackPointer->texCoords = glm::vec2(texCoords[i]);
				data.vertexStackPointer->texId = (float)texId;
				data.vertexStackPointer->entityId = entityId;

				data.vertexStackPointer++;
			}
		}

		void LoadVertexProperties(
			RenderBatchData& data,
			const glm::vec2* vertices,
			const glm::vec2* texCoords,
			const glm::vec4& color,
			const glm::vec2& position,
			int texId,
			int numVertices,
			uint32 entityId)
		{
			for (int i = 0; i < numVertices; i++)
			{
				// Load Attributes
				data.vertexStackPointer->position = glm::vec3(vertices[i].x + position.x, vertices[i].y + position.y, 0.0f);
				data.vertexStackPointer->color = glm::vec4(color);
				data.vertexStackPointer->texCoords = glm::vec2(texCoords[i]);
				data.vertexStackPointer->texId = (float)texId;
				data.vertexStackPointer->entityId = entityId;

				data.vertexStackPointer++;
			}
		}

		void LoadEmptyVertexProperties(RenderBatchData& data)
		{
			for (int i = 0; i < 4; i++)
			{
				data.vertexStackPointer = {};
				data.vertexStackPointer++;
			}
		}

		void render(RenderBatchData& data)
		{
			glBindBuffer(GL_ARRAY_BUFFER, data.vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * data.numUsedElements, &data.vertexBufferBase[0]);

			for (int i = 0; i < data.numTextures; i++)
			{
				glActiveTexture(GL_TEXTURE0 + i + 1);
				TextureUtil::bind(AssetManager::getTexture(data.textures[i].assetId));
			}

			glBindVertexArray(data.vao);

			glDrawElements(GL_TRIANGLES, data.numUsedElements, GL_UNSIGNED_INT, 0);

			glBindVertexArray(0);

			for (int i = 0; i < data.numTextures; i++)
			{
				TextureUtil::unbind(AssetManager::getTexture(data.textures[i].assetId));
			}
		}

		void GenerateIndices(RenderBatchData& data)
		{
			for (int i = 0; i < data.maxBatchSize * 2; i++)
			{
				LoadElementIndices(data, i);
			}
		}

		void LoadElementIndices(RenderBatchData& data, int index)
		{
			int offsetArray = 6 * index;
			int offset = 4 * index;

			if (offsetArray + 2 < data.maxBatchSize * 2)
			{
				// Triangle 1
				data.indices[offsetArray] = offset + 3;
				data.indices[offsetArray + 1] = offset + 2;
				data.indices[offsetArray + 2] = offset + 0;
			}

			if (offsetArray + 5 < data.maxBatchSize * 2)
			{
				// Triangle 2
				data.indices[offsetArray + 3] = offset + 0;
				data.indices[offsetArray + 4] = offset + 2;
				data.indices[offsetArray + 5] = offset + 1;
			}
		}

		void clear(RenderBatchData& data)
		{
			data.vertexStackPointer = data.vertexBufferBase;
			data.numUsedElements = 0;
			data.numTextures = 0;
			for (int i = 0; i < data.numTextures; i++)
			{
				data.textures[i] = NHandle::createHandle<Texture>();
			}
		}

		bool hasRoom(const RenderBatchData& data, int numVertices)
		{
			return data.vertexStackPointer + numVertices < data.vertexBufferBase + data.maxBatchSize * 4;
		}

		bool hasRoom(const RenderBatchData& data, const FontRenderer& fontRenderer)
		{
			// 4 Vertices per quad
			return data.numUsedElements + (fontRenderer.text.size() * 4) < data.maxBatchSize;
		}

		bool hasTextureRoom(const RenderBatchData& data)
		{
			return data.numTextures < RenderBatch::TEXTURE_SIZE;
		}

		bool hasTexture(const RenderBatchData& data, Handle<Texture> texture)
		{
			for (int i = 0; i < data.numTextures; i++)
			{
				if (data.textures[i] == texture)
				{
					return true;
				}
			}
			return false;
		}

		bool compare(const RenderBatchData& b1, const RenderBatchData& b2)
		{
			return b1.zIndex < b2.zIndex;
		}
	}
}