#pragma once
#include <filesystem>
#include <glm/fwd.hpp>
#include <GL/glew.h>

class ShaderProgram {
public:
	// you can add more constructors for pipeline with GS, TS etc.
	//ShaderProgram(void) = default;
	ShaderProgram(std::filesystem::path& VS_file, std::filesystem::path& FS_file);
	void activate(void);
	void deactivate(void);
	void clear(void);

	GLuint getId();

	void setUniform(const std::string& name, float val);
	void setUniform(const std::string& name, int val);
	void setUniform(const std::string& name, glm::vec3 val);
	void setUniform(const std::string& name, glm::vec4 val);
	void setUniform(const std::string& name, glm::mat4 val);
	void setUniform(const std::string& name, glm::mat3 val);


private:
	GLuint ID;
	std::string getShaderInfoLog(const GLuint obj);
	std::string getProgramInfoLog(const GLuint obj);
	std::string ReadFromFile(std::filesystem::path& path);
};
