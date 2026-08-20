#include "core/entrypoint.h"
#include "core/application.h"
#include "data/tester.h"

int main(int argc, char *argv[]) {
    if (argc == 2) {
        LoadTester(argv[1]);
    } else if (argc > 2) {
        EZ_ERROR("Cannot start with invalid arguments");
        return -1;
    }
    PreloadExtensions();
    InitializeApplication();
    PostloadExtensions();
    RunApplication();
    DestroyApplication();
    CleanTester();
    return 0;
}
