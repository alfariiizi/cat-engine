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
        return EXIT_FAILURE;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    /// Uncomment and then hover these macros to check the values
    // std::cout << SHADER_PATH << std::endl;
    // std::cout << VERTEX_SHADER_PATH << std::endl;
    // std::cout << FRAGMENT_SHADER_PATH << std::endl;
    // std::cout << ASSETS_PATH << std::endl;

    std::cout << "Running Successfully\n";

    return EXIT_SUCCESS;
}