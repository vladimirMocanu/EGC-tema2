#include "Laborator7.h"

#include <vector>
#include <string>
#include <iostream>
#include <math.h>
#include <glm/gtc/quaternion.hpp>

#include <Core/Engine.h>

using namespace std;

Tema2::Tema2()
{
}

Tema2::~Tema2()
{
}

void Tema2::Init()
{
	//initializam variabilele
	angle = RADIANS(30);
	GetSceneCamera()->SetRotation(glm::quat(1, RADIANS(-8), 0, 0));
	GetSceneCamera()->SetPosition(glm::vec3(0, 1.6, 2.5));

	//crearea de obiecte

	//avion
	{
		Mesh* mesh = new Mesh("avion");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "avion1.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	//elice avion
	{
		Mesh* mesh = new Mesh("elice");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "elice2.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	//marea
	{
		Mesh* mesh = new Mesh("marea");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "mare2.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	//combustibil
	{
		Mesh* mesh = new Mesh("combustibil");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "combustibil.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	//sfere (obstacole + viata)
	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	//nori
	{
		Mesh* mesh = new Mesh("nori");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "nori3.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	//baraCombustibil
	{
		Mesh* mesh = new Mesh("baraCombustibil");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "baraCombustibil2.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}


	// Create a shader program for drawing face polygon with the color of the normal
	{
		Shader* shader = new Shader("ShaderLab7");
		shader->AddShader("Source/Laboratoare/Laborator7/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Laborator7/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	//shader pentru cilindru/mare
	{
		Shader* shader = new Shader("ShaderLab7bis");
		shader->AddShader("Source/Laboratoare/Laborator7/Shaders/VertexShader1.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Laborator7/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	//Light & material properties
	{
		lightPosition = glm::vec3(0, 2, 0);
		lightDirection = glm::vec3(0, -1, 0);
		materialShininess = 30;
		materialKd = 0.5;
		materialKs = 0.5;
		isSpot = 1;
	}

	rotatieMarea = 0;
	translateY = 0;
	globalY = 0;

	score = 0;
	pauseScore = 0;
	pause = false;
	level = 0;
	vitezaGlobal = 1;
	combustibil1 = 0.18;
	camera = true;
	rotireAvion = 0;

	for (int i = 0; i < 3; i++)
	{
		obstacol.push_back(obiecte());
		combustibil.push_back(obiecte());
		nori.push_back(obiecte());

		obstacol[i].viteza = 1 + rand() / (RAND_MAX / 16);
		combustibil[i].viteza = 1 + rand() / (RAND_MAX / 16);
		nori[i].viteza = 1 + rand() / (RAND_MAX / 16);
		cout << nori[i].viteza << " " << obstacol[i].viteza << " " << combustibil[i].viteza << "\n";
	}

}

void Tema2::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(1.000, 0.871, 0.678, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);


}

void Tema2::Update(float deltaTimeSeconds)
{
	//folosim vector de strucutura pentru obstacole, combustibil si nori\
	//acolo retinem viteza, rotatia si un numar random
	//daca este pauza, se opreste totul

	//viteza obiecte
	float speed = 0.5;
	glm::vec3 up = glm::vec3(0, 1, 0);

	if (pause == false) {
		rotatieMarea -= (15 + vitezaGlobal) * deltaTimeSeconds;

		for (int i = 0; i < 3; i++)
		{
			combustibil[i].rotatie += (combustibil[i].viteza + vitezaGlobal) * deltaTimeSeconds;
			obstacol[i].rotatie += (obstacol[i].viteza + vitezaGlobal) * deltaTimeSeconds;
			nori[i].rotatie += (nori[i].viteza + vitezaGlobal) * deltaTimeSeconds;
		}
	}


	//rotatie random obiecte
	for (int i = 0; i < obstacol.size(); i++)
	{
		if (int(obstacol[i].rotatie) % 360 == 0)
		{
			obstacol[i].rand = pow(-1, rand() % 10) * (rand() % 5) / 10;
		}

		if (int(combustibil[i].rotatie) % 360 == 0)
		{
			combustibil[i].rand = pow(-1, rand() % 10) * (rand() % 5) / 10;
		}

		if (int(nori[i].rotatie) % 360 == 0)
		{
			nori[i].rand = pow(-1, rand() % 10) * (rand() % 5) / 10;
		}
	}

	//miscare avion
	if (translateY <= globalY && pause == false)
	{
		translateY += 15 * deltaTimeSeconds;
		lightPosition += up * deltaTimeSeconds * speed;
		rotireAvion += 0.2;
	}
	else if (translateY > globalY&& pause == false)
	{
		translateY -= 15 * deltaTimeSeconds;
		lightPosition -= up * deltaTimeSeconds * speed;
		rotireAvion -= 0.2;
	}

	//desenare obiecte

	//marea
	{
		glm::mat4 marea = glm::mat4(1);
		marea = glm::translate(marea, glm::vec3(0, -0.7, 0));
		marea = glm::rotate(marea, RADIANS(90.0f), glm::vec3(0, 1, 0));
		marea = glm::rotate(marea, RADIANS(rotatieMarea), glm::vec3(1, 0, 0));
		RenderSimpleMesh(meshes["marea"], shaders["ShaderLab7bis"], marea, glm::vec3(0, 0.749, 1));
	}

	//avionul
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1.3, 0));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(rotireAvion), glm::vec3(0, 0, 1));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, translateY, 0));
		RenderSimpleMesh(meshes["avion"], shaders["ShaderLab7"], modelMatrix, glm::vec3(1.0, 0, 0));
	}

	//elicea
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.05, 1.3, 0));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(rotireAvion), glm::vec3(0, 0, 1));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, translateY, 0));
		RenderSimpleMesh(meshes["elice"], shaders["ShaderLab7"], modelMatrix, glm::vec3(0.5, 0.6, 0));
	}

	//am folosit un for pentru a nu scrie acelasi cod de mai multe ori
	//combustibil

	for (int i = 0; i < combustibil.size(); i++)
	{
		int a = 0;
		if (i % 2 == 0)
			a = -1;


		glm::mat4 combustibil1 = glm::mat4(1);
		combustibil1 = glm::translate(combustibil1, glm::vec3(0, -0.7, 0));
		combustibil1 = glm::rotate(combustibil1, RADIANS(combustibil[i].rotatie), glm::vec3(a, 0, 1));
		combustibil1 = glm::translate(combustibil1, glm::vec3(0, -2.5 + combustibil[i].rand, 0));
		combustibil1 = glm::rotate(combustibil1, RADIANS(combustibil[i].rotatie), glm::vec3(2, 2, 1));
		combustibil1 = glm::scale(combustibil1, glm::vec3(0.02f));
		RenderSimpleMesh(meshes["combustibil"], shaders["ShaderLab7"], combustibil1, glm::vec3(0.251, 0.878, 0.816));
	}

	//obstacol
	for (int i = 0; i < obstacol.size(); i++)
	{
		int a = 0;
		if (i % 2 == 0)
			a = -1;

		glm::mat4 obstacol1 = glm::mat4(1);
		obstacol1 = glm::translate(obstacol1, glm::vec3(0, -0.7, 0));
		obstacol1 = glm::rotate(obstacol1, RADIANS(obstacol[i].rotatie), glm::vec3(a, 0, 1));
		obstacol1 = glm::translate(obstacol1, glm::vec3(0, -2.5 + obstacol[i].rand, 0));
		obstacol1 = glm::rotate(obstacol1, RADIANS(obstacol[i].rotatie), glm::vec3(1, 1, 1));
		obstacol1 = glm::scale(obstacol1, glm::vec3(0.09f));

		RenderSimpleMesh(meshes["sphere"], shaders["ShaderLab7"], obstacol1, glm::vec3(1.0, 0, 0));
	}


	//nori

	for (int i = 0; i < nori.size(); i++)
	{

		glm::mat4 nori1 = glm::mat4(1);
		nori1 = glm::translate(nori1, glm::vec3(0, -0.7, 0));
		nori1 = glm::rotate(nori1, RADIANS(nori[i].rotatie), glm::vec3(0, 0, 1));
		nori1 = glm::translate(nori1, glm::vec3(0, -3 + nori[i].rand, -1));
		nori1 = glm::rotate(nori1, RADIANS(nori[i].rotatie), glm::vec3(1, 0, 0));
		nori1 = glm::scale(nori1, glm::vec3(0.05f));

		RenderSimpleMesh(meshes["nori"], shaders["ShaderLab7"], nori1, glm::vec3(0.878, 1, 1));
	}

	//vieti
	for (int i = 0; i < 3; i++)
	{
		glm::mat4 vieti = glm::mat4(1);
		vieti = glm::translate(vieti, glm::vec3(-2.2 + i * 0.2, 2.1, 0));
		vieti = glm::rotate(vieti, RADIANS(rotatieMarea), glm::vec3(1, 0, 0));
		vieti = glm::scale(vieti, glm::vec3(0.09f));

		RenderSimpleMesh(meshes["sphere"], shaders["ShaderLab7"], vieti, glm::vec3(1.0, 0, 0));
	}

	//bara combustibil
	{
		glm::mat4 obstacol1 = glm::mat4(1);
		obstacol1 = glm::translate(obstacol1, glm::vec3(2, 2.1, -0.01));
		obstacol1 = glm::scale(obstacol1, glm::vec3(0.2f));

		RenderSimpleMesh(meshes["baraCombustibil"], shaders["ShaderLab7"], obstacol1, glm::vec3(0, 0, 0));
	}

	{
		glm::mat4 obstacol1 = glm::mat4(1);
		obstacol1 = glm::translate(obstacol1, glm::vec3(2, 2.1, 0));
		obstacol1 = glm::scale(obstacol1, glm::vec3(combustibil1));

		RenderSimpleMesh(meshes["baraCombustibil"], shaders["ShaderLab7"], obstacol1, glm::vec3(1, 1, 1));
	}
	//score
	Score();

	//level
	//nivelul se schimba la 20 de sec
	if (score > 20 * level)
	{
		level += 1;
		vitezaGlobal += 10;
		cout << "\n\nLEVEL : " << level << "\n\n";
	}

	//mutare camera
	if (camera == false)
	{
		GetSceneCamera()->SetPosition(glm::vec3(0, 0.11 * translateY, 0));
		GetSceneCamera()->SetRotation(glm::quat(1, 0, RADIANS(-40), 0));
	}

	for (int i = 0; i < 3; i++)
	{
		if (-2.5f + obstacol[i].rand == translateY)
		{

		}
		cout << -2.5f + obstacol[i].rand << "\n";
		cout << translateY << "\n";
	}

}


