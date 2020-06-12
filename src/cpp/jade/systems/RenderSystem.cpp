#include "jade/systems/RenderSystem.h"

#include "jade/util/Log.h"

void RenderSystem::AddActor(entt::actor& actor) {
    bool hasSprite = actor.has<SpriteRenderer>();
    
    if (hasSprite) {
        SpriteRenderer spr = actor.get<SpriteRenderer>();
        Sprite* sprite = spr.m_Sprite;
        bool wasAdded = false;
        for (RenderBatch* batch : m_Batches) {
            if (batch->HasRoom()) {
                Texture* tex = sprite->m_Texture;
                if (tex == nullptr || (batch->HasTexture(tex) || batch->HasTextureRoom())) {
                    batch->Add(actor);
                    wasAdded = true;
                    break;
                }
            }
        }

        if (!wasAdded) {
            RenderBatch* newBatch = new RenderBatch(this);
            newBatch->Start();
            newBatch->Add(actor);
            m_Batches.push_back(newBatch);
        }
    }
}

void RenderSystem::Render() {
    for (RenderBatch* batch : m_Batches) {
        batch->Render();
    }
}