#include "jade/systems/RenderSystem.h"

#include "jade/util/Log.h"

void RenderSystem::AddActor(entt::actor& actor) {
    bool hasSprite = actor.has<SpriteRenderer>();
    
    if (hasSprite) {
        SpriteRenderer spr = actor.get<SpriteRenderer>();
        bool wasAdded = false;
        for (RenderBatch* batch : m_Batches) {
            if (batch->HasRoom()) {
                batch->Add(actor);
                wasAdded = true;
                Log::Info("Entity added to batch.");
                break;
            }
        }

        if (!wasAdded) {
            Log::Info("New batch created for entity.");
            RenderBatch* newBatch = new RenderBatch();
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