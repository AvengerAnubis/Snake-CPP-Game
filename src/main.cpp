#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include <deque>
#include "shader.h"


//				   X  Y
#define SIZE_X 32
#define SIZE_Y 18
#define FIELD_SIZE SIZE_X*SIZE_Y

typedef struct _Snake
{
	std::deque<GLuint> body;

} Snake;

std::mutex mutex;

const char* v_shader =
"#version 330 core														\n"
"																		\n"
"layout (location = 0) in vec2 position;								\n"
"																		\n"
"void main()															\n"
"{																		\n"
"	gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);				\n"
"}																		\n"
;
const char* f_shader =
"#version 330 core																								\n"
"																												\n"
"out vec4 color;																								\n"
"uniform uint field[32 * 18];																					\n"
"																												\n"
"void main()																									\n"
"{																												\n"
"	int x = int(gl_FragCoord.x / 50);																			\n"
"	int y = 17 - int(gl_FragCoord.y / 50);																		\n"
"	float r = 0.0f, g = 0.0f, b = 0.0f;																			\n"
"	if (field[x + 32 * y] == 1u)																				\n"
"	{																											\n"
"		r = 0.3f;																								\n"
"		g = 0.1f;																								\n"
"		if (																									\n"
"			(int(gl_FragCoord.y) % 50 >= 20 && int(gl_FragCoord.y) % 50 <= 30)									\n"
"			||																									\n"
"			(int(gl_FragCoord.y) % 50 > 30 && int(gl_FragCoord.x) % 50 >= 20 && int(gl_FragCoord.x) % 50 <= 30)	\n"
"			||																									\n"
"			(int(gl_FragCoord.y) % 50 < 20 && (int(gl_FragCoord.x) % 50 <= 5 || int(gl_FragCoord.x) % 50 >= 45))\n"
"			||																									\n"
"			(int(gl_FragCoord.y) % 50 >= 45 || int(gl_FragCoord.y) % 50 <= 5)									\n"
"			)																									\n"
"		{																										\n"
"			r = 0.5f;																							\n"
"			g = 0.5f;																							\n"
"			b = 0.5f;																							\n"
"		}																										\n"
"	}																											\n"
"	else if (field[x + 32 * y] == 2u)																			\n"
"	{																											\n"
"		g = 1.0f;																								\n"
"	}																											\n"
"	else if (field[x + 32 * y] == 3u)																			\n"
"	{																											\n"
"		r = 1.0f;																								\n"
"	}																											\n"
"																												\n"
"	color = vec4(																								\n"
"		r,																										\n"
"		g,																										\n"
"		b,																										\n"
"		1.0f																									\n"
"	);																											\n"
"}																												\n"
;

enum KEYS
{
	W,
	A,
	S,
	D
};

GLfloat square[] =
{
	//	X		Y	
		1.0f,	1.0f,	//Right Up
		1.0f,	-1.0f,	//Right Down
		-1.0f,	-1.0f,	//Left Down
		-1.0f,	1.0f,	//Left Up
};
GLuint indexes[] =
{
	3, 0, 1,
	3, 1, 2
};

GLuint field[FIELD_SIZE];

Snake snake;

bool keys[]
{
	true,	//W
	false,	//A
	false,	//S
	false	//D
};

void init_field()
{
	std::ifstream file("level.lvl");
	std::string data;
	std::string line;
	snake = Snake();
	keys[W] = true;
	keys[A] = false;
	keys[S] = false;
	keys[D] = false;


	while (std::getline(file, line))
	{
		data += line;
	}
	bool is_snake_intialized = false;
	for (int i = 0; i < FIELD_SIZE; ++i)
	{
		field[i] = data[i] - '0';
		if (field[i] == 2 && !is_snake_intialized)
		{
			is_snake_intialized = true;
			snake.body.push_front(i);
			if (i % SIZE_X >= SIZE_X)
			{
				snake.body.push_front(i - 1);
			}
			else
			{
				snake.body.push_front(i + 1);
			}
			continue;
		}
		if (field[i] == 2)
		{
			field[i] = 0;
		}
	}
	for (auto el : snake.body)
	{
		field[el] = 2;
	}
	file.close();
}

