#pragma once

#include "jade/physics2d/rigidbody/Rigidbody2D.h"

namespace Jade
{
	struct Circle
	{
		float m_Radius = 1.0f;
		float m_InertiaTensor = 0.0f;
		Rigidbody2D* m_Rigidbody = nullptr;
	};
}