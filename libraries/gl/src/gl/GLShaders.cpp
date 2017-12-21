#include "GLShaders.h"

#include "GLLogging.h"
#include <sstream>

namespace gl {

std::vector<std::string> splitStringLines(const std::string& src) {
    std::vector<std::string> lines;
    std::istringstream srcStream(src);
    std::string currentLine;
    while (!srcStream.eof()) {
        getline(srcStream, currentLine);
        lines.emplace_back(currentLine);
    }
    return lines;
}

std::string getStringLineNum(const std::string& src, int lineNum, int offset = 0) {
    std::istringstream srcStream(src);
    int currentNum = offset;
    std::string currentLine;
    while (!srcStream.eof() && (currentNum < lineNum)) {
        getline(srcStream, currentLine);
        currentNum++;
    }
    return currentLine;
}

#ifdef SEPARATE_PROGRAM
    bool compileShader(GLenum shaderDomain, const std::string& shaderSource, const std::string& defines, GLuint &shaderObject, GLuint &programObject, std::string& error) {
#else
    bool compileShader(GLenum shaderDomain, const std::string& shaderSource, const std::string& defines, GLuint &shaderObject, std::string& error) {
#endif
    if (shaderSource.empty()) {
        qCDebug(glLogging) << "GLShader::compileShader - no GLSL shader source code ? so failed to create";
        return false;
    }

    // Create the shader object
    GLuint glshader = glCreateShader(shaderDomain);
    if (!glshader) {
        qCDebug(glLogging) << "GLShader::compileShader - failed to create the gl shader object";
        return false;
    }

    // Assign the source
    const int NUM_SOURCE_STRINGS = 2;
    const GLchar* srcstr[] = { defines.c_str(), shaderSource.c_str() };
    glShaderSource(glshader, NUM_SOURCE_STRINGS, srcstr, NULL);

    // Compile !
    glCompileShader(glshader);

    // check if shader compiled
    GLint compiled = 0;
    glGetShaderiv(glshader, GL_COMPILE_STATUS, &compiled);

    // if compilation fails
    if (!compiled) {

        // save the source code to a temp file so we can debug easily
        /*
        std::ofstream filestream;
        filestream.open("debugshader.glsl");
        if (filestream.is_open()) {
        filestream << srcstr[0];
        filestream << srcstr[1];
        filestream.close();
        }
        */

        GLint infoLength = 0;
        glGetShaderiv(glshader, GL_INFO_LOG_LENGTH, &infoLength);

        char* temp = new char[infoLength];
        glGetShaderInfoLog(glshader, infoLength, NULL, temp);


        /*
        filestream.open("debugshader.glsl.info.txt");
        if (filestream.is_open()) {
        filestream << std::string(temp);
        filestream.close();
        }
        */

        qCWarning(glLogging) << "GLShader::compileShader - failed to compile the gl shader object:";
        for (auto s : srcstr) {
            qCWarning(glLogging) << s;
        }
        qCWarning(glLogging) << "GLShader::compileShader - errors:";

        std::istringstream errorStream(temp);
        std::string line;
        std::string  srcDefine(srcstr[0]);
        std::string  srcCode(srcstr[1]);
        auto definesLines = splitStringLines(srcDefine);
        auto srcLines = splitStringLines(srcCode);


        while (!errorStream.eof()) {
            std::getline(errorStream, line);
            qCWarning(glLogging) << line.c_str();

            if ((line[0] == '0') && (line[1] == '(')) {
                int srcLineNum = (int)(std::stoi(line.substr(2)) - definesLines.size());
                if (srcLineNum >= 0 && srcLineNum < srcLines.size()) {
                    qCWarning(glLogging) << srcLines[srcLineNum].c_str();
                }
            }
        }

        error = std::string(temp);
        delete[] temp;

        glDeleteShader(glshader);
        return false;
    }

#ifdef SEPARATE_PROGRAM
    GLuint glprogram = 0;
    // so far so good, program is almost done, need to link:
    GLuint glprogram = glCreateProgram();
    if (!glprogram) {
        qCDebug(glLogging) << "GLShader::compileShader - failed to create the gl shader & gl program object";
        return false;
    }

    glProgramParameteri(glprogram, GL_PROGRAM_SEPARABLE, GL_TRUE);
    glAttachShader(glprogram, glshader);
    glLinkProgram(glprogram);

    GLint linked = 0;
    glGetProgramiv(glprogram, GL_LINK_STATUS, &linked);

    if (!linked) {
        /*
        // save the source code to a temp file so we can debug easily
        std::ofstream filestream;
        filestream.open("debugshader.glsl");
        if (filestream.is_open()) {
        filestream << shaderSource->source;
        filestream.close();
        }
        */

        GLint infoLength = 0;
        glGetProgramiv(glprogram, GL_INFO_LOG_LENGTH, &infoLength);

        char* temp = new char[infoLength];
        glGetProgramInfoLog(glprogram, infoLength, NULL, temp);

        qCDebug(glLogging) << "GLShader::compileShader -  failed to LINK the gl program object :";
        qCDebug(glLogging) << temp;

        /*
        filestream.open("debugshader.glsl.info.txt");
        if (filestream.is_open()) {
        filestream << String(temp);
        filestream.close();
        }
        */
        delete[] temp;

        glDeleteShader(glshader);
        glDeleteProgram(glprogram);
        return false;
    }
    programObject = glprogram;
#endif
    shaderObject = glshader;
    return true;
}

GLuint compileProgram(const std::vector<GLuint>& glshaders, std::string& error) {
    // A brand new program:
    GLuint glprogram = glCreateProgram();
    if (!glprogram) {
        qCDebug(glLogging) << "GLShader::compileProgram - failed to create the gl program object";
        return 0;
    }

    // glProgramParameteri(glprogram, GL_PROGRAM_, GL_TRUE);
    // Create the program from the sub shaders
    for (auto so : glshaders) {
        glAttachShader(glprogram, so);
    }

    // Link!
    glLinkProgram(glprogram);

    GLint linked = 0;
    glGetProgramiv(glprogram, GL_LINK_STATUS, &linked);

    if (!linked) {
        /*
        // save the source code to a temp file so we can debug easily
        std::ofstream filestream;
        filestream.open("debugshader.glsl");
        if (filestream.is_open()) {
        filestream << shaderSource->source;
        filestream.close();
        }
        */

        GLint infoLength = 0;
        glGetProgramiv(glprogram, GL_INFO_LOG_LENGTH, &infoLength);

        char* temp = new char[infoLength];
        glGetProgramInfoLog(glprogram, infoLength, NULL, temp);

        qCDebug(glLogging) << "GLShader::compileProgram -  failed to LINK the gl program object :";
        qCDebug(glLogging) << temp;

        error = std::string(temp);
        delete[] temp;

        /*
        filestream.open("debugshader.glsl.info.txt");
        if (filestream.is_open()) {
        filestream << std::string(temp);
        filestream.close();
        }
        */

        glDeleteProgram(glprogram);
        return 0;
    }

    return glprogram;
}

}
