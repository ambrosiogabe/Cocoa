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
			// Standard vertices for a 1x1 box
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

		void init()
		{
			m_Batches = List<RenderBatchData>();
			m_Lines = List<Line2D>();
			m_Sprites = List<DebugSprite>();
			m_Shapes = List<DebugShape>();
			m_Shader = NHandle::createHandle<Shader>();
		}

		void destroy()
		{
			for (int i = 0; i < m_Batches.size(); i++)
			{
				RenderBatch::free(m_Batches[i]);
			}

			for (int i = 0; i < m_Shapes.size(); i++)
			{
				FreeMem(m_Shapes[i].vertices);
			}
		}

		void beginFrame()
		{
			if (m_Shader.isNull())
			{
				CPath shaderPath = Settings::General::engineAssetsPath;
				shaderPath.join(CPath::create("shaders/SpriteRenderer.glsl"));
				m_Shader = AssetManager::getShader(shaderPath);
			}

			RemoveDeadLines();
			RemoveDeadSprites();
			RemoveDeadShapes();
		}

		void addDebugObjectsToBatches()
		{
			AddLinesToBatches();
			AddSpritesToBatches();
			AddShapesToBatches();
		}

		void drawBottomBatches(const Camera& camera)
		{
			const Shader& shaderRef = AssetManager::getShader(m_Shader.assetId);
			NShader::bind(shaderRef);
			NShader::uploadMat4(shaderRef, "uProjection", camera.projectionMatrix);
			NShader::uploadMat4(shaderRef, "uView", camera.viewMatrix);
			NShader::uploadIntArray(shaderRef, "uTextures[0]", 16, m_TexSlots);

			for (auto batch = m_Batches.begin(); batch != m_Batches.end(); batch++)
			{
				if (!batch->batchOnTop)
				{
					RenderBatch::render(*batch);
				}
			}

			NShader::unbind(shaderRef);
		}

		void clearAllBatches()
		{
			for (auto batch = m_Batches.begin(); batch != m_Batches.end(); batch++)
			{
				RenderBatch::clear(*batch);
			}
		}

		void drawTopBatches(const Camera& camera)
		{
			const Shader& shaderRef = AssetManager::getShader(m_Shader.assetId);
			NShader::bind(shaderRef);
			NShader::uploadMat4(shaderRef, "uProjection", camera.projectionMatrix);
			NShader::uploadMat4(shaderRef, "uView", camera.viewMatrix);
			NShader::uploadIntArray(shaderRef, "uTextures[0]", 16, m_TexSlots);

			for (auto batch = m_Batches.begin(); batch != m_Batches.end(); batch++)
			{
				if (batch->batchOnTop)
				{
					RenderBatch::render(*batch);
				}
			}

			NShader::unbind(shaderRef);
		}

		// ===================================================================================================================
		// Draw Primitive Methods
		// ===================================================================================================================
		void addLine2D(glm::vec2& from, glm::vec2& to, float strokeWidth, glm::vec3 color, int lifetime, bool onTop)
		{
			m_Lines.push(NLine2D::create(from, to, color, strokeWidth, lifetime, onTop));
		}

		void addBox2D(glm::vec2& center, glm::vec2& dimensions, float rotation, float strokeWidth, glm::vec3 color, int lifetime, bool onTop)
		{
			glm::vec2 min = center - (dimensions / 2.0f);
			glm::vec2 max = center + (dimensions / 2.0f);

			std::array<glm::vec2, 4> vertices = {
				glm::vec2(min.x, min.y), glm::vec2(min.x, max.y),
				glm::vec2(max.x, min.y), glm::vec2(max.x, max.y)
			};

			if (!CMath::compare(rotation, 0.0f))
			{
				for (auto& vec : vertices)
				{
					CMath::rotate(vec, rotation, center);
				}
			}

			addLine2D(vertices[0], vertices[1], strokeWidth, color, lifetime, onTop);
			addLine2D(vertices[1], vertices[3], strokeWidth, color, lifetime, onTop);
			addLine2D(vertices[3], vertices[2], strokeWidth, color, lifetime, onTop);
			addLine2D(vertices[2], vertices[0], strokeWidth, color, lifetime, onTop);
		}

		void addFilledBox(
			const glm::vec2& center,
			const glm::vec2& dimensions,
			float rotation,
			const glm::vec3 color,
			int lifetime,
			bool onTop)
		{
			addShape(m_FilledBoxModel, m_FilledBoxVertCount, m_FilledBoxElementCount, color, center, dimensions, rotation, lifetime, onTop);
		}

		void addSprite(
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

		void addShape(
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
			if (!CMath::compare(rotation, 0.0f) || !CMath::compare(scale, {1.0f, 1.0f}))
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
				spriteIter.lifetime--;
				if (spriteIter.lifetime <= 0)
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
				lineIter.lifetime--;
				// TODO: Investigate why this needs < but RemoveDeadShapes only needs <=
				if (lineIter.lifetime < 0)
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
				shapeIter.lifetime--;
				if (shapeIter.lifetime <= 0)
				{
					FreeMem(m_Shapes[index].vertices);
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
				bool spriteOnTop = sprite.onTop;
				for (auto batch = m_Batches.begin(); batch != m_Batches.end(); batch++)
				{
					if (RenderBatch::hasRoom(*batch) && spriteOnTop == batch->batchOnTop)
					{
						if (!sprite.spriteTexture || RenderBatch::hasTexture(*batch, sprite.spriteTexture) || RenderBatch::hasTextureRoom(*batch))
						{
							RenderBatch::add(
								*batch, 
								sprite.spriteTexture, 
								CMath::vector3From2(sprite.position), 
								CMath::vector3From2(sprite.size), 
								sprite.tint, 
								sprite.texCoordMin, 
								sprite.texCoordMax, 
								sprite.rotation);

							wasAdded = true;
							break;
						}
					}
				}

				if (!wasAdded)
				{
					RenderBatchData newBatch = RenderBatch::createRenderBatch(m_MaxBatchSize, 0, m_Shader, spriteOnTop);
					RenderBatch::start(newBatch);
					RenderBatch::add(
						newBatch, 
						sprite.spriteTexture, 
						CMath::vector3From2(sprite.position), 
						CMath::vector3From2(sprite.size), 
						sprite.tint, 
						sprite.texCoordMin, 
						sprite.texCoordMax, 
						sprite.rotation);

					m_Batches.push(newBatch);
				}
			}
		}

		static void AddLinesToBatches()
		{
			for (auto line = m_Lines.begin(); line != m_Lines.end(); line++)
			{
				bool wasAdded = false;
				bool lineOnTop = line->onTop;
				for (auto batch = m_Batches.begin(); batch != m_Batches.end(); batch++)
				{
					if (RenderBatch::hasRoom(*batch) && (lineOnTop == batch->batchOnTop))
					{
						RenderBatch::add(*batch, line->vertices, line->color);
						wasAdded = true;
						break;
					}
				}

				if (!wasAdded)
				{
					RenderBatchData newBatch = RenderBatch::createRenderBatch(m_MaxBatchSize, 0, m_Shader, lineOnTop);
					RenderBatch::start(newBatch);
					RenderBatch::add(newBatch, line->vertices, line->color);
					m_Batches.push(newBatch);
				}
			}
		}

		static void AddShapesToBatches()
		{
			for (auto shape = m_Shapes.begin(); shape != m_Shapes.end(); shape++)
			{
				bool wasAdded = false;
				bool shapeOnTop = shape->onTop;
				for (auto batch = m_Batches.begin(); batch != m_Batches.end(); batch++)
				{
					if (RenderBatch::hasRoom(*batch, shape->numVertices) && (shapeOnTop == batch->batchOnTop))
					{
						RenderBatch::add(*batch, shape->vertices, shape->color, shape->position, shape->numVertices, shape->numElements);
						wasAdded = true;
						break;
					}
				}

				if (!wasAdded)
				{
					RenderBatchData newBatch = RenderBatch::createRenderBatch(m_MaxBatchSize, 0, m_Shader, shapeOnTop);
					RenderBatch::start(newBatch);
					RenderBatch::add(newBatch, shape->vertices, shape->color, shape->position, shape->numVertices, shape->numElements);
					m_Batches.push(newBatch);
				}
			}
		}
	}
}