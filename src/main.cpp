#include "Application.h"
#include <iostream> // For initial error message

int main(int argc, char* argv[]) {
    Application app;

    try {
        if (app.Initialize()) {
            app.Run(); // Contains the main loop
        } else {
             std::cerr << "FATAL: Application initialization failed!" << std::endl;
             // Shutdown might have already cleaned up some parts,
             // but call it again to be safe (it handles null checks)
             app.Shutdown();
             return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "FATAL EXCEPTION: " << e.what() << std::endl;
        app.Shutdown(); // Attempt cleanup
        return 1;
    } catch (...) {
        std::cerr << "FATAL UNKNOWN EXCEPTION occurred!" << std::endl;
        app.Shutdown(); // Attempt cleanup
        return 1;
    }


    // Application destructor will call Shutdown() automatically
    // if Run() exits normally (or if Initialize failed and Run wasn't called)

    return 0; // Success
}