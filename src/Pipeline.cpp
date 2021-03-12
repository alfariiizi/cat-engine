#include "Pipeline.hpp"

#include <fstream>

void Pipeline::create() 
{
    assert( _hasBeenInit_ );

    auto vertCode = readFile( *_pVertFile_ );
    auto fragCode = readFile( *_pFragFile_ );
    
}

std::vector<char> Pipeline::readFile( const std::string& fileName )
{
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("FAILED TO OPEN SHADER FILE\n");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}
