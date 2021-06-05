#ifndef COCOA_ENGINE_CONTACT_LISTENER_H
#define COCOA_ENGINE_CONTACT_LISTENER_H
#include "externalLibs.h"

namespace Cocoa
{
	class ContactListener final : public b2ContactListener
	{
	public:
		void BeginContact(b2Contact* contact) override;

		void EndContact(b2Contact* contact) override;

		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;

		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
	};
}

#endif
