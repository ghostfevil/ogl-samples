//**********************************
// OpenGL Samples Pack 
// ogl-samples.g-truc.net
//**********************************
// OpenGL Atomic Counter
// 23/07/2011 - 23/07/2011
//**********************************
// Christophe Riccio
// ogl-samples@g-truc.net
//**********************************
// G-Truc Creation
// www.g-truc.net
//**********************************

#include <glf/glf.hpp>

namespace
{
	std::string const SAMPLE_NAME = "OpenGL Atomic Counter";
	std::string const VERT_SHADER_SOURCE(glf::DATA_DIRECTORY + "420/atomic-counter.vert");
	std::string const FRAG_SHADER_SOURCE(glf::DATA_DIRECTORY + "420/atomic-counter.frag");
	std::string const TEXTURE_DIFFUSE(glf::DATA_DIRECTORY + "kueken256-rgba8.dds");
	int const SAMPLE_SIZE_WIDTH(640);
	int const SAMPLE_SIZE_HEIGHT(480);
	int const SAMPLE_MAJOR_VERSION(4);
	int const SAMPLE_MINOR_VERSION(1);

	glf::window Window(glm::ivec2(SAMPLE_SIZE_WIDTH, SAMPLE_SIZE_HEIGHT));

	GLsizei const VertexCount(4);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fv2f);
	glf::vertex_v2fv2f const VertexData[VertexCount] =
	{
		glf::vertex_v2fv2f(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f,-1.0f), glm::vec2(1.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2(-1.0f, 1.0f), glm::vec2(0.0f, 0.0f))
	};

	GLsizei const ElementCount(6);
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
	GLushort const ElementData[ElementCount] =
	{
		0, 1, 2, 
		2, 3, 0
	};

	namespace buffer
	{
		enum type
		{
			VERTEX,
			ELEMENT,
			TRANSFORM,
			MAX
		};
	}//namespace buffer

	GLuint PipelineName(0);
	GLuint VertexArrayName(0);
	GLuint ProgramName(0);
	GLuint BufferName[buffer::MAX] = {0, 0, 0};
	GLuint TextureName(0);
	GLuint AtomicCounter(0);

	GLint UniformMVP(0);
}//namespace

bool initProgram()
{
	bool Validated(true);
	
	glGenProgramPipelines(1, &PipelineName);
	glBindProgramPipeline(PipelineName);

	if(Validated)
	{
		GLuint VertShaderName = glf::createShader(GL_VERTEX_SHADER, VERT_SHADER_SOURCE);
		GLuint FragShaderName = glf::createShader(GL_FRAGMENT_SHADER, FRAG_SHADER_SOURCE);

		ProgramName = glCreateProgram();
		glProgramParameteri(ProgramName, GL_PROGRAM_SEPARABLE, GL_TRUE);
		glAttachShader(ProgramName, VertShaderName);
		glAttachShader(ProgramName, FragShaderName);
		glDeleteShader(VertShaderName);
		glDeleteShader(FragShaderName);

		glLinkProgram(ProgramName);
		Validated = glf::checkProgram(ProgramName);
	}

	if(Validated)
	{
		UniformMVP = glGetUniformLocation(ProgramName, "MVP");
	}

	if(Validated)
		glUseProgramStages(PipelineName, GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName);

	glBindProgramPipeline(0);

	return Validated;
}

bool initArrayBuffer()
{
	bool Validated(true);

	glGenBuffers(1, &BufferName[buffer::ELEMENT]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &BufferName[buffer::VERTEX]);
    glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
    glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return Validated;
}

bool initTexture2D()
{
	bool Validated(true);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &TextureName);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1000);

	gli::texture2D Image = gli::load(TEXTURE_DIFFUSE);
	for(std::size_t Level = 0; Level < Image.levels(); ++Level)
	{
		glTexImage2D(
			GL_TEXTURE_2D, 
			GLint(Level), 
			GL_RGBA8, 
			GLsizei(Image[Level].dimensions().x), 
			GLsizei(Image[Level].dimensions().y), 
			0,  
			GL_BGRA, 
			GL_UNSIGNED_BYTE, 
			Image[Level].data());
	}
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	return Validated;
}

