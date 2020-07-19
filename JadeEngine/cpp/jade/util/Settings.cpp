#include "jade/util/Settings.h"

namespace Jade
{
    namespace Settings
    {
        int General::s_GridSizeX = 32;
        int General::s_GridSizeY = 32;
        bool General::s_DrawGrid = false;
        std::string General::s_CurrentSaveFile = "New Scene.jade";

        int Physics2D::s_PositionIterations = 3;
        int Physics2D::s_VelocityIterations = 8;
        float Physics2D::s_Timestep = 1.0f / 60.0f;
    }
}