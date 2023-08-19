#ifndef COCOA_ENGINE_SCRIPT_H
#define COCOA_ENGINE_SCRIPT_H
#include "cocoa/core/Core.h"
#include "cocoa/core/EntityStruct.h"

namespace Cocoa
{
	class Script
	{
	public:		
		virtual void start(Entity entity) {}
		virtual void editorUpdate(Entity entity, float dt) {}

		virtual void update(Entity entity, float dt) {}
		virtual void imGui(Entity entity) {}

		virtual void beginContact(Entity other) {}
		virtual void endContact(Entity other) {}
	};
}

#endif
