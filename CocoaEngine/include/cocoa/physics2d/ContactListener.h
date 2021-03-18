#pragma once
#include "externalLibs.h"

namespace Cocoa
{
	class ContactListener : public b2ContactListener
	{
	public:
		void BeginContact(b2Contact* contact) override;

		void EndContact(b2Contact* contact) override;

		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;

		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
	};
}