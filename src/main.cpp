#include "App.h"

int main() {
    App app;
    if(app.initialize()){
        app.start();
        return 0;
    }
    return -1;
}