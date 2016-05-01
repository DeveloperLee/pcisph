#pragma once
#include "utils/Def.h"
namespace cs224{
    class ResourceLoader
    {
    public:
        ResourceLoader();
        //static GLuint loadShaders(const char * vertex_file_path,const char * fragment_file_path);
        static GLuint loadShadersFromText(const char * vertex_file,const char * fragment_file);
        static std::string fileToString(std::string filename);
    };
}


