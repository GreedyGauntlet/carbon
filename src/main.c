#include "core/entrypoint.h"
#include "core/application.h"

int main(int argc, char *argv[]) {
    PreloadExtensions();
    InitializeApplication();
    RunApplication();
    DestroyApplication();
    return 0;
}
