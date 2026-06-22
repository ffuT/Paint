#include "App.h"
#include <cstdlib>

int main(int argc, char* argv[]) {
    App app;
    if(!app.initialize(argc, argv))
        return EXIT_FAILURE;

    app.start();
    return EXIT_SUCCESS;
}