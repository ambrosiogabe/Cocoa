#pragma once
#include "cocoa/core/Core.h"
#include "cocoa/util/Log.h"
#include "cocoa/components/Script.h"

namespace Cocoa
{
	UCLASS();
	class DefaultScript : public Script
	{
	public:
		virtual void Start() override;
		virtual void Update(float dt) override;

	public:
		UPROPERTY();
		float m_ExampleFloatProperty = 3.14f;

		UPROPERTY();
		int m_ExampleIntProperty = 10;
	};
}