bool initVertexArray()
{
	bool Validated(true);

	glGenVertexArrays(1, &VertexArrayName);
    glBindVertexArray(VertexArrayName);
		glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
		glVertexAttribPointer(glf::semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fv2f), GLF_BUFFER_OFFSET(0));
		glVertexAttribPointer(glf::semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fv2f), GLF_BUFFER_OFFSET(sizeof(glm::vec2)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(glf::semantic::attr::POSITION);
		glEnableVertexAttribArray(glf::semantic::attr::TEXCOORD);
	glBindVertexArray(0);

	return Validated;
}

bool initAtomicCounter()
{
	bool Validated(true);

	glGenBuffers(1, &AtomicCounter);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, AtomicCounter);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	return Validated;
}

bool initUniformBuffer()
{
	bool Validated(true);

	glGenBuffers(1, &BufferName[buffer::TRANSFORM]);
	glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), 0, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	return Validated;
}

bool initDebugOutput()
{
	bool Validated(true);

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	glDebugMessageCallbackARB(&glf::debugOutput, NULL);

	return Validated;
}

bool begin()
{
	bool Validated(true);
	Validated = Validated && glf::checkGLVersion(SAMPLE_MAJOR_VERSION, SAMPLE_MINOR_VERSION);

	if(Validated && glf::checkExtension("GL_ARB_debug_output"))
		Validated = initDebugOutput();
	if(Validated)
		Validated = initAtomicCounter();
	if(Validated)
		Validated = initTexture2D();
	if(Validated)
		Validated = initProgram();
	if(Validated)
		Validated = initArrayBuffer();
	if(Validated)
		Validated = initVertexArray();
	if(Validated)
		Validated = initUniformBuffer();

	return Validated;
}

bool end()
{
	bool Validated(true);

	glDeleteBuffers(buffer::MAX, BufferName);
	glDeleteProgram(ProgramName);
	glDeleteTextures(1, &TextureName);
	glDeleteVertexArrays(1, &VertexArrayName);

	return Validated;
}

void display()
{
	// Setup blending
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Compute the MVP (Model View Projection matrix)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);

		glm::mat4* Pointer = (glm::mat4*)glMapBufferRange(
			GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

		glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
		glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Window.TranlationCurrent.y));
		glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, Window.RotationCurrent.y, glm::vec3(1.f, 0.f, 0.f));
		glm::mat4 View = glm::rotate(ViewRotateX, Window.RotationCurrent.x, glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * View * Model;

		*Pointer = MVP;
	}

	{
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, AtomicCounter);

		glm::uint32* Pointer = (glm::uint32*)glMapBufferRange(
			GL_UNIFORM_BUFFER, 0, sizeof(glm::uint32),
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

		*Pointer = 0;
	}

	glViewportIndexedf(0, 0, 0, float(Window.Size.x), float(Window.Size.y));
	glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)[0]);

	glBindProgramPipeline(PipelineName);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureName);
	glBindVertexArray(VertexArrayName);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, AtomicCounter);
	glBindBufferBase(GL_UNIFORM_BUFFER, glf::semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);

	// Make sure the uniform buffer is uploaded
	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

	glDrawElementsInstancedBaseVertexBaseInstance(
		GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, 0, 5, 0, 0);

	glf::swapBuffers();
}

int main(int argc, char* argv[])
{
	return glf::run(
		argc, argv,
		glm::ivec2(::SAMPLE_SIZE_WIDTH, ::SAMPLE_SIZE_HEIGHT), 
		::SAMPLE_MAJOR_VERSION, 
		::SAMPLE_MINOR_VERSION);
}
