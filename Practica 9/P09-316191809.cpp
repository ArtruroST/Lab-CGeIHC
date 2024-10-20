/*
Semestre 2024-1
Animación:
Sesión 1:
Simple o básica:Por banderas y condicionales (más de 1 transforomación geométrica se ve modificada
Sesión 2
Compleja: Por medio de funciones y algoritmos.
Adicional.- ,Textura Animada
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include <cstdlib> 
#include <ctime> 
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animación
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;

//Letrero
float movLetrero;
float rotLetrero;

float movOffLetrero;
float rotOffLetrero;

//Dado
float movDado;
float rotDadoY;
float rotDadoZ;

float movOffDado;
float rotOffDadoY;
float rotOffDadoZ;

//Avion
float movAvionX;
float movAvionY;
float rotAvion;

float movOffAvionX;
float movOffAvionY;
float rotOffAvion;

float aux;

bool avanza;
bool mueve;
bool vuela;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;

Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;
Model Letrero;
Model Marco;
Model Dado;
Model Tablero;
Model Avion;


Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int generarNumeroAleatorio() {
	return std::rand() % 10 + 1;
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Letrero = Model();
	Letrero.LoadModel("Models/Letrero.obj");
	Marco = Model();
	Marco.LoadModel("Models/Marco.obj");
	Dado = Model();
	Dado.LoadModel("Models/Dado.obj");
	Tablero = Model();
	Tablero.LoadModel("Models/Tablero.obj");
	Avion = Model();
	Avion.LoadModel("Models/Avion.obj");

	

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;
	
	//se crean mas luces puntuales y spotlight 

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	movCoche = 0.0f;
	movOffset = 0.5f;
	rotllanta = 0.0f;
	rotllantaOffset = 5.0f;

	//Letrero
	movLetrero = 6.0f;
	movOffLetrero = 1.0f;

	rotLetrero = 0.0f;
	rotOffLetrero = 6.0f;

	//Dado
	movDado = 15.0f;
	movOffDado = 0.5f;
	
	rotDadoY = 0.0f;
	rotOffDadoY = 1.0f;

	rotDadoZ = 0.0f;
	rotOffDadoZ = 1.0f;

	//Avion
	movAvionX = 20.0f;
	movAvionY = 36.0f;
	rotAvion = 0.0f;

	movOffAvionX = 0.3f;
	movOffAvionY = 0.3f;
	rotOffAvion = 0.8f;


	aux = 0;

	mueve = true;
	avanza = true;
	vuela = true;
	glfwSetTime(0);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

	/*	printf("time %f \n", glfwGetTime());*/
		if (glfwGetTime() > 5)
		{
			if (avanza)
			{
				if (movCoche > -15.0f)
				{
					movCoche -= movOffset * deltaTime;
					//printf("avanza%f \n ",movCoche);
					rotllanta += rotllantaOffset * deltaTime;

				}
				else
				{
					avanza = !avanza;
				}
			}
			else
			{
				if (movCoche < 280.0f)
				{
					movCoche += movOffset * deltaTime;
					rotllanta -= rotllantaOffset * deltaTime;

				}
				else
				{
					avanza = !avanza;
				}

			}
		}

		//Funcionando
		if (mueve)
		{
			if (rotLetrero <= 360)
			{
				rotLetrero += rotOffLetrero * deltaTime;
			}

			else
			{
				if (movLetrero > 0)
				{
					movLetrero -= movOffLetrero * deltaTime;
				}
				else
				{
					rotLetrero = 0.0f;
					mueve = !mueve;
				}
			}
		}
		else
		{
			if (rotLetrero <= 360)
			{
				rotLetrero += rotOffLetrero * deltaTime;
			}

			else
			{
				if (movLetrero < 6)
				{
					movLetrero += movOffLetrero * deltaTime;
				}
				else
				{
					rotLetrero = 0.0f;
					mueve = !mueve;
				}
			}
		}

		// 2 segundos
		/*if (aux == 0)
		{
			if (rotLetrero <= 360 and static_cast<int>(glfwGetTime()) % 2 != 0)
			{
				rotLetrero += rotOffLetrero * deltaTime;
			}
			else
			{
				rotLetrero = 0.0f;
				aux = 1;
			}
		}

		if (aux == 1)
		{
			if (movLetrero > 0 and static_cast<int>(glfwGetTime()) % 2 != 0)
			{
				movLetrero -= movOffLetrero * deltaTime;
			}
			else
			{
				aux = 2;
			}
		}

		if (aux == 2)
		{
			if (movLetrero < 6 and static_cast<int>(glfwGetTime()) % 2 != 0)
			{
				movLetrero += movOffLetrero * deltaTime;
			}
			else
			{
				aux = 0;
			}
		}*/
	
		//Dado
		std::srand(std::time(0));
		static int numeroGuardado = generarNumeroAleatorio();

		if (mainWindow.getReinicio() == 1.0f) 
		{
			numeroGuardado = generarNumeroAleatorio();
			std::cout << "Nuevo número: " << numeroGuardado << std::endl;
			movDado = 15.0f;
			rotDadoY = 0.0f;
			rotDadoZ = 0.0f;
		}

		if (glfwGetTime() > 5)
		{
			if (numeroGuardado <= 5)
			{
				if (movDado > 5.0f)
				{
					movDado -= movOffDado * deltaTime;
				}

				if (rotDadoZ <= 52)
				{
					{
						rotDadoZ += rotOffDadoZ * deltaTime;
					}
				}

				//Cara 1
				if (numeroGuardado == 1)
				{
					if (rotDadoY <= 36)
					{
						rotDadoY += rotOffDadoY * deltaTime;
					}
				}

				//Cara 2
				if (numeroGuardado == 2)
				{
					if (rotDadoY <= 108)
					{
						rotDadoY += rotOffDadoY * deltaTime;
					}
				}

				//Cara 3
				if (numeroGuardado == 3)
				{
					if (rotDadoY <= 180)
					{
						rotDadoY += rotOffDadoY * deltaTime;
					}
				}

				//Cara 4
				if (numeroGuardado == 4)
				{
					if (rotDadoY <= 252)
					{
						rotDadoY += rotOffDadoY * deltaTime;
					}
				}

				//Cara 5
				if (numeroGuardado == 5)
				{
					if (rotDadoY <= 324)
					{
						rotDadoY += rotOffDadoY * deltaTime;
					}
				}
			}

			if (numeroGuardado >= 6)
			{
				if (movDado > 5.0f)
				{
					movDado -= movOffDado * deltaTime;
				}

				if (rotDadoZ <= 128)
				{
					{
						rotDadoZ += rotOffDadoZ * deltaTime;
					}
				}

				//Cara 6
				if (numeroGuardado == 6)
				{
					if (rotDadoY <= 0)
					{
						rotDadoY += rotOffDadoY * deltaTime;
					}
				}

				//Cara 7
				if (numeroGuardado == 7)
				{
					if (rotDadoY <= 72)
					{
						rotDadoY += rotOffDadoY * deltaTime;
					}
				}

				//Cara 8
				if (numeroGuardado == 8)
				{
					if (rotDadoY <= 144)
					{
						rotDadoY += rotOffDadoY * deltaTime;
					}
				}

				//Cara 9
				if (numeroGuardado == 9)
				{
					if (rotDadoY <= 216)
					{
						rotDadoY += rotOffDadoY * deltaTime;
					}
				}

				//Cara 10
				if (numeroGuardado == 10)
				{
					if (rotDadoY <= 288)
					{
						rotDadoY += rotOffDadoY * deltaTime;
					}
				}
			}
		}

		//Avion
		if (glfwGetTime() > 5)
		{
			if (aux == 0) 
			{
				if (movAvionX >= 7)
				{
					movAvionX -= movOffAvionX * deltaTime;
				}
				else
				{
					if (rotAvion >= -90)
					{
						rotAvion -= rotOffAvion * deltaTime;
					}
					else
					{
						aux = 1;
					}
				}
			}
			
			else if (aux == 1)
			{
				if (movAvionY >= 22)
				{
					movAvionY -= movOffAvionY * deltaTime;
				}
				else
				{
					if (rotAvion >= -180)
					{
						rotAvion -= rotOffAvion * deltaTime;
					}
					else
					{
						aux = 2;
					}
				}

			}

			else if (aux == 2)
			{
				if (movAvionX <= 20)
				{
					movAvionX += movOffAvionX * deltaTime;
				}
				else
				{
					if (rotAvion >= -270)
					{
						rotAvion -= rotOffAvion * deltaTime;
					}
					else
					{
						aux = 3;
					}
				}
			}

			else if (aux == 3)
			{
				if (movAvionY <= 36)
				{
					movAvionY += movOffAvionY * deltaTime;
				}
				else
				{
					if (rotAvion >= -360)
					{
						rotAvion -= rotOffAvion * deltaTime;
					}
					else
					{
						rotAvion = 0;
						aux = 0;
					}
				}
			}
		}


		//if (mainWindow.getReinicio() == 1.0) {
		//	numeroGuardado = generarNumeroAleatorio();
		//	std::cout << "Nuevo número: " << numeroGuardado << std::endl;
		//}
		//else {
		//	std::cout << "Número guardado sin cambios: " << numeroGuardado << std::endl;
		//}
		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		
		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);



		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//Tablero
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.0f, 1.15f, 30.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Tablero.RenderModel();
		model = modelaux;

		//Avion
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movAvionX, 5.0f, movAvionY));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, rotAvion * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Avion.RenderModel();
		model = modelaux;

		//Dado
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.0f, movDado, 10.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		//model = glm::rotate(model, numeroGuardado * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::rotate(model, 128 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::rotate(model, 52 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::rotate(model, 144 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotDadoZ * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, rotDadoY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Dado.RenderModel();
		model = modelaux;

		//Marco
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.15f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Marco.RenderModel();
		model = modelaux;

		//Letrero
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, movLetrero, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotLetrero * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, rotLetrero * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Letrero.RenderModel();
		model = modelaux;

		//Instancia del coche 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movCoche, 0.5f, -3.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kitt_M.RenderModel();

		//Llanta delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, -0.5f, 8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);//llanta con color gris
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(15.5f, -0.5f, 8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta delantera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, -0.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(15.5f, -0.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();
	

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 5.0f, 6.0));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();

		//Agave ¿qué sucede si lo renderizan antes del coche y el helicóptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
