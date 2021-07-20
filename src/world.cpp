/*/

#include "world.h"
#include "game.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"
#include "animation.h"
#include "input.h"
#include "extra/coldet/coldet.h"
#include "stage.h"

#include <iostream>
#include <fstream>
#include <cmath>

World* World::instance = NULL;

//some globals
Shader* shader;
Shader* anim_shader;
Shader* terrain_shader;

Sky* sky;
FloorEntity* world_floor;

std::vector<Entity*> objects;
std::vector<Character*> characters;


World::World()
{
	instance = this;

	// example of shader loading using the shaders manager
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	anim_shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");
	terrain_shader = Shader::Get("data/shaders/terrain.vs", "data/shaders/terrain.fs");

	sky = new Sky();
	world_floor = new FloorEntity();

	readPropFile("data/objProperties.txt", &objects, &characters);

	Game::instance->player = (MainCharacter*)characters[0];
}

//what to do when the image has to be draw
void World::render(void)
{
	
	if (shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", Game::instance->camera->viewprojection_matrix);
		shader->setUniform("u_time", Game::instance->time);

		sky->render(shader);

		glEnable(GL_CULL_FACE);

		for (Entity* e : objects)
			e->render(shader);
		glDisable(GL_CULL_FACE);
		
		//disable shader
		shader->disable();
	}
	
	if(terrain_shader)
	{
		// Render floor
		terrain_shader->enable();
		terrain_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		terrain_shader->setUniform("u_viewprojection", Game::instance->camera->viewprojection_matrix);
		terrain_shader->setUniform("u_time", Game::instance->time);
		world_floor->render(terrain_shader);
		terrain_shader->disable();
	}
	
	if (anim_shader)
	{

		//enable shader
		anim_shader->enable();

		//upload uniforms
		anim_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		anim_shader->setUniform("u_viewprojection", Game::instance->camera->viewprojection_matrix);
		anim_shader->setUniform("u_time", Game::instance->time);

		for (Character* c : characters) {
			c->render(anim_shader, Game::instance->time);
		}

		//disable shader
		anim_shader->disable();
		glDisable(GL_CULL_FACE);
	}
	
}

void World::update(double seconds_elapsed)
{
	
	((MainCharacter*)characters[0])->update(seconds_elapsed, &objects);

}

void World::setEntities()
{
}

void World::readPropFile(const char* filename, std::vector<Entity*>* objects, std::vector<Character*>* characters)
{
	std::ifstream file(filename);
	std::string str;

	while (std::getline(file, str))
	{
		std::vector <std::string> array;
		std::stringstream ss(str);
		std::string temp_string;
		while (std::getline(ss, temp_string, ','))
			array.push_back(temp_string);
		char type = *array[0].c_str();
		
		if (type == 'H') {
			Entity* temp_ent = NULL;
			temp_ent = new House();
			temp_ent->setTranslation(std::stof(array[1]), std::stof(array[2]), std::stof(array[3]));
			temp_ent->scale(0.5, 0.5, 0.5);
			if (atof(array[4].c_str()) != 0)
				temp_ent->rotate(atof(array[4].c_str()), Vector3(std::stof(array[5]), std::stof(array[6]), std::stof(array[7])));
			objects->push_back(temp_ent);
		}
		
		if (type == 'M') {
			Entity* temp_ent = NULL;
			temp_ent = new McDonalds();
			temp_ent->setTranslation(std::stof(array[1]), std::stof(array[2]), std::stof(array[3]));
			if (atof(array[4].c_str()) != 0)
				temp_ent->rotate(atof(array[4].c_str()), Vector3(std::stof(array[5]), std::stof(array[6]), std::stof(array[7])));
			objects->push_back(temp_ent);
		}
		else if (type == 'C') {
			Character* temp_char;
			temp_char = new Character();
			temp_char->setTranslation(std::stof(array[1]), std::stof(array[2]), std::stof(array[3]));
			if (atof(array[4].c_str()) != 0)
				temp_char->rotate(atof(array[4].c_str()), Vector3(std::stof(array[5]), std::stof(array[6]), std::stof(array[7])));
			characters->push_back(temp_char);
		}
	}
}

*/