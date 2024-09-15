#ifndef ENTRY_POINT_H
#define ENTRY_POINT_H

#include "Application.h"
#include "core/logger/Log.h"

int main(int argc, char** argv) {
	Brotherhood::Log::Init();
	auto app = Brotherhood::CreateApplication();
	app->Run();
	delete app;

	return 0;
}

#endif // !ENTRY_POINT_H