#include "cocoa/renderer/DebugDraw.h"

#include "cocoa/util/Settings.h"
#include "cocoa/util/CMath.h"
#include "cocoa/renderer/RenderBatch.h"
#include "cocoa/renderer/Line2D.h"
#include "cocoa/renderer/DebugSprite.h"
#include "cocoa/renderer/DebugShape.h"
#include "cocoa/core/AssetManager.h"

namespace Cocoa
{
	namespace DebugDraw
	{
		static const int m_FilledBoxVertCount = 4;
		static const int m_FilledBoxElementCount = 6;
		static const glm::vec2 m_FilledBoxModel[] = {
			// Standard verts for a 1x1 box
			{  0.5f, -0.5f },
			{  0.5f,  0.5f },
			{ -0.5f,  0.5f },
			{ -0.5f, -0.5f },
		};

		// Internal Variables
		static List<RenderBatchData> m_Batches;
		static List<Line2D> m_Lines;
		static List<DebugSprite> m_Sprites;
		static List<DebugShape> m_Shapes;
		static Handle<Shader> m_Shader;

		static const int m_TexSlots[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
		static const int m_MaxBatchSize = 500;

		// Forward Declarations
		static void RemoveDeadSprites();
		static void RemoveDeadLines();
		static void RemoveDeadShapes();
		static void AddSpritesToBatches();
		static void AddLinesToBatches();
		static void AddShapesToBatches();

		void Init()
		{
			m_Batches = List<RenderBatchData>();
			m_Lines = List<Line2D>();
			m_Sprites = List<DebugSprite>();
			m_Shapes = List<DebugShape>();
			m_Shader = NHandle::CreateHandle<Shader>();
		}

		void Destroy()
		{
			for (int i = 0; i < m_Batches.size(); i++)
			{
				RenderBatch::Free(m_Batches[i]);
			}

			for (int i = 0; i < m_Shapes.size(); i++)
			{
				FreeMem(m_Shapes[i].Vertices);
			}
		}

		void BeginFrame()
		{
			if (m_Shader.IsNull())
			{
				CPath shaderPath = Settings::General::s_EngineAssetsPath;
				NCPath::Join(shaderPath, NCPath::CreatePath("shaders/SpriteRenderer.glsl"));
				m_Shader = AssetManager::GetShader(shaderPath);
			}

			RemoveDeadLines();
			RemoveDeadSprites();
			RemoveDeadShapes();
		}

		void AddDebugObjectsToBatches()
		{
			AddLinesToBatches();
			AddSpritesToBatches();
			AddShapesToBatches();
		}

		void DrawBottomBatches(const Camera& camera)
		{
			const Shader& shaderRef = AssetManager::GetShader(m_Shader.AssetId);
			NShader::Bind(shaderRef);
			NShader::UploadMat4(shaderRef, "uProjection", camera.ProjectionMatrix);
			NShader::UploadMat4(shaderRef, "uView", camera.ViewMatrix);
			NShader::UploadIntArray(shaderRef, "uTextures[0]", 16, m_TexSlots);

			for (auto batch = m_Batches.begin(); batch != m_Batches.end(); batch++)
			{
				if (!batch->BatchOnTop)
				{
					RenderBatch::Render(*batch);
				}
			}

			NShader::Unbind(shaderRef);
		}

		void ClearAllBatches()
		{
			for (auto batch = m_Batches.begin(); batch != m_Batches.end(); batch++)
			{
				RenderBatch::Clear(*batch);
			}
		}

		void DrawTopBatches(const Camera& camera)
		{
			const Shader& shaderRef = AssetManager::GetShader(m_Shader.AssetId);
			NShader::Bind(shaderRef);
			NShader::UploadMat4(shaderRef, "uProjection", camera.ProjectionMatrix);
			NShader::UploadMat4(shaderRef, "uView", camera.ViewMatrix);
			NShader::UploadIntArray(shaderRef, "uTextures[0]", 16, m_TexSlots);

			for (auto batch = m_Batches.begin(); batch != m_Batches.end(); batch++)
			{
				if (batch->BatchOnTop)
				{
					RenderBatch::Render(*batch);
				}
			}

			NShader::Unbind(shaderRef);
		}

		// ===================================================================================================================
		// Draw Primitive Methods
		// ===================================================================================================================
		void AddLine2D(glm::vec2& from, glm::vec2& to, float strokeWidth, glm::vec3 color, int lifetime, bool onTop)
		{
			m_Lines.push(NLine2D::Create(from, to, color, strokeWidth, lifetime, onTop));
		}

		void AddBox2D(glm::vec2& center, glm::vec2& dimensions, float rotation, float strokeWidth, glm::vec3 color, int lifetime, bool onTop)
		{
			glm::vec2 min = center - (dimensions / 2.0f);
			glm::vec2 max = center + (dimensions / 2.0f);

			std::array<glm::vec2, 4> vertices = {
				glm::vec2(min.x, min.y), glm::vec2(min.x, max.y),
				glm::vec2(max.x, min.y), glm::vec2(max.x, max.y)
			};

			if (!CMath::Compare(rotation, 0.0f))
			{
				for (auto& vec : vertices)
				{
					CMath::Rotate(vec, rotation, center);
				}
			}

			AddLine2D(vertices[0], vertices[1], strokeWidth, color, lifetime, onTop);
			AddLine2D(vertices[1], vertices[3], strokeWidth, color, lifetime, onTop);
			AddLine2D(vertices[3], vertices[2], strokeWidth, color, lifetime, onTop);
			AddLine2D(vertices[2], vertices[0], strokeWidth, color, lifetime, onTop);
		}

		void AddFilledBox(
			const glm::vec2& center,
			const glm::vec2& dimensions,
			float rotation,
			const glm::vec3 color,
			int lifetime,
			bool onTop)
		{
			AddShape(m_FilledBoxModel, m_FilledBoxVertCount, m_FilledBoxElementCount, color, center, dimensions, rotation, lifetime, onTop);
		}

		void AddSprite(
			Handle<Texture> spriteTexture,
			glm::vec2 size,
			glm::vec2 position,
			glm::vec3 tint,
			glm::vec2 texCoordMin,
			glm::vec2 texCoordMax,
			float rotation,
			int lifetime,
			bool onTop)
		{
			m_Sprites.push(DebugSprite{ spriteTexture, size, position, tint, texCoordMin, texCoordMax, rotation, lifetime, onTop });
		}

		void AddShape(
			const glm::vec2* vertices, 
			int numVertices, 
			int numElements,
			const glm::vec3& color, 
			const glm::vec2& position, 
			const glm::vec2& scale, 
			float rotation, 
			int lifetime, 
			bool onTop)
		{
			// TODO: Maybe do this differently? I really don't like allocating and freeing so much small bits of memory
			glm::vec2* vertsCopy = (glm::vec2*)AllocMem(sizeof(glm::vec2) * numVertices);
			memcpy(vertsCopy, vertices, sizeof(glm::vec2) * numVertices);
			if (!CMath::Compare(rotation, 0.0f) || !CMath::Compare(scale, {1.0f, 1.0f}))
			{
				glm::mat4 matrix = glm::mat4(1.0f);
				matrix = glm::rotate(matrix, glm::radians(rotation), glm::vec3(0, 0, 1));
				matrix = glm::scale(matrix, glm::vec3(scale.x, scale.y, 1.0f));
				for (int i = 0; i < numVertices; i++)
				{
					glm::vec4 transformedPos = matrix * glm::vec4(vertsCopy[i].x, vertsCopy[i].y, 0.0f, 1.0f);
					vertsCopy[i] = glm::vec2(transformedPos.x, transformedPos.y);
				}
			}
			m_Shapes.push(DebugShape{ vertsCopy, numVertices, numElements, glm::vec3(color), glm::vec2(position), rotation, lifetime, onTop });
		}


		// ===================================================================================================================
		// Private methods
		// ===================================================================================================================
		static void RemoveDeadSprites()
		{
			int index = 0;
			while (index < m_Sprites.size())
			{
				DebugSprite& spriteIter = m_Sprites[index];
				spriteIter.Lifetime--;
				if (spriteIter.Lifetime <= 0)
				{
					m_Sprites.removeByIndex(index);
				}
				else
				{
					index++;
				}
			}
		}

		static void RemoveDeadLines()
		{
			auto index = 0;
			while (index != m_Lines.size())
			{
				Line2D& lineIter = m_Lines[index];
				lineIter.Lifetime--;
				// TODO: Investigate why this needs < but RemoveDeadShapes only needs <=
				if (lineIter.Lifetime < 0)
				{
					m_Lines.removeByIndex(index);
				}
				else
				{
					index++;
				}
			}
		}

		static void RemoveDeadShapes()
		{
			auto index = 0;
			while (index != m_Shapes.size())
			{
				DebugShape& shapeIter = m_Shapes[index];
				shapeIter.Lifetime--;
				if (shapeIter.Lifetime <= 0)
				{
					FreeMem(m_Shapes[index].Vertices);
					m_Shapes.removeByIndex(index);
				}
				else
				{
					index++;
				}
			}
		}

		static void AddSpritesToBatches()
		{
			for (int i = 0; i < m_Sprites.size(); i++)
			{
				const DebugSprite& sprite = m_Sprites[i];
				bool wasAdded = false;
				bool spriteOnTop = sprite.OnTop;
				for (auto batch = m_Batches.begin(); batch != m_Batches.end(); batch++)
				{
					if (RenderBatch::HasRoom(*batch) && spriteOnTop == batch->BatchOnTop)
					{
						if (!sprite.SpriteTexture || RenderBatch::HasTexture(*batch, sprite.SpriteTexture) || RenderBatch::HasTextureRoom(*batch))
						{
							RenderBatch::Add(
								*batch, 
								sprite.SpriteTexture, 
								CMath::Vector3From2(sprite.Position), 
								CMath::Vector3From2(sprite.Size), 
								sprite.Tint, 
								sprite.TexCoordMin, 
								sprite.TexCoordMax, 
								sprite.Rotation);

							wasAdded = true;
							break;
						}
					}
				}

				if (!wasAdded)
				{
					RenderBatchData newBatch = RenderBatch::CreateRenderBatch(m_MaxBatchSize, 0, m_Shader, spriteOnTop);
					RenderBatch::Start(newBatch);
					RenderBatch::Add(
						newBatch, 
						sprite.SpriteTexture, 
						CMath::Vector3From2(sprite.Position), 
						CMath::Vector3From2(sprite.Size), 
						sprite.Tint, 
						sprite.TexCoordMin, 
						sprite.TexCoordMax, 
						sprite.Rotation);

					m_Batches.push(newBatch);
				}
			}
		}

		static void AddLinesToBatches()
		{
			for (auto line = m_Lines.begin(); line != m_Lines.end(); line++)
			{
				bool wasAdded = false;
				bool lineOnTop = line->OnTop;
				for (auto batch = m_Batches.begin(); batch != m_Batches.end(); batch++)
				{
					if (RenderBatch::HasRoom(*batch) && (lineOnTop == batch->BatchOnTop))
					{
						RenderBatch::Add(*batch, line->Verts, line->Color);
						wasAdded = true;
						break;
					}
				}

				if (!wasAdded)
				{
					RenderBatchData newBatch = RenderBatch::CreateRenderBatch(m_MaxBatchSize, 0, m_Shader, lineOnTop);
					RenderBatch::Start(newBatch);
					RenderBatch::Add(newBatch, line->Verts, line->Color);
					m_Batches.push(newBatch);
				}
			}
		}

		static void AddShapesToBatches()
		{
			for (auto shape = m_Shapes.begin(); shape != m_Shapes.end(); shape++)
			{
				bool wasAdded = false;
				bool shapeOnTop = shape->OnTop;
				for (auto batch = m_Batches.begin(); batch != m_Batches.end(); batch++)
				{
					if (RenderBatch::HasRoom(*batch, shape->NumVertices) && (shapeOnTop == batch->BatchOnTop))
					{
						RenderBatch::Add(*batch, shape->Vertices, shape->Color, shape->Position, shape->NumVertices, shape->NumElements);
						wasAdded = true;
						break;
					}
				}

				if (!wasAdded)
				{
					RenderBatchData newBatch = RenderBatch::CreateRenderBatch(m_MaxBatchSize, 0, m_Shader, shapeOnTop);
					RenderBatch::Start(newBatch);
					RenderBatch::Add(newBatch, shape->Vertices, shape->Color, shape->Position, shape->NumVertices, shape->NumElements);
					m_Batches.push(newBatch);
				}
			}
		}
	}
}