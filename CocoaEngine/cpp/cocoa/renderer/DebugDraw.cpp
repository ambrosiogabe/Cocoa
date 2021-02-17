#include "externalLibs.h"

#include "cocoa/renderer/DebugDraw.h"
#include "cocoa/renderer/Camera.h"
#include "cocoa/core/Application.h"
#include "cocoa/core/AssetManager.h"
#include "cocoa/util/CMath.h"
#include "cocoa/util/Settings.h"

namespace Cocoa
{
	// TODO: Major memory leak in here, fix that...
	//DynamicArray<RenderBatch> DebugDraw::s_Batches = NDynamicArray::Create<RenderBatch>();
	std::vector<Line2D> DebugDraw::s_Lines = std::vector<Line2D>();
	std::vector<DebugSprite> DebugDraw::s_Sprites = std::vector<DebugSprite>();
	int DebugDraw::s_TexSlots[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
	Handle<Shader> DebugDraw::s_Shader = Handle<Shader>();
	int DebugDraw::s_MaxBatchSize = 500;
	SceneData* DebugDraw::s_Scene = nullptr;

	void DebugDraw::Init(SceneData* scene)
	{
		s_Scene = scene;
	}

	void DebugDraw::BeginFrame()
	{
		if (s_Shader.IsNull())
		{
			Log::Assert((s_Scene != nullptr), "DebugDraw's scene is nullptr. Did you forget to initialize DebugDraw when you changed scenes?");
			CPath shaderPath = Settings::General::s_EngineAssetsPath;
			NCPath::Join(shaderPath, NCPath::CreatePath("shaders/SpriteRenderer.glsl"));
			s_Shader = AssetManager::GetShader(shaderPath);
		}

		RemoveDeadLines();
		RemoveDeadSprites();
	}

	void DebugDraw::DrawBottomBatches()
	{
		AddLinesToBatches();
		AddSpritesToBatches();

		const Shader& shaderRef = AssetManager::GetShader(s_Shader.m_AssetId);
		NShader::Bind(shaderRef);
		NShader::UploadMat4(shaderRef, "uProjection", s_Scene->SceneCamera.ProjectionMatrix);
		NShader::UploadMat4(shaderRef, "uView", s_Scene->SceneCamera.ViewMatrix);
		NShader::UploadIntArray(shaderRef, "uTextures[0]", 16, s_TexSlots);

		//for (auto batch = NDynamicArray::Begin<RenderBatch>(s_Batches); batch != NDynamicArray::End<RenderBatch>(s_Batches); batch++)
		//{
		//	if (!batch->BatchOnTop())
		//	{
		//		batch->Render();
		//		batch->Clear();
		//	}
		//}

		NShader::Unbind(shaderRef);
	}

	void DebugDraw::DrawTopBatches()
	{
		const Shader& shaderRef = AssetManager::GetShader(s_Shader.m_AssetId);
		NShader::Bind(shaderRef);
		NShader::UploadMat4(shaderRef, "uProjection", s_Scene->SceneCamera.ProjectionMatrix);
		NShader::UploadMat4(shaderRef, "uView", s_Scene->SceneCamera.ViewMatrix);
		NShader::UploadIntArray(shaderRef, "uTextures[0]", 16, s_TexSlots);

		//for (auto batch = NDynamicArray::Begin<RenderBatch>(s_Batches); batch != NDynamicArray::End<RenderBatch>(s_Batches); batch++)
		//{
		//	if (batch->BatchOnTop())
		//	{
		//		batch->Render();
		//		batch->Clear();
		//	}
		//}

		NShader::Unbind(shaderRef);
	}

	// ===================================================================================================================
	// Draw Primitive Methods
	// ===================================================================================================================
	void DebugDraw::AddLine2D(glm::vec2& from, glm::vec2& to, float strokeWidth, glm::vec3 color, int lifetime, bool onTop)
	{
		// TODO: The offender
		//if (s_Lines.size() < s_MaxBatchSize);
			//s_Lines.push_back(Line2D(from, to, color, strokeWidth, lifetime, onTop));
	}

	void DebugDraw::AddBox2D(glm::vec2& center, glm::vec2& dimensions, float rotation, float strokeWidth, glm::vec3 color, int lifetime, bool onTop)
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

	void DebugDraw::AddSprite(uint32 textureAssetId, glm::vec2 size, glm::vec2 position, glm::vec3 tint,
		glm::vec2 texCoordMin, glm::vec2 texCoordMax, float rotation, int lifetime, bool onTop)
	{
		// TODO: Possibly another offender
		//s_Sprites.push_back({ textureAssetId, size, position, tint, texCoordMin, texCoordMax, rotation, lifetime, onTop });
	}


	// ===================================================================================================================
	// Private methods
	// ===================================================================================================================
	void DebugDraw::RemoveDeadSprites()
	{
		auto spriteIter = s_Sprites.begin();
		while (spriteIter != s_Sprites.end())
		{
			if (spriteIter->BeginFrame() <= 0)
			{
				spriteIter = s_Sprites.erase(spriteIter);
			}
			else
			{
				spriteIter++;
			}
		}
	}

	void DebugDraw::RemoveDeadLines()
	{
		auto lineIter = s_Lines.begin();
		while (lineIter != s_Lines.end())
		{
			if (lineIter->BeginFrame() <= 0)
			{
				lineIter = s_Lines.erase(lineIter);
			}
			else
			{
				lineIter++;
			}
		}
	}

	void DebugDraw::AddSpritesToBatches()
	{
		//for (int i = (int)s_Sprites.size() - 1; i >= 0; i--)
		//{
		//	DebugSprite sprite = s_Sprites[i];
		//	bool wasAdded = false;
		//	bool spriteOnTop = sprite.m_OnTop;
		//	for (auto batch = NDynamicArray::Begin<RenderBatch>(s_Batches); batch != NDynamicArray::End<RenderBatch>(s_Batches); batch++)
		//	{
		//		if (batch->HasRoom() && (batch->HasTexture(sprite.m_TextureAssetId) || batch->HasTextureRoom()) && (spriteOnTop == batch->BatchOnTop()))
		//		{
		//			batch->Add(sprite.m_TextureAssetId, sprite.m_Size, sprite.m_Position, sprite.m_Tint, sprite.m_TexCoordMin, sprite.m_TexCoordMax, sprite.m_Rotation);
		//			wasAdded = true;
		//			break;
		//		}
		//	}

		//	if (!wasAdded)
		//	{
		//		RenderBatch newBatch = RenderBatch(s_MaxBatchSize, 0, spriteOnTop);
		//		newBatch.Start();
		//		newBatch.Add(sprite.m_TextureAssetId, sprite.m_Size, sprite.m_Position, sprite.m_Tint, sprite.m_TexCoordMin, sprite.m_TexCoordMax, sprite.m_Rotation);
		//		NDynamicArray::Add<RenderBatch>(s_Batches, newBatch);
		//		std::sort(NDynamicArray::Begin<RenderBatch>(s_Batches), NDynamicArray::End<RenderBatch>(s_Batches), RenderBatch::Compare);
		//	}
		//}
	}

	void DebugDraw::AddLinesToBatches()
	{
		//for (Line2D line : s_Lines)
		//{
		//	bool wasAdded = false;
		//	bool lineOnTop = line.IsOnTop();
		//	for (auto batch = NDynamicArray::Begin<RenderBatch>(s_Batches); batch != NDynamicArray::End<RenderBatch>(s_Batches); batch++)
		//	{
		//		if (batch->HasRoom() && (lineOnTop == batch->BatchOnTop()))
		//		{
		//			batch->Add(line.GetVerts(), line.GetColor());
		//			wasAdded = true;
		//			break;
		//		}
		//	}

		//	if (!wasAdded)
		//	{
		//		RenderBatch newBatch = RenderBatch(s_MaxBatchSize, 0, lineOnTop);
		//		newBatch.Start();
		//		newBatch.Add(line.GetMin(), line.GetMax(), line.GetColor());;
		//		NDynamicArray::Add<RenderBatch>(s_Batches, newBatch);
		//		std::sort(NDynamicArray::Begin<RenderBatch>(s_Batches), NDynamicArray::End<RenderBatch>(s_Batches), RenderBatch::Compare);
		//	}
		//}
	}


}