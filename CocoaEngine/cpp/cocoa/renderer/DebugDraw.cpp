#include "cocoa/renderer/DebugDraw.h"

#include "cocoa/util/DynamicArray.h"
#include "cocoa/util/Settings.h"
#include "cocoa/util/CMath.h"
#include "cocoa/renderer/RenderBatch.h"
#include "cocoa/renderer/Line2D.h"
#include "cocoa/renderer/DebugSprite.h"
#include "cocoa/core/AssetManager.h"

namespace Cocoa
{
	namespace DebugDraw
	{
		// Internal Variables
		static DynamicArray<RenderBatchData> m_Batches;
		static DynamicArray<Line2D> m_Lines;
		static DynamicArray<DebugSprite> m_Sprites;
		static Handle<Shader> m_Shader;

		static const int m_TexSlots[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
		static const int m_MaxBatchSize = 500;

		// Forward Declarations
		static void RemoveDeadSprites();
		static void RemoveDeadLines();
		static void AddSpritesToBatches();
		static void AddLinesToBatches();

		void Init()
		{
			m_Batches = NDynamicArray::Create<RenderBatchData>();
			m_Lines = NDynamicArray::Create<Line2D>();
			m_Sprites = NDynamicArray::Create<DebugSprite>();
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
		}

		void DrawBottomBatches(const Camera& camera)
		{
			AddLinesToBatches();
			AddSpritesToBatches();

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
					// TODO: Triple check this logic
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
	}
}