#include "cocoa/physics2d/ContactListener.h"
#include "cocoa/core/Entity.h"
#include "cocoa/systems/ScriptSystem.h"

namespace Cocoa
{
	void ContactListener::BeginContact(b2Contact* contact)
	{
		entt::entity* rawHandleARef = (entt::entity*)contact->GetFixtureA()->GetBody()->GetUserData();
		entt::entity* rawHandleBRef = (entt::entity*)contact->GetFixtureB()->GetBody()->GetUserData();
		ScriptSystem::notifyBeginContact(NEntity::createEntity(*rawHandleARef), NEntity::createEntity(*rawHandleBRef));
	}

	void ContactListener::EndContact(b2Contact* contact)
	{
		// TODO: See why this throws nullptr exception on scene stop
		void* userDataA = contact->GetFixtureA()->GetBody()->GetUserData();
		void* userDataB = contact->GetFixtureB()->GetBody()->GetUserData();
		if (userDataA && userDataB)
		{
			entt::entity* rawHandleARef = (entt::entity*)userDataA;
			entt::entity* rawHandleBRef = (entt::entity*)userDataB;
			ScriptSystem::notifyEndContact(NEntity::createEntity(*rawHandleARef), NEntity::createEntity(*rawHandleBRef));
		}
	}

	void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
	{
		// TODO: Implement me
	}

	void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
	{
		// TODO: Implement me
	}
}