#include "App.h"
#include <cstdlib>

int main() {
    App app;
    if(!app.initialize())
        return EXIT_FAILURE;

    app.start();
    return EXIT_SUCCESS;
}