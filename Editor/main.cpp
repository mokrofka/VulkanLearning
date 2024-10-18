#include "defines.h"

#include "App.h"

void compileShaders() {
	const char* batFilePath = PROJECT_DIR"compile_shaders.bat";

	b8 result = system(batFilePath);

	// Check the result
	if (result != 0) {
		printf("Failed to execute batch file. Error code: %d\n", result);
	}
}

i32 main() {
	compileShaders();
		
	App app;

	try {
		app.run();
	} catch (const std::exception& e) {
		printf("%s\n", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
