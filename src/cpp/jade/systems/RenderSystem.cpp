#include "jade/systems/RenderSystem.h"

#include "jade/util/Log.h"

void RenderSystem::AddEntity(entt::entity& entity) {
    bool hasSprite = m_Registry.has<SpriteRenderer>(entity);
    
    if (hasSprite) {
        SpriteRenderer spr = m_Registry.get<SpriteRenderer>(entity);
        Sprite* sprite = spr.m_Sprite;
        bool wasAdded = false;
        for (RenderBatch* batch : m_Batches) {
            if (batch->HasRoom()) {
                Texture* tex = sprite->m_Texture;
                if (tex == nullptr || (batch->HasTexture(tex) || batch->HasTextureRoom())) {
                    batch->Add(entity);
                    wasAdded = true;
                    break;
                }
            }
        }

        if (!wasAdded) {
            RenderBatch* newBatch = new RenderBatch(this);
            newBatch->Start();
            newBatch->Add(entity);
            m_Batches.push_back(newBatch);
        }
    }
}

void RenderSystem::Render() {
    for (RenderBatch* batch : m_Batches) {
        batch->Render();
    }
}