void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Set shader uniforms for light & material properties
	//  Set light position uniform
	int light_position = glGetUniformLocation(shader->program, "light_position");
	glUniform3f(light_position, lightPosition.x, lightPosition.y, lightPosition.z);

	int light_direction = glGetUniformLocation(shader->program, "light_direction");
	glUniform3f(light_direction, lightDirection.x, lightDirection.y, lightDirection.z);

	// spotlight
	int isSpot_location = glGetUniformLocation(shader->program, "isSpot");
	glUniform1i(isSpot_location, isSpot);

	// angle
	int angle_location = glGetUniformLocation(shader->program, "angle");
	glUniform1f(angle_location, angle);

	// Set eye position (camera position) uniform
	glm::vec3 eyePosition = GetSceneCamera()->transform->GetWorldPosition();
	int eye_position = glGetUniformLocation(shader->program, "eye_position");
	glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

	// Set material property uniforms (shininess, kd, ks, object color) 
	int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
	glUniform1i(material_shininess, materialShininess);

	int material_kd = glGetUniformLocation(shader->program, "material_kd");
	glUniform1f(material_kd, materialKd);

	int material_ks = glGetUniformLocation(shader->program, "material_ks");
	glUniform1f(material_ks, materialKs);

	int object_color = glGetUniformLocation(shader->program, "object_color");
	glUniform3f(object_color, color.r, color.g, color.b);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);

	// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
	// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h
}

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
	float speed = 2;
	glm::vec3 right = GetSceneCamera()->transform->GetLocalOXVector();

	//pentru miuscarea luminii
	if (window->KeyHold(GLFW_KEY_RIGHT)) lightDirection += right * deltaTime * speed;
	if (window->KeyHold(GLFW_KEY_LEFT)) lightDirection -= right * deltaTime * speed;

	lightDirection /= sqrt(lightDirection.x * lightDirection.x + lightDirection.y * lightDirection.y + lightDirection.z * lightDirection.z);
}

