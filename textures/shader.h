#pragma once

#include <GL/glew.h>

#include <string>
#include <fstream>
#include <sstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	unsigned int id;

	Shader() = default;

	Shader (const char* vertexPath, const char* fragmentPath)
	{
		loadShaders(vertexPath, fragmentPath);
	}

	~Shader()
	{
		glDeleteProgram(id);
	}

	void loadShaders(const char* vertexPath, const char* fragmentPath)
	{
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;

		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try 
		{
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;

			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			vShaderFile.close();
			fShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch(const std::ifstream::failure& e)
		{
			std::cout << "Could not read shader file" << std::endl;
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		unsigned int vertex, fragment;
		int success;
		char infoLog[512];

		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);

		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	
		if (!success) {
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cerr << "Failed to compile vertex shader\n" << infoLog << std::endl;
		}
	
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	
		if (!success) {
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cerr << "Failed to compile fragment shader\n" << infoLog << std::endl;
		}
	
		id = glCreateProgram();
		glAttachShader(id, vertex);
		glAttachShader(id, fragment);
		glLinkProgram(id);
	
		glGetProgramiv(id, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(id, 512, NULL, infoLog);
			std::cerr << "Failed to link program\n" << infoLog << std::endl;
		}
	
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	void use()
	{
		glUseProgram(id);
	}

	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
	}

	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), value);
	}

	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(id, name.c_str()), value);
	}

	void setFloat4(const std::string& name, float v1, float v2, float v3, float v4) const
	{
		glUniform4f(glGetUniformLocation(id, name.c_str()), v1, v2, v3, v4);
	}
};
