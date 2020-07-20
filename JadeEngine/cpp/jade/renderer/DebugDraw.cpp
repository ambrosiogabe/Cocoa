#include "externalLibs.h"

#include "jade/renderer/DebugDraw.h"
#include "jade/core/Application.h"
#include "jade/util/JMath.h"

namespace Jade
{
	std::vector<RenderBatch*> DebugDraw::m_Batches = std::vector<RenderBatch*>();
	std::vector<Line2D> DebugDraw::m_Lines = std::vector<Line2D>();
	Shader* DebugDraw::m_Shader = nullptr;
	int DebugDraw::m_MaxBatchSize = 500;

	void DebugDraw::BeginFrame()
	{
		if (m_Shader == nullptr)
		{
			m_Shader = new Shader("assets/shaders/SpriteRenderer.glsl");
		}

		// Remove dead lines
		for (int i=0; i < m_Lines.size(); i++)
		{
			if (m_Lines[i].BeginFrame() <= 0)
			{
				m_Lines.erase(m_Lines.begin() + i);
				i--;
			}
		}
	}

	void DebugDraw::EndFrame()
	{
		for (Line2D line : m_Lines)
		{
			bool wasAdded = false;
			for (RenderBatch* batch : m_Batches)
			{
				if (batch->HasRoom())
				{
					batch->Add(line.GetMin(), line.GetMax(), line.GetColor());
					wasAdded = true;
					break;
				}
			}

			if (!wasAdded)
			{
				RenderBatch* newBatch = new RenderBatch(m_MaxBatchSize);
				newBatch->Start();
				newBatch->Add(line.GetMin(), line.GetMax(), line.GetColor());;
				m_Batches.push_back(newBatch);
			}
		}

		// Use shader
		m_Shader->Bind();
		m_Shader->UploadMat4("uProjection", Application::Get()->GetScene()->GetCamera()->GetOrthoProjection());
		m_Shader->UploadMat4("uView", Application::Get()->GetScene()->GetCamera()->GetOrthoView());

		for (RenderBatch* batch : m_Batches)
		{
			batch->Render();
			batch->Clear();
		}

		m_Shader->Unbind();
	}



	// ------Draw Primitive Methods---------------------------------------------------------------------------------------

	void DebugDraw::AddLine2D(glm::vec2& from, glm::vec2& to, float strokeWidth, glm::vec3 color, int lifetime)
	{
		m_Lines.push_back(Line2D(from, to, color, strokeWidth, lifetime));
	}

	void DebugDraw::AddBox2D(glm::vec2& center, glm::vec2& dimensions, float rotation, float strokeWidth, glm::vec3 color, int lifetime)
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

		AddLine2D(vertices[0], vertices[1], strokeWidth, color, lifetime);
		AddLine2D(vertices[0], vertices[3], strokeWidth, color, lifetime);
		AddLine2D(vertices[1], vertices[2], strokeWidth, color, lifetime);
		AddLine2D(vertices[2], vertices[3], strokeWidth, color, lifetime);
	}

	// ---------------------------------------------------------------------------------------------------------------

}