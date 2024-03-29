#pragma once

#include "cocoa/core/Application.h"

extern Cocoa::Application* Cocoa::createApplication();

int main()
{
	using namespace Cocoa;

	Application* application = createApplication();

	application->init();

	application->run();

	application->shutdown();

	delete application;
	return 0;
}