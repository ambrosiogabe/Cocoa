#pragma once

// ===================================================================
// Json Library
// ===================================================================
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// ===================================================================
// GLM
// ===================================================================
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/matrix.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/type_ptr.hpp>

// ===================================================================
// Box2D
// ===================================================================
#include <box2D/b2_body.h>
#include <box2D/b2_shape.h>
#include <box2D/b2_polygon_shape.h>
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>

// ===================================================================
// Entt
// ===================================================================
#define ENTT_STANDARD_CPP
#include <entt/entt.hpp>

// ===================================================================
// Std Library
// ===================================================================
#include <string>
#include <vector>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <stdlib.h>
#include <iostream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <functional>
#include <typeinfo>

// STB
#include <stb/stb_image.h>

// Make sure to include windows before glfw
#ifdef _WIN32
#include <windows.h>
#undef DeleteFile
#undef CopyFile
#undef CreateFile
#undef GetSaveFileName
#undef GetOpenFileName
#endif

// ===================================================================
// GLFW & GLAD
// ===================================================================
#include <glad/glad.h>
#include <GLFW/include/GLFW/glfw3.h>

// ===================================================================
// Cpp Utils
// ===================================================================
#include <CppUtils/CppUtils.h>
using namespace CppUtils;

// Some core defines and stuff
#include "cocoa/core/Core.h"