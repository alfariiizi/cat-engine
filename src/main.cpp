#include "Engine.hpp"

#include <iostream>


int main()
{
    try
    {
        Engine engine;
        engine.loop();
    }
    catch( const vk::SystemError& err )
    {
        std::cerr << err.what() << '\n';
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    std::cout << "Running Successfully\n";

    return 0;
}