#pragma once

// ===================================================================
// Entt
// ===================================================================
#include <entt/entt.hpp>

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

// ===================================================================
// ImGUI
// ===================================================================
#pragma warning( push )
#pragma warning ( disable : 26812 )
#pragma warning ( disable : 26451 )
#pragma warning ( disable : 6031 )
#pragma warning ( disable : 26495 )
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imstb_rectpack.h>
#pragma warning( pop )

// STB
#include <stb/stb_image.h>

// ===================================================================
// GLFW & GLAD
// ===================================================================
#include <glad/glad.h>
#include <GLFW/include/GLFW/glfw3.h>