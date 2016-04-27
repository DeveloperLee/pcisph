#include "ResourceLoader.h"


namespace cs224{

    ResourceLoader::ResourceLoader()
    {
    }

//    GLuint ResourceLoader::loadShaders(const char * vertex_file_path,const char * fragment_file_path){

//        // Create the shaders
//        GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
//        GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

//        // Read the Vertex Shader code from the file
//        std::string VertexShaderCode;
//        QString vertFilePath = QString(vertex_file_path);
//        QFile vertFile(vertFilePath);
//        if (vertFile.open(QIODevice::ReadOnly | QIODevice::Text)){
//            QTextStream vertStream(&vertFile);
//            VertexShaderCode = vertStream.readAll().toStdString();
//        }

//        // Read fragment shader code from file
//        std::string FragmentShaderCode;
//        QString fragFilePath = QString(fragment_file_path);
//        QFile fragFile(fragFilePath);
//        if (fragFile.open(QIODevice::ReadOnly | QIODevice::Text)){
//            QTextStream fragStream(&fragFile);
//            FragmentShaderCode = fragStream.readAll().toStdString();
//        }

//        GLint Result = GL_FALSE;
//        int InfoLogLength;

//        // Compile Vertex Shader
//        char const * VertexSourcePointer = VertexShaderCode.c_str();
//        glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
//        glCompileShader(VertexShaderID);

//        // Check Vertex Shader
//        glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
//        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//        if (!Result && InfoLogLength > 0) {
//            std::vector<char> VertexShaderErrorMessage(InfoLogLength);
//            glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
//            fprintf(stderr, "Error compiling shader: %s\n%s\n",
//                    vertex_file_path, &VertexShaderErrorMessage[0]);
//        }

//        // Compile Fragment Shader
//        char const * FragmentSourcePointer = FragmentShaderCode.c_str();
//        glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
//        glCompileShader(FragmentShaderID);

//        // Check Fragment Shader
//        glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
//        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//        if (!Result && InfoLogLength > 0) {
//            std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
//            glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
//            fprintf(stderr, "Error compiling shader: %s\n%s\n",
//                    fragment_file_path, &FragmentShaderErrorMessage[0]);
//        }

//        // Link the program
//        GLuint programId = glCreateProgram();
//        glAttachShader(programId, VertexShaderID);
//        glAttachShader(programId, FragmentShaderID);
//        glLinkProgram(programId);

//        // Check the program
//        glGetProgramiv(programId, GL_LINK_STATUS, &Result);
//        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &InfoLogLength);
//        if (!Result && InfoLogLength > 0) {
//            std::vector<char> ProgramErrorMessage(InfoLogLength);
//            glGetProgramInfoLog(programId, InfoLogLength, NULL, &ProgramErrorMessage[0]);
//            fprintf(stderr, "Error linking shader: %s\n", &ProgramErrorMessage[0]);
//        }

//        glDeleteShader(VertexShaderID);
//        glDeleteShader(FragmentShaderID);

//        return programId;
//    }

    GLuint ResourceLoader::loadShadersFromText(const char *vertex_file, const char *fragment_file)
    {
                GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
                GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
        GLint Result = GL_FALSE;
        int InfoLogLength;

        // Compile Vertex Shader
        char const * VertexSourcePointer = vertex_file;
        glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
        glCompileShader(VertexShaderID);

        // Check Vertex Shader
        glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (!Result && InfoLogLength > 0) {
            std::vector<char> VertexShaderErrorMessage(InfoLogLength);
            glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
            fprintf(stderr, "Error compiling shader: %s\n%s\n",
                    vertex_file, &VertexShaderErrorMessage[0]);
        }

        // Compile Fragment Shader
        char const * FragmentSourcePointer = fragment_file;
        glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
        glCompileShader(FragmentShaderID);

        // Check Fragment Shader
        glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (!Result && InfoLogLength > 0) {
            std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
            glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
            fprintf(stderr, "Error compiling shader: %s\n%s\n",
                    fragment_file, &FragmentShaderErrorMessage[0]);
        }

        // Link the program
        GLuint programId = glCreateProgram();
        glAttachShader(programId, VertexShaderID);
        glAttachShader(programId, FragmentShaderID);
        glLinkProgram(programId);

        // Check the program
        glGetProgramiv(programId, GL_LINK_STATUS, &Result);
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (!Result && InfoLogLength > 0) {
            std::vector<char> ProgramErrorMessage(InfoLogLength);
            glGetProgramInfoLog(programId, InfoLogLength, NULL, &ProgramErrorMessage[0]);
            fprintf(stderr, "Error linking shader: %s\n", &ProgramErrorMessage[0]);
        }

        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);

        return programId;
    }
}
