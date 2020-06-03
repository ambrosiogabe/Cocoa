#pragma once

#include "jade/renderer/RenderBatch.h"

#include <vector>

class RenderBatch;
class RenderSystem {
public:
    RenderSystem();

public:
    static const int MAX_BATCH_SIZE;

private:
    std::vector<RenderBatch*> m_Batches;
};

const int RenderSystem::MAX_BATCH_SIZE = 100;