#pragma once
#include "externalLibs.h"

namespace Jade {
    namespace Settings
    {
        class General
        {
        public:
            static int s_GridSizeX;
            static int s_GridSizeY;
            static bool s_DrawGrid;
            static std::string s_CurrentSaveFile;
        };
  
        class Physics2D
        {
        public:
            static int s_VelocityIterations;
            static int s_PositionIterations;
            static float s_Timestep;
        };
    }
}