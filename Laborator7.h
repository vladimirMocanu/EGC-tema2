#pragma once
#include <Component/SimpleScene.h>
#include <Component/Transform/Transform.h>

#include <vector> 
#include "LabCamera1.h"


class Tema2 : public SimpleScene
{
	public:
		Tema2();
		~Tema2();

		void Init() override;

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;

		void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, const glm::vec3 &color = glm::vec3(1));

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;

		void  Tema2::Score();


		glm::vec3 lightPosition;
		glm::vec3 lightDirection;
		int isSpot;
		unsigned int materialShininess;
		float materialKd;
		float materialKs;
		float rotatieMarea;
		float translateY;
		float globalY;
		float vitezaGlobal;
		float angle;
		float rotireAvion ;
		bool camera;
		float combustibil1;

		bool pause;
		int pauseScore;
		int score;
		int level;

		struct obiecte
		{
			float rand;
			float rotatie;
			float viteza;
		};

		std::vector<obiecte> obstacol;
		std::vector<obiecte> combustibil;
		std::vector<obiecte> nori;


};
