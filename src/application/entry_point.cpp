extern "C" {
#include "gbCore.h"
#include "log.h"
}

#include <SDL3/SDL.h>
#include <imgui.h>


#include <stdio.h>

#include <iostream>
#include "App.hpp"

namespace GeimBoi {
int main();
}

int GeimBoi::main()
{
    GeimBoi::App app;
    app.run();
	return 0;
}
