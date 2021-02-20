#include "externalLibs.h"

#include "cocoa/systems/RenderSystem.h"
#include "cocoa/renderer/Shader.h"
#include "cocoa/core/Application.h"
#include "cocoa/core/AssetManager.h"
#include "cocoa/core/Memory.h"
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

		RenderBatchData CreateRenderBatch(int maxBatchSize, int zIndex, Handle<Shader> shader, bool batchOnTop)
		{
			RenderBatchData data;
			data.BatchShader = shader;
			data.ZIndex = zIndex;
			data.MaxBatchSize = maxBatchSize;
			data.VertexBufferBase = (Vertex*)AllocMem(sizeof(Vertex) * data.MaxBatchSize);
			data.VertexStackPointer = data.VertexBufferBase;
			data.Indices = (uint32*)AllocMem(sizeof(uint32) * data.MaxBatchSize * 2);

			for (int i = 0; i < data.Textures.size(); i++)
			{
				data.Textures[i] = {};
			}

			data.VAO = -1;
			data.VBO = -1;
			data.EBO = -1;

			data.BatchOnTop = batchOnTop;
			return data;
		}

		void Free(RenderBatchData& data)
		{
			if (data.VertexBufferBase)
			{
				FreeMem(data.VertexBufferBase);
				data.VertexBufferBase = nullptr;
			}
			else
			{
				Log::Warning("Failed to free render batches vertex data, invalid pointer.");
			}

			if (data.Indices)
			{
				FreeMem(data.Indices);
				data.Indices = nullptr;
			}
			else
			{
				Log::Warning("Failed to free render batches indices, invalid pointer.");
			}

			if (data.VAO != -1)
			{
				glDeleteBuffers(1, &data.VBO);
				glDeleteBuffers(1, &data.EBO);
				glDeleteVertexArrays(1, &data.VAO);
			}
			else
			{
				Log::Warning("Destroyed render batch, but it did not have any valid vao, vbo, or ebo");
			}
		}

		void Start(RenderBatchData& data)
		{
			GenerateIndices(data);

			glGenVertexArrays(1, &data.VAO);
			glGenBuffers(1, &data.VBO);
			glGenBuffers(1, &data.EBO);

			glBindVertexArray(data.VAO);

			glBindBuffer(GL_ARRAY_BUFFER, data.VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * data.MaxBatchSize, nullptr, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 2 * data.MaxBatchSize, data.Indices, GL_STATIC_DRAW);

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

		void Add(RenderBatchData& data, const TransformData& transform, const SpriteRenderer& spr)
		{
			// 4 Vertices per sprite
			data.NumUsedVertices += 4;

			const Sprite& sprite = spr.m_Sprite;
			Handle<Texture> tex = sprite.m_Texture;
			if (!tex.IsNull())
			{
				if (!HasTexture(data, tex))
				{
					data.Textures[data.NumTextures] = tex;
					data.NumTextures++;
				}
			}

			LoadVertexProperties(data, transform, spr);
		}

		void Add(RenderBatchData& data, const TransformData& transform, const FontRenderer& fontRenderer)
		{
			const Font& font = AssetManager::GetFont(fontRenderer.m_Font.m_AssetId);
			Handle<Texture> tex = font.m_FontTexture;
			const Texture& texture = AssetManager::GetTexture(tex.m_AssetId);

			if (!tex.IsNull())
			{
				if (!HasTexture(data, tex))
				{
					data.Textures[data.NumTextures] = tex;
					data.NumTextures++;
				}
			}

			int texId = 0;
			for (int i = 0; i < data.Textures.size(); i++)
			{
				if (data.Textures[i] == tex)
				{
					texId = i + 1;
					break;
				}
			}

			Entity res = NEntity::FromComponent<TransformData>(transform);
			uint32 entityId = NEntity::GetID(res);

			const std::string& str = fontRenderer.text;
			float x = transform.Position.x;
			float y = transform.Position.y;
			int strLength = str.size();
			for (int i = 0; i < strLength; i++)
			{
				// 4 Vertices per sprite
				data.NumUsedVertices += 4;
				const CharInfo& charInfo = font.GetCharacterInfo(str[i]);
				float scaleX = transform.Scale.x * fontRenderer.fontSize;
				float scaleY = transform.Scale.y * fontRenderer.fontSize;
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

				LoadVertexProperties(data, vertices, texCoords, fontRenderer.m_Color, { 0.0f, 0.0f }, texId, 4, entityId);

				x += charInfo.advance * fontRenderer.fontSize * transform.Scale.x;
			}
		}

		void Add(RenderBatchData& data, const glm::vec2* vertices, const glm::vec3& color, const glm::vec2& position, int numVertices)
		{
			data.NumUsedVertices += numVertices;
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

		void Add(
			RenderBatchData& data,
			Handle<Texture> textureHandle,
			const glm::vec3& position,
			const glm::vec3& scale,
			const glm::vec3& color,
			const glm::vec2& texCoordMin,
			const glm::vec2& texCoordMax,
			float rotation)
		{
			// 4 Vertices per sprite
			data.NumUsedVertices += 4;
			std::array<glm::vec2, 4> texCoords{
				glm::vec2 {texCoordMax.x, texCoordMax.y},
				glm::vec2 {texCoordMax.x, texCoordMin.y},
				glm::vec2 {texCoordMin.x, texCoordMin.y},
				glm::vec2 {texCoordMin.x, texCoordMax.y}
			};
			glm::vec4 vec4Color{ color.x, color.y, color.z, 1.0f };
			glm::vec3 vec3Pos{ position.x, position.y, 0.0f };


			if (!HasTexture(data, textureHandle))
			{
				data.Textures[data.NumTextures] = textureHandle;
				data.NumTextures++;
			}

			int texId = 0;
			for (int i = 0; i < data.NumTextures; i++)
			{
				if (data.Textures[i] == textureHandle)
				{
					texId = i + 1;
					break;
				}
			}

			LoadVertexProperties(data, position, scale, &texCoords[0], rotation, vec4Color, texId);
		}

		void LoadVertexProperties(RenderBatchData& data, const TransformData& transform, const SpriteRenderer& spr)
		{
			glm::vec4 color = spr.m_Color;
			const Sprite& sprite = spr.m_Sprite;
			const glm::vec2* texCoords = spr.m_Sprite.m_TexCoords;
			float rotation = transform.EulerRotation.z;

			int texId = 0;
			if (!sprite.m_Texture.IsNull())
			{
				for (int i = 0; i < data.Textures.size(); i++)
				{
					if (data.Textures[i] == sprite.m_Texture)
					{
						texId = i + 1;
						break;
					}
				}
			}

			Entity res = NEntity::FromComponent<TransformData>(transform);
			LoadVertexProperties(data, transform.Position, transform.Scale, texCoords, rotation, color, texId, NEntity::GetID(res));
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
				data.VertexStackPointer->position = glm::vec3(currentPos);
				data.VertexStackPointer->color = glm::vec4(color);
				data.VertexStackPointer->texCoords = glm::vec2(texCoords[i]);
				data.VertexStackPointer->texId = (float)texId;
				data.VertexStackPointer->entityId = entityId;

				data.VertexStackPointer++;
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
				data.VertexStackPointer->position = glm::vec3(vertices[i].x + position.x, vertices[i].y + position.y, 0.0f);
				data.VertexStackPointer->color = glm::vec4(color);
				data.VertexStackPointer->texCoords = glm::vec2(texCoords[i]);
				data.VertexStackPointer->texId = (float)texId;
				data.VertexStackPointer->entityId = entityId;

				data.VertexStackPointer++;
			}
		}

		void LoadEmptyVertexProperties(RenderBatchData& data)
		{
			for (int i = 0; i < 4; i++)
			{
				data.VertexStackPointer = {};
				data.VertexStackPointer++;
			}
		}

		void Render(RenderBatchData& data)
		{
			glBindBuffer(GL_ARRAY_BUFFER, data.VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * data.NumUsedVertices, &data.VertexBufferBase[0]);

			for (int i = 0; i < data.NumTextures; i++)
			{
				glActiveTexture(GL_TEXTURE0 + i + 1);
				TextureUtil::Bind(AssetManager::GetTexture(data.Textures[i].m_AssetId));
			}

			glBindVertexArray(data.VAO);

			glDrawElements(GL_TRIANGLES, data.NumUsedVertices * 2, GL_UNSIGNED_INT, 0);

			glBindVertexArray(0);

			for (int i = 0; i < data.NumTextures; i++)
			{
				TextureUtil::Unbind(AssetManager::GetTexture(data.Textures[i].m_AssetId));
			}
		}

		void GenerateIndices(RenderBatchData& data)
		{
			for (int i = 0; i < data.MaxBatchSize * 2; i++)
			{
				LoadElementIndices(data, i);
			}
		}

		void LoadElementIndices(RenderBatchData& data, int index)
		{
			int offsetArray = 6 * index;
			int offset = 4 * index;

			if (offsetArray + 2 < data.MaxBatchSize * 2)
			{
				// Triangle 1
				data.Indices[offsetArray] = offset + 3;
				data.Indices[offsetArray + 1] = offset + 2;
				data.Indices[offsetArray + 2] = offset + 0;
			}

			if (offsetArray + 5 < data.MaxBatchSize * 2)
			{
				// Triangle 2
				data.Indices[offsetArray + 3] = offset + 0;
				data.Indices[offsetArray + 4] = offset + 2;
				data.Indices[offsetArray + 5] = offset + 1;
			}
		}

		void Clear(RenderBatchData& data)
		{
			data.VertexStackPointer = data.VertexBufferBase;
			data.NumUsedVertices = 0;
			data.NumTextures = 0;
			for (int i = 0; i < data.NumTextures; i++)
			{
				data.Textures[i] = Handle<Texture>();
			}
		}

		bool HasRoom(const RenderBatchData& data, int numVertices)
		{
			return data.VertexStackPointer + numVertices < data.VertexBufferBase + data.MaxBatchSize * 4;
		}

		bool HasRoom(const RenderBatchData& data, const FontRenderer& fontRenderer)
		{
			// 4 Vertices per quad
			return data.NumUsedVertices + (fontRenderer.text.size() * 4) < data.MaxBatchSize;
		}

		bool HasTextureRoom(const RenderBatchData& data)
		{
			return data.NumTextures < data.Textures.size();
		}

		bool HasTexture(const RenderBatchData& data, Handle<Texture> texture)
		{
			for (int i = 0; i < data.NumTextures; i++)
			{
				if (data.Textures[i] == texture)
				{
					return true;
				}
			}
			return false;
		}

		bool Compare(const RenderBatchData& b1, const RenderBatchData& b2)
		{
			return b1.ZIndex < b2.ZIndex;
		}
	}
}