GLFWwindow* init_window()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(1600, 900, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return nullptr;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);
	return window;
}
GLuint createVAO()
{
	GLuint vbo, vao, ebo;

	//Генерируем буферы (Vertex Buffer Object, Element Buffer Object, Vertex Array Object)
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glGenVertexArrays(1, &vao);

	//Биндим текущий объект, для его настройки
	glBindVertexArray(vao);

		//Привязываем буфер 
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);

		//Привязываем буфер индексов
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

		//Добавляем указатели на вершины
		//(позиция в шейдере [layout (location = 0)], размер (vec2), тип данных, нормализация, смещение шага, отступ от начала)
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

	//Отключаем текущий объект
	glBindVertexArray(0);

	return vao;
}
void render(GLFWwindow* window, Shader& shader, GLuint vao)
{
	glUseProgram(shader.id());

	{
		glUniform1uiv(glGetUniformLocation(shader.id(), "field"), FIELD_SIZE, field);
	}
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glfwSwapBuffers(window);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::lock_guard<std::mutex> lock(mutex);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		keys[W] = true;
		keys[A] = false;
		keys[S] = false;
		keys[D] = false;
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		keys[W] = false;
		keys[A] = true;
		keys[S] = false;
		keys[D] = false;
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		keys[W] = false;
		keys[A] = false;
		keys[S] = true;
		keys[D] = false;
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		keys[W] = false;
		keys[A] = false;
		keys[S] = false;
		keys[D] = true;
	}
}

bool run = true;
void process_field()
{
	while (run)
	{
		auto begin = std::chrono::steady_clock::now();
		{
			std::lock_guard<std::mutex> lock(mutex);

			if (keys[W])
			{
				snake.body.push_front(snake.body.front() - SIZE_X);
				if (snake.body.front() / SIZE_X > SIZE_Y)
				{
					init_field();
					continue;
				}
			}
			else if (keys[A])
			{
				snake.body.push_front(snake.body.front() - 1);
				if (snake.body.front() % SIZE_X > SIZE_X)
				{
					init_field();
					continue;
				}
			}
			else if (keys[S])
			{
				snake.body.push_front(snake.body.front() + SIZE_X);
				if (snake.body.front() / SIZE_X > SIZE_Y)
				{
					init_field();
					continue;
				}
			}
			else if (keys[D])
			{
				snake.body.push_front(snake.body.front() + 1);
				if (snake.body.front() % SIZE_X > SIZE_X)
				{
					init_field();
					continue;
				}
			}

			if (field[snake.body.front()] != 3)
			{
				field[snake.body.back()] = 0;
				snake.body.pop_back();
				if (field[snake.body.front()] != 0)
				{
					init_field();
					continue;
				}
			}
			else
			{
				srand(std::chrono::system_clock::now().time_since_epoch().count());
				int random = rand() % FIELD_SIZE;
				while (field[random] != 0)
				{
					random = rand() % FIELD_SIZE;
				}
				field[random] = 3;
			}
			field[snake.body.front()] = 2;
		}
		auto end = std::chrono::steady_clock::now();
		std::this_thread::sleep_for(
			std::chrono::milliseconds(250) - 
			std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
		);
	}
}

int main()
{
	init_field();
	GLFWwindow* window = init_window();
	if (!window)
	{
		return -1;
	}

	Shader shader(v_shader, f_shader);

	GLuint vao = createVAO();

	std::thread logic(process_field);
	glfwSetKeyCallback(window, key_callback);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		render(window, shader, vao);
	}
	run = false;

	logic.join();
	glfwTerminate();

	return 0;
}