/*
- calculare scor + afisare
- verificare terminare joc
- scorul creste in fiecare secunda
*/
void  Tema2::Score()
{
	int lastTime = glfwGetTime();

	if (score < lastTime - pauseScore)
	{
		//in caz de este pauza, creste scorul
		if (pause == true)
		{
			pauseScore = lastTime - score;

		}
		else {
			score = lastTime - pauseScore;
			combustibil1 -= 0.005;

			//afisare scor din 2 in 2
			if (score % 2 == 0)
			{
				cout << "Score:" << score << "\n";
			}
		}
	}

	//terminare joc cand iesim din ecran, cand atingem marea 
	//sau ramanem fara combustibil
	if (translateY < -7  || translateY > 25 || combustibil1 <= 0)
	{
		cout << "\n\nGAME OVER!\n";
		cout << "FINAL SCORE: " << score << "\n";
		Exit();
	}


}

/*
- daca apasam pe tasta S, creste viteza
- daca apasam pe F, schimbam tipul lumina
- daca apasam pe P, punem pauza
- daca apasam pe G, "se stinge lumina"
- daca apasam pe C, se schimba camera
*/
void Tema2::OnKeyPress(int key, int mods)
{
	//shimbare viteza
	if ((key == GLFW_KEY_S))
	{
		if (vitezaGlobal < 30 * level) {
			vitezaGlobal += 20;
		}
		else {
			vitezaGlobal = 10 * level;
		}
	}
	//pauza
	if ((key == GLFW_KEY_P))
	{

		if (pause == true)
		{
			pause = false;
		}
		else if (pause == false)
		{
			pause = true;
		}
	}
	//schimbare lumina
	if (window->KeyHold(GLFW_KEY_F)) {
		isSpot = (isSpot + 1) % 2;
	}

	if (window->KeyHold(GLFW_KEY_G)) {
		if (isSpot != -1) {
			isSpot = -1;
		}
		else {
			isSpot = (isSpot + 1) % 2;
		}

	}

	//schimbare thirdPerson sau firstPerson
	if ((key == GLFW_KEY_C))
	{
		if (camera == true)
			camera = false;
		else
		{
			GetSceneCamera()->SetRotation(glm::quat(1, RADIANS(-8), 0, 0));
			GetSceneCamera()->SetPosition(glm::vec3(0,1.6,2.5));
			camera = true;
		}


	}
}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	//pentru miscarea avionului
	globalY -= 0.2 * deltaY;

}
