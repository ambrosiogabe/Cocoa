#include "externalLibs.h"

#include "jade/renderer/DebugDraw.h"
#include "jade/core/Application.h"
#include "jade/util/JMath.h"

namespace Jade
{
	std::vector<RenderBatch*> DebugDraw::m_Batches = std::vector<RenderBatch*>();
	std::vector<Line2D> DebugDraw::m_Lines = std::vector<Line2D>();
	std::vector<DebugSprite> DebugDraw::m_Sprites = std::vector<DebugSprite>();
	int DebugDraw::m_TexSlots[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
	Shader* DebugDraw::m_Shader = nullptr;
	int DebugDraw::m_MaxBatchSize = 500;


	void DebugDraw::BeginFrame()
	{
		if (m_Shader == nullptr)
		{
			m_Shader = new Shader(Settings::General::s_EngineAssetsPath + "shaders/SpriteRenderer.glsl");
		}

		RemoveDeadLines();
		RemoveDeadSprites();
	}

	void DebugDraw::DrawBottomBatches()
	{
		AddLinesToBatches();
		AddSpritesToBatches();

		m_Shader->Bind();
		m_Shader->UploadMat4("uProjection", Application::Get()->GetScene()->GetCamera()->GetOrthoProjection());
		m_Shader->UploadMat4("uView", Application::Get()->GetScene()->GetCamera()->GetOrthoView());
		m_Shader->UploadIntArray("uTextures", 16, m_TexSlots);

		for (RenderBatch* batch : m_Batches)
		{
			if (!batch->BatchOnTop())
			{
				batch->Render();
				batch->Clear();
			}
		}

		m_Shader->Unbind();
	}

	void DebugDraw::DrawTopBatches()
	{
		m_Shader->Bind();
		m_Shader->UploadMat4("uProjection", Application::Get()->GetScene()->GetCamera()->GetOrthoProjection());
		m_Shader->UploadMat4("uView", Application::Get()->GetScene()->GetCamera()->GetOrthoView());
		m_Shader->UploadIntArray("uTextures", 16, m_TexSlots);

		for (RenderBatch* batch : m_Batches)
		{
			if (batch->BatchOnTop())
			{
				batch->Render();
				batch->Clear();
			}
		}

		m_Shader->Unbind();
	}

	// ===================================================================================================================
	// Draw Primitive Methods
	// ===================================================================================================================
	void DebugDraw::AddLine2D(glm::vec2& from, glm::vec2& to, float strokeWidth, glm::vec3 color, int lifetime, bool onTop)
	{
		m_Lines.push_back(Line2D(from, to, color, strokeWidth, lifetime, onTop));
	}

	void DebugDraw::AddBox2D(glm::vec2& center, glm::vec2& dimensions, float rotation, float strokeWidth, glm::vec3 color, int lifetime, bool onTop)
	{
		glm::vec2 min = center - (dimensions / 2.0f);
		glm::vec2 max = center + (dimensions / 2.0f);

		std::array<glm::vec2, 4> vertices = {
			glm::vec2(min.x, min.y), glm::vec2(min.x, max.y),
			glm::vec2(max.x, max.y), glm::vec2(max.x, min.y)
		};

		if (!JMath::Compare(rotation, 0.0f))
		{
			for (auto& vec : vertices)
			{
				JMath::Rotate(vec, rotation, center);
			}
		}

		AddLine2D(vertices[0], vertices[1], strokeWidth, color, lifetime, onTop);
		AddLine2D(vertices[0], vertices[3], strokeWidth, color, lifetime, onTop);
		AddLine2D(vertices[1], vertices[2], strokeWidth, color, lifetime, onTop);
		AddLine2D(vertices[2], vertices[3], strokeWidth, color, lifetime, onTop);
	}

	void DebugDraw::AddSprite(uint32 textureAssetId, glm::vec2 size, glm::vec2 position, glm::vec3 tint,
		glm::vec2 texCoordMin, glm::vec2 texCoordMax, float rotation, int lifetime, bool onTop)
	{
		m_Sprites.push_back({ textureAssetId, size, position, tint, texCoordMin, texCoordMax, rotation, lifetime, onTop });
	}


	// ===================================================================================================================
	// Private methods
	// ===================================================================================================================
	void DebugDraw::RemoveDeadSprites()
	{
		for (int i = 0; i < m_Sprites.size(); i++)
		{
			if (m_Sprites[i].BeginFrame() <= 0)
			{
				m_Sprites.erase(m_Sprites.begin() + i);
				i--;
			}
		}
	}

	void DebugDraw::RemoveDeadLines()
	{
		for (int i = 0; i < m_Lines.size(); i++)
		{
			if (m_Lines[i].BeginFrame() <= 0)
			{
				m_Lines.erase(m_Lines.begin() + i);
				i--;
			}
		}
	}

	void DebugDraw::AddSpritesToBatches()
	{
		for (int i = (int)m_Sprites.size() - 1; i >= 0; i--)
		{
			DebugSprite sprite = m_Sprites[i];
			bool wasAdded = false;
			bool spriteOnTop = sprite.m_OnTop;
			for (RenderBatch* batch : m_Batches)
			{
				if (batch->HasRoom() && (batch->HasTexture(sprite.m_TextureAssetId) || batch->HasTextureRoom()) && (spriteOnTop == batch->BatchOnTop()))
				{
					batch->Add(sprite.m_TextureAssetId, sprite.m_Size, sprite.m_Position, sprite.m_Tint, sprite.m_TexCoordMin, sprite.m_TexCoordMax, sprite.m_Rotation);
					wasAdded = true;
					break;
				}
			}

			if (!wasAdded)
			{
				RenderBatch* newBatch = new RenderBatch(m_MaxBatchSize, spriteOnTop);
				newBatch->Start();
				newBatch->Add(sprite.m_TextureAssetId, sprite.m_Size, sprite.m_Position, sprite.m_Tint, sprite.m_TexCoordMin, sprite.m_TexCoordMax, sprite.m_Rotation);
				m_Batches.push_back(newBatch);
			}
		}
	}

	void DebugDraw::AddLinesToBatches()
	{
		for (Line2D line : m_Lines)
		{
			bool wasAdded = false;
			bool lineOnTop = line.IsOnTop();
			for (RenderBatch* batch : m_Batches)
			{
				if (batch->HasRoom() && (lineOnTop == batch->BatchOnTop()))
				{
					batch->Add(line.GetMin(), line.GetMax(), line.GetColor());
					wasAdded = true;
					break;
				}
			}

			if (!wasAdded)
			{
				RenderBatch* newBatch = new RenderBatch(m_MaxBatchSize, lineOnTop);
				newBatch->Start();
				newBatch->Add(line.GetMin(), line.GetMax(), line.GetColor());;
				m_Batches.push_back(newBatch);
			}
		}
	}


}