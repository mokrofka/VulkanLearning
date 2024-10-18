
#include "defines.h"

#include "App.h"

i32 main() {
	App app;

	try {
		app.run();
	} catch (const std::exception &e) {
		printf("%s\n", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
