#include "cocoa/renderer/DebugDraw.h"

#include "cocoa/util/DynamicArray.h"
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
		static const glm::vec2 m_FilledBoxModel[] = {
			// Standard verts for a 1x1 box
			{  0.5f, -0.5f },
			{  0.5f,  0.5f },
			{ -0.5f,  0.5f },
			{ -0.5f, -0.5f },
		};

		// Internal Variables
		static DynamicArray<RenderBatchData> m_Batches;
		static DynamicArray<Line2D> m_Lines;
		static DynamicArray<DebugSprite> m_Sprites;
		static DynamicArray<DebugShape> m_Shapes;
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
			m_Batches = NDynamicArray::Create<RenderBatchData>();
			m_Lines = NDynamicArray::Create<Line2D>();
			m_Sprites = NDynamicArray::Create<DebugSprite>();
			m_Shapes = NDynamicArray::Create<DebugShape>();
			m_Shader = Handle<Shader>();
		}

		void Destroy()
		{
			for (int i = 0; i < m_Batches.m_NumElements; i++)
			{
				RenderBatch::Free(m_Batches.m_Data[i]);
			}
			NDynamicArray::Free<RenderBatchData>(m_Batches);
			NDynamicArray::Free<Line2D>(m_Lines);
			NDynamicArray::Free<DebugSprite>(m_Sprites);

			for (int i = 0; i < m_Shapes.m_NumElements; i++)
			{
				FreeMem(m_Shapes.m_Data[i].Vertices);
			}
			NDynamicArray::Free<DebugShape>(m_Shapes);
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

		void DrawBottomBatches(const Camera& camera)
		{
			AddLinesToBatches();
			AddSpritesToBatches();
			AddShapesToBatches();

			const Shader& shaderRef = AssetManager::GetShader(m_Shader.m_AssetId);
			NShader::Bind(shaderRef);
			NShader::UploadMat4(shaderRef, "uProjection", camera.ProjectionMatrix);
			NShader::UploadMat4(shaderRef, "uView", camera.ViewMatrix);
			NShader::UploadIntArray(shaderRef, "uTextures[0]", 16, m_TexSlots);

			for (auto batch = NDynamicArray::Begin<RenderBatchData>(m_Batches); batch != NDynamicArray::End<RenderBatchData>(m_Batches); batch++)
			{
				if (!batch->BatchOnTop)
				{
					RenderBatch::Render(*batch);
					RenderBatch::Clear(*batch);
				}
			}

			NShader::Unbind(shaderRef);
		}

		void DrawTopBatches(const Camera& camera)
		{
			const Shader& shaderRef = AssetManager::GetShader(m_Shader.m_AssetId);
			NShader::Bind(shaderRef);
			NShader::UploadMat4(shaderRef, "uProjection", camera.ProjectionMatrix);
			NShader::UploadMat4(shaderRef, "uView", camera.ViewMatrix);
			NShader::UploadIntArray(shaderRef, "uTextures[0]", 16, m_TexSlots);

			for (auto batch = NDynamicArray::Begin<RenderBatchData>(m_Batches); batch != NDynamicArray::End<RenderBatchData>(m_Batches); batch++)
			{
				if (batch->BatchOnTop)
				{
					RenderBatch::Render(*batch);
					RenderBatch::Clear(*batch);
				}
			}

			NShader::Unbind(shaderRef);
		}

		// ===================================================================================================================
		// Draw Primitive Methods
		// ===================================================================================================================
		void AddLine2D(glm::vec2& from, glm::vec2& to, float strokeWidth, glm::vec3 color, int lifetime, bool onTop)
		{
			NDynamicArray::Add<Line2D>(m_Lines, NLine2D::Create(from, to, color, strokeWidth, lifetime, onTop));
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
			AddShape(m_FilledBoxModel, m_FilledBoxVertCount, color, center, dimensions, rotation, lifetime, onTop);
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
			NDynamicArray::Add<DebugSprite>(m_Sprites, DebugSprite{ spriteTexture, size, position, tint, texCoordMin, texCoordMax, rotation, lifetime, onTop });
		}

		void AddShape(const glm::vec2* vertices, int numVertices, const glm::vec3& color, const glm::vec2& position, const glm::vec2& scale, float rotation, int lifetime, bool onTop)
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
			NDynamicArray::Add<DebugShape>(m_Shapes, DebugShape{ vertsCopy, numVertices, glm::vec3(color), glm::vec2(position), rotation, lifetime, onTop });
		}


		// ===================================================================================================================
		// Private methods
		// ===================================================================================================================
		static void RemoveDeadSprites()
		{
			int index = 0;
			while (index < m_Sprites.m_NumElements)
			{
				DebugSprite& spriteIter = m_Sprites.m_Data[index];
				spriteIter.Lifetime--;
				if (spriteIter.Lifetime <= 0)
				{
					NDynamicArray::Remove<DebugSprite>(m_Sprites, index);
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
			while (index != m_Lines.m_NumElements)
			{
				Line2D& lineIter = m_Lines.m_Data[index];
				lineIter.Lifetime--;
				if (lineIter.Lifetime <= 0)
				{
					NDynamicArray::Remove<Line2D>(m_Lines, index);
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
			while (index != m_Shapes.m_NumElements)
			{
				DebugShape& shapeIter = m_Shapes.m_Data[index];
				shapeIter.Lifetime--;
				if (shapeIter.Lifetime <= 0)
				{
					FreeMem(m_Shapes.m_Data[index].Vertices);
					NDynamicArray::Remove<DebugShape>(m_Shapes, index);
				}
				else
				{
					index++;
				}
			}
		}

		static void AddSpritesToBatches()
		{
			for (int i = 0; i < m_Sprites.m_NumElements; i++)
			{
				const DebugSprite& sprite = m_Sprites.m_Data[i];
				bool wasAdded = false;
				bool spriteOnTop = sprite.OnTop;
				for (auto batch = NDynamicArray::Begin<RenderBatchData>(m_Batches); batch != NDynamicArray::End<RenderBatchData>(m_Batches); batch++)
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

					NDynamicArray::Add<RenderBatchData>(m_Batches, newBatch);
				}
			}
		}

		static void AddLinesToBatches()
		{
			for (auto line = NDynamicArray::Begin<Line2D>(m_Lines); line != NDynamicArray::End<Line2D>(m_Lines); line++)
			{
				bool wasAdded = false;
				bool lineOnTop = line->OnTop;
				for (auto batch = NDynamicArray::Begin<RenderBatchData>(m_Batches); batch != NDynamicArray::End<RenderBatchData>(m_Batches); batch++)
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
					NDynamicArray::Add<RenderBatchData>(m_Batches, newBatch);
				}
			}
		}

		static void AddShapesToBatches()
		{
			for (auto shape = NDynamicArray::Begin<DebugShape>(m_Shapes); shape != NDynamicArray::End<DebugShape>(m_Shapes); shape++)
			{
				bool wasAdded = false;
				bool shapeOnTop = shape->OnTop;
				for (auto batch = NDynamicArray::Begin<RenderBatchData>(m_Batches); batch != NDynamicArray::End<RenderBatchData>(m_Batches); batch++)
				{
					if (RenderBatch::HasRoom(*batch, shape->NumVertices) && (shapeOnTop == batch->BatchOnTop))
					{
						RenderBatch::Add(*batch, shape->Vertices, shape->Color, shape->Position, shape->NumVertices);
						wasAdded = true;
						break;
					}
				}

				if (!wasAdded)
				{
					RenderBatchData newBatch = RenderBatch::CreateRenderBatch(m_MaxBatchSize, 0, m_Shader, shapeOnTop);
					RenderBatch::Start(newBatch);
					RenderBatch::Add(newBatch, shape->Vertices, shape->Color, shape->Position, shape->NumVertices);
					NDynamicArray::Add<RenderBatchData>(m_Batches, newBatch);
				}
			}
		}
	}
}