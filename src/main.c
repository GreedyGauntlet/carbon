#include "core/entrypoint.h"
#include "core/application.h"

int main(int argc, char *argv[]) {
    PreloadExtensions();
    InitializeApplication();
    PostloadExtensions();
    RunApplication();
    DestroyApplication();
    return 0;
}
