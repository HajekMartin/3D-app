#include "App.h";
// define our application
App app;

int main()
{
    App app = App();
    if (app.init())
        return app.run();
}

