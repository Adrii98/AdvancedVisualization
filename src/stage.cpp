#include "stage.h"
#include "game.h"
#include "input.h"
#include "entity.h"
#include "order.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <time.h>
#include <stdlib.h> 



bool Stage::hasClicked(std::vector<Vector2> points)
{
	Vector2 p0 = points[0];
	Vector2 p1 = points[1];
	Vector2 p2 = points[2];
	Vector2 p3 = points[3];

	Vector2 mouse_pos = Input::mouse_position;
	float width = Game::instance->window_width;
	float height = Game::instance->window_height;

	mouse_pos.y = height - mouse_pos.y;

	// Get mouse position between -1 and 1
	mouse_pos.x = ((mouse_pos.x * 2) / width) - 1;
	mouse_pos.y = ((mouse_pos.y * 2) / height) - 1;

	return(mouse_pos.x >= p0.x && mouse_pos.x <= p1.x && mouse_pos.y >= p0.y && mouse_pos.y <= p2.y);
}

InitialStage::InitialStage()
{
	quad_shader = Shader::Get("data/shaders/quad.vs", "data/shaders/texture.fs");
	
	menu_pos = MenuPos::START;

	background_points.push_back(Vector2(-1, -1)); background_points.push_back(Vector2(1, -1)); background_points.push_back(Vector2(-1, 1)); background_points.push_back(Vector2(1, 1));
	title_points.push_back(Vector2(-0.4, 0.5)); title_points.push_back(Vector2(0.4, 0.5)); title_points.push_back(Vector2(-0.4, 0.8)); title_points.push_back(Vector2(0.4, 0.8));
	start_button_points.push_back(Vector2(-0.3, 0.05)); start_button_points.push_back(Vector2(0.3, 0.05)); start_button_points.push_back(Vector2(-0.3, 0.3)); start_button_points.push_back(Vector2(0.3, 0.3));
	difficulty_button_points.push_back(Vector2(-0.3, -0.3)); difficulty_button_points.push_back(Vector2(0.3, -0.3)); difficulty_button_points.push_back(Vector2(-0.3, -0.05)); difficulty_button_points.push_back(Vector2(0.3, -0.05));
	exit_button_points.push_back(Vector2(-0.3, -0.65)); exit_button_points.push_back(Vector2(0.3, -0.65)); exit_button_points.push_back(Vector2(-0.3, -0.4)); exit_button_points.push_back(Vector2(0.3, -0.4));

	background = new QuadImage(background_points, "data/background.tga");
	title = new QuadImage(title_points, "data/titulo.tga");
	start_button = new ButtonImage(start_button_points, "data/icons/start_button.tga", "data/icons/s_start_button.tga");
	easy_button = new ButtonImage(difficulty_button_points, "data/icons/easy_button.tga", "data/icons/s_easy_button.tga");
	medium_button = new ButtonImage(difficulty_button_points, "data/icons/medium_button.tga", "data/icons/s_medium_button.tga");
	hard_button = new ButtonImage(difficulty_button_points, "data/icons/hard_button.tga", "data/icons/s_hard_button.tga");
	exit_button = new ButtonImage(exit_button_points, "data/icons/exit_button.tga", "data/icons/s_exit_button.tga");

}

void InitialStage::render(void)
{
	if (quad_shader)
	{
		start_button->unSelect();
		easy_button->unSelect();
		medium_button->unSelect();
		hard_button->unSelect();
		exit_button->unSelect();
		switch (menu_pos)
		{
		case MenuPos::START:
			start_button->select();
			break;
		case MenuPos::DIFFICULTY:
			easy_button->select();
			medium_button->select();
			hard_button->select();
			break;
		case MenuPos::EXIT:
			exit_button->select();
			break;
		}

		//enable shader
		quad_shader->enable();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);

		//upload uniforms
		quad_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		quad_shader->setUniform("u_time", Game::instance->time);
		background->render();
		title->render();
		start_button->render();

		switch (Game::instance->difficulty)
		{
		case Difficulty::EASY:
			easy_button->render();
			break;
		case Difficulty::MEDIUM:
			medium_button->render();
			break;
		case Difficulty::HARD:
			hard_button->render();
			break;
		default:
			easy_button->render();
			break;
		}

		exit_button->render();

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		//disable shader
		quad_shader->disable();
	}
}

void InitialStage::update(double seconds_elapsed)
{

	HSAMPLE hSample = BASS_SampleLoad(false, "data/sounds/menu.wav", 0, 0, 3, 0);
	if (hSample == 0)
	{
		std::cout << "No lee el archivo";
		Game::instance->must_exit = true;
	}
	HCHANNEL hSampleChannel = BASS_SampleGetChannel(hSample, false);
	
	if (Input::wasKeyPressed(SDL_SCANCODE_RETURN))
	{
		if (menu_pos == MenuPos::START)
			Game::instance->nextStage();

		else if (menu_pos == MenuPos::EXIT)
			Game::instance->must_exit = true;
		else if (menu_pos == MenuPos::DIFFICULTY)
		{
			switch (Game::instance->difficulty)
			{
			case Difficulty::EASY:
				Game::instance->difficulty = Difficulty::MEDIUM;
				break;
			case Difficulty::MEDIUM:
				Game::instance->difficulty = Difficulty::HARD;
				break;
			case Difficulty::HARD:
				Game::instance->difficulty = Difficulty::EASY;
				break;
			default:
				Game::instance->difficulty = Difficulty::EASY;
				break;
			}
			BASS_ChannelPlay(hSampleChannel, true); //Sound
		}
	}

	if ((Input::wasKeyPressed(SDL_SCANCODE_A) || Input::wasKeyPressed(SDL_SCANCODE_RIGHT)) && menu_pos == MenuPos::DIFFICULTY)
	{
		switch (Game::instance->difficulty)
		{
		case Difficulty::EASY:
			Game::instance->difficulty = Difficulty::HARD;
			break;
		case Difficulty::MEDIUM:
			Game::instance->difficulty = Difficulty::EASY;
			break;
		case Difficulty::HARD:
			Game::instance->difficulty = Difficulty::MEDIUM;
			break;
		default:
			Game::instance->difficulty = Difficulty::EASY;
			break;
		}
		BASS_ChannelPlay(hSampleChannel, true); //Sound

	}

	if ((Input::wasKeyPressed(SDL_SCANCODE_D) || Input::wasKeyPressed(SDL_SCANCODE_LEFT)) && menu_pos == MenuPos::DIFFICULTY)
	{
		switch (Game::instance->difficulty)
		{
		case Difficulty::EASY:
			Game::instance->difficulty = Difficulty::MEDIUM;
			break;
		case Difficulty::MEDIUM:
			Game::instance->difficulty = Difficulty::HARD;
			break;
		case Difficulty::HARD:
			Game::instance->difficulty = Difficulty::EASY;
			break;
		default:
			Game::instance->difficulty = Difficulty::EASY;
			break;
		}
		BASS_ChannelPlay(hSampleChannel, true); //Sound
	}

	if ((Input::wasKeyPressed(SDL_SCANCODE_W) || Input::wasKeyPressed(SDL_SCANCODE_UP)))
	{
		switch (menu_pos)
		{
		case MenuPos::START:
			menu_pos = MenuPos::EXIT;
			break;
		case MenuPos::DIFFICULTY:
			menu_pos = MenuPos::START;
			break;
		case MenuPos::EXIT:
			menu_pos = MenuPos::DIFFICULTY;
			break;
		}
		BASS_ChannelPlay(hSampleChannel, true); //Sound
	}

	if ((Input::wasKeyPressed(SDL_SCANCODE_S) || Input::wasKeyPressed(SDL_SCANCODE_DOWN)))
	{
		switch (menu_pos)
		{
		case MenuPos::START:
			menu_pos = MenuPos::DIFFICULTY;
			break;
		case MenuPos::DIFFICULTY:
			menu_pos = MenuPos::EXIT;
			break;
		case MenuPos::EXIT:
			menu_pos = MenuPos::START;
			break;
		}
		BASS_ChannelPlay(hSampleChannel, true); //Sound
	}
}

void InitialStage::mouseButtonWasPressed()
{

	if (hasClicked(start_button_points))
	{
		Game::instance->nextStage();
		BASS_ChannelPlay(hSampleChannel, true); //Sound
	}
	else if (hasClicked(exit_button_points))
	{
		Game::instance->must_exit = true;
		BASS_ChannelPlay(hSampleChannel, true); //Sound
	}
	else if (hasClicked(difficulty_button_points))
	{
		switch (Game::instance->difficulty)
		{
		case Difficulty::EASY:
			Game::instance->difficulty = Difficulty::MEDIUM;
			break;
		case Difficulty::MEDIUM:
			Game::instance->difficulty = Difficulty::HARD;
			break;
		case Difficulty::HARD:
			Game::instance->difficulty = Difficulty::EASY;
			break;
		default:
			Game::instance->difficulty = Difficulty::EASY;
			break;
		}
		BASS_ChannelPlay(hSampleChannel, true); //Sound
	}
}

TransitionStage::TransitionStage()
{
	quad_shader = Shader::Get("data/shaders/quad.vs", "data/shaders/texture.fs");
	background_points.push_back(Vector2(-1, -1)); background_points.push_back(Vector2(1, -1)); background_points.push_back(Vector2(-1, 1)); background_points.push_back(Vector2(1, 1));
	background = new QuadImage(background_points, "data/controls.tga");
}

void TransitionStage::render(void)
{
	if (quad_shader)
	{
		//enable shader
		quad_shader->enable();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);

		//upload uniforms
		quad_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		quad_shader->setUniform("u_time", Game::instance->time);
		this->background->render();
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		//disable shader
		quad_shader->disable();
	}

}
void TransitionStage::update(double seconds_elapsed)
{
	if (Input::wasKeyPressed(SDL_SCANCODE_RETURN))
	{
		Game::instance->nextStage();
		HSAMPLE hSample = BASS_SampleLoad(false, "data/sounds/menu.wav", 0, 0, 3, 0);
		if (hSample == 0)
		{
			std::cout << "No lee el archivo";
			Game::instance->must_exit = true;
		}
		HCHANNEL hSampleChannel = BASS_SampleGetChannel(hSample, false);
		BASS_ChannelPlay(hSampleChannel, true); //Sound
	}
}


GameStage::GameStage()
{
	pitch = 20;
	angle = 0;
	zoom = 0;
	show_map = false;
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	anim_shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");
	terrain_shader = Shader::Get("data/shaders/terrain.vs", "data/shaders/terrain.fs");
	quad_shader = Shader::Get("data/shaders/quad.vs", "data/shaders/texture.fs");
	
	sky = new Sky();
	world_floor = new FloorEntity();

	player = new MainCharacter();

	readPropFile("data/objProperties.txt");
	
	map = new QuadImage(Vector2(-0.8, -0.8), Vector2(0.8, -0.8), Vector2(-0.8, 0.8), Vector2(0.8, 0.8), "data/map.tga");
	player_dot = new QuadImage(Vector2(), Vector2(), Vector2(), Vector2(), "data/icons/reddot.tga");
	dest_dot = new QuadImage(Vector2(), Vector2(), Vector2(), Vector2(), "data/icons/reddot.tga");

	curr_order = player->getCurrentOrder();

	time_for_next_order = 0.f;
	time_for_next_bonus = 0.f;

	message_box_points.push_back(Vector2(0.3, 0.5)); message_box_points.push_back(Vector2(1, 0.5)); message_box_points.push_back(Vector2(0.3, 1)); message_box_points.push_back(Vector2(1, 1));

	dollar_positions.push_back(Vector3(330, 2, 280));	dollar_positions.push_back(Vector3(290, 2, 630));	dollar_positions.push_back(Vector3(730, 2, 200));	dollar_positions.push_back(Vector3(-570, 2, 780));
	dollar_positions.push_back(Vector3(-800, 2, 350));	dollar_positions.push_back(Vector3(720, 2, 650));	dollar_positions.push_back(Vector3(-710, 2, -450));	dollar_positions.push_back(Vector3(-630, 2, -750));
	dollar_positions.push_back(Vector3(170, 2, 720));	dollar_positions.push_back(Vector3(-670, 2, -620));	dollar_positions.push_back(Vector3(390, 2, -740));	dollar_positions.push_back(Vector3(800, 2, -335));
	reputation_star = new QuadImage(Vector2(-0.95, -0.9), Vector2(-0.85, -0.9), Vector2(-0.95, -0.8), Vector2(-0.85, -0.8), "data/icons/star.tga");

	//curr_order = (Order*)malloc(sizeof(Order));
	//bonus = (DollarBonus*)malloc(sizeof(DollarBonus));

}

void GameStage::init()
{ 
	time_for_next_order = 10.f; 
	time_for_next_bonus = 10.f * (int)Game::instance->difficulty; 
};

void GameStage::render(void)
{
	if (terrain_shader)
	{
		// Render floor
		terrain_shader->enable();
		terrain_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		terrain_shader->setUniform("u_viewprojection", Game::instance->camera->viewprojection_matrix);
		terrain_shader->setUniform("u_time", Game::instance->time);
		world_floor->render();
		terrain_shader->disable();
	}

	if (shader)
	{
		//enable shader
		shader->enable();
		
		//upload uniforms
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", Game::instance->camera->viewprojection_matrix);
		shader->setUniform("u_time", Game::instance->time);

		sky->render();

		Vector3 camera_pos = Game::instance->camera->eye;
		Vector3 camera_front = Game::instance->camera->center;

		for (Entity* obj : getObjectsInArea(Game::instance->camera->eye, Game::instance->RENDER_DISTANCE))
		{
			Vector3 sphere_center = obj->getModel() * obj->getMesh()->box.center;
			float sphere_radius = obj->getMesh()->radius + 100;

			if (Game::instance->camera->testSphereInFrustum(sphere_center, sphere_radius) == false)
				continue;
			obj->render();
		}

		if (bonus != NULL)
			bonus->render();

		//disable shader
		shader->disable();
	}
	

	if (anim_shader)
	{
		//enable shader
		anim_shader->enable();

		//upload uniforms
		anim_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		anim_shader->setUniform("u_viewprojection", Game::instance->camera->viewprojection_matrix);
		anim_shader->setUniform("u_time", Game::instance->time);

		assert(player != NULL);

		for (std::vector<Neighbour*>::iterator it = neighbours.begin(); it != neighbours.end(); ++it)
			(*it)->render();
		
		player->render();

		//disable shader
		anim_shader->disable();
	}
	
	if (quad_shader)
	{
		//enable shader
		quad_shader->enable();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//upload uniforms
		quad_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		quad_shader->setUniform("u_time", Game::instance->time);
		
		if (show_map)
		{
			Vector2 pos; 
			pos.x = player->getPosition().x;
			pos.y = player->getPosition().z;

			pos *=(0.8/1024);
			
			player_dot->setPosition(Vector2(pos.x - 0.01, pos.y - 0.01), Vector2(pos.x + 0.01, pos.y - 0.01), Vector2(pos.x - 0.01, pos.y + 0.01), Vector2(pos.x + 0.01, pos.y + 0.01));
			player_dot->render();

			if (curr_order != NULL) 
			{
				Vector2 dest;
				if (curr_order->getOrderState() == OrderState::NOT_PICKED_UP)
				{
					dest.x = curr_order->getShop()->getPosition().x;
					dest.y = curr_order->getShop()->getPosition().z;
					if(curr_order->getType() == OrderType::BURGER)
						dest_dot->setImage("data/icons/burger_icon.tga");
					else
						dest_dot->setImage("data/icons/hotdog_icon.tga");
				}
				else if (curr_order->getOrderState() == OrderState::GO_TO_DELIVER)
				{
					dest.x = curr_order->getHouse()->getPosition().x;
					dest.y = curr_order->getHouse()->getPosition().z;
					dest_dot->setImage("data/icons/house_icon.tga");
				}
				dest *= (0.8 / 1024);
				dest_dot->setPosition(Vector2(dest.x - 0.02, dest.y - 0.02), Vector2(dest.x + 0.02, dest.y - 0.02), Vector2(dest.x - 0.02, dest.y + 0.02), Vector2(dest.x + 0.02, dest.y + 0.02));
							
				dest_dot->render();
			}

			map->render();
		}

		for (int i = 0; i < player->getReputation(); i++)
		{
			reputation_star->render();
			Vector2 p0 = Vector2(-0.95, -0.9 + ((i + 1) * 0.1f));
			Vector2 p1 = Vector2(-0.85, -0.9 + ((i + 1) * 0.1f));
			Vector2 p2 = Vector2(-0.95, -0.8 + ((i + 1) * 0.1f));
			Vector2 p3 = Vector2(-0.85, -0.8 + ((i + 1) * 0.1f));
			reputation_star->setPosition(p0, p1, p2, p3);
		}

		if (curr_order != NULL)
			curr_order->render();

		glDisable(GL_BLEND);

		//disable shader
		quad_shader->disable();
	}

}

void GameStage::update(double seconds_elapsed)
{
	
	player->update(seconds_elapsed, &objects);


	if (bonus != NULL)
	{
		Vector3 character_center = player->getPosition() + Vector3(0, 4, 0);
		Vector3 coll_point;
		Vector3 coll_norm;

		if (bonus->getMesh()->testSphereCollision(bonus->getModel(), character_center, 1, coll_point, coll_norm) == true)
		{
			player->addMoney(bonus->getBonus());
			bonus = NULL;
			HSAMPLE hSample = BASS_SampleLoad(false, "data/sounds/money.wav", 0, 0, 3, 0);
			if (hSample == 0)
			{
				std::cout << "No lee el archivo";
				Game::instance->must_exit = true;
			}
			HCHANNEL hSampleChannel = BASS_SampleGetChannel(hSample, false);
			BASS_ChannelPlay(hSampleChannel, true); //Sound
		}
	}

	for (std::vector<Neighbour*>::iterator it = neighbours.begin(); it != neighbours.end(); ++it)
		(*it)->update(seconds_elapsed, &objects);
	
	if (Input::wasKeyPressed(SDL_SCANCODE_P))
	{
		Game::instance->returnStage();
	}

	if (Input::mouse_state & SDL_BUTTON_LEFT)
	{

		float MAX_PITCH = 90.f;
		float MIN_PITCH = 1.f;

		float new_pitch = pitch - Input::mouse_delta.y * 0.1f;

		if (new_pitch < MAX_PITCH && new_pitch > MIN_PITCH)
			pitch = new_pitch;

		angle = fmod(angle + Input::mouse_delta.x * 0.3f, 360);
	}

	lookAtCharacter();

	if (Input::wasKeyPressed(SDL_SCANCODE_M))
	{
		if (!show_map) {
			show_map = true;}
		else 
			show_map = false;
	}

	// Check / Set Bonus
	
	if (bonus == NULL)
	{
		if (time_for_next_bonus > 0)
			time_for_next_bonus = time_for_next_order - (seconds_elapsed);
		else
		{
			srand(time(NULL));

			int rand_pos = rand() % dollar_positions.size();

			bonus = new DollarBonus();
			bonus->setTranslation(dollar_positions[rand_pos].x, dollar_positions[rand_pos].y, dollar_positions[rand_pos].z);
		}
	}else
		bonus->update(seconds_elapsed);
	

	// Check / Set Order
	if (curr_order == NULL)
	{
		if (time_for_next_order > 0)
			time_for_next_order = time_for_next_order - (seconds_elapsed);
		else
		{
			srand(time(NULL));
			//Get random house of objects list ( last 2 objects are shops)
			int rand_house = rand() % objects.size() - 2;

			if ((rand_house % 2) == 0)
				curr_order = new Order((House*)objects[rand_house], mcdonalds);
			else
				curr_order = new Order((House*)objects[rand_house], hotdog_cart);
			player->newOrder(curr_order);
			player->payOrder();
		}
		return;
	}

	if (curr_order->hasFinished())
	{
		if (time_for_next_order > 0)
			time_for_next_order = time_for_next_order - (seconds_elapsed);
		else
		{
			srand(time(NULL));
			//Get random house of objects list ( last 2 objects are shops)
			int rand_house = rand() % objects.size() - 2;

			if((rand_house % 2) == 0)
				curr_order = new Order((House*)objects[rand_house], mcdonalds);
			else
				curr_order = new Order((House*)objects[rand_house], hotdog_cart);
				curr_order = new Order((House*)objects[rand_house], mcdonalds);
			player->newOrder(curr_order);
			player->payOrder();
		}
	}
	else if (player->getOrderState() == OrderState::DELIVERED)
	{
		player->chargeDelivery();
		curr_order->finishOrder();
		srand(time(NULL));
		time_for_next_order = 10.f + rand() % 20 * (int)Game::instance->difficulty * 4/(player->getReputation() + 1.f);
	}
	else if (player->getOrderState() == OrderState::STOLEN || player->getOrderState() == OrderState::TIME_FINISHED)
	{
		curr_order->finishOrder();
		srand(time(NULL));
		time_for_next_order = 10.f + std::rand() % 20 * (int)Game::instance->difficulty;
	}
	else
		curr_order->update(seconds_elapsed);
	if (Input::wasKeyPressed(SDL_SCANCODE_H))
	{
		Game::instance->nextStage(-1);
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_J))
	{
		Game::instance->nextStage(1);
	}
}

void GameStage::mouseButtonWasPressed()
{
	if (curr_order == NULL)
		return;
	if (curr_order->messageIsShowing() && hasClicked(message_box_points))
	{
		curr_order->unshowMessage();
		if (curr_order->hasFinished())
			curr_order = NULL;
	}
		
}

void GameStage::lookAtCharacter()
{
	Vector3 v = player->getPosition();
	Game::instance->camera->eye = calculateCameraPosition();
	Game::instance->camera->center = v;
}

Vector3 GameStage::calculateCameraPosition()
{
	Vector3 pos;
	float distanceFromPlayer = 100.0f;
	float angleAroundPlayer = 100.0f;
	float horitzontalDistance;
	float verticalDistance;
	float theta, offsetX, offsetZ;

	// Calculate Zoom
	distanceFromPlayer -= zoom;

	// Calculate Angle
	angleAroundPlayer -= angle;

	horitzontalDistance = distanceFromPlayer * cos(pitch * DEG2RAD);
	verticalDistance = distanceFromPlayer * sin(pitch * DEG2RAD);

	horitzontalDistance = (horitzontalDistance > 0) ? horitzontalDistance : 0;
	verticalDistance = (verticalDistance > 0) ? verticalDistance : 0;

	theta = angleAroundPlayer;
	offsetX = horitzontalDistance * sin(theta * DEG2RAD);
	offsetZ = horitzontalDistance * cos(theta * DEG2RAD);

	yaw = 180 - (player->getAngle() + angleAroundPlayer);

	pos = player->getPosition() + Vector3(offsetX, verticalDistance, -offsetZ);

	return pos;
}

std::vector<Entity*> GameStage::getObjectsInArea(Vector3 point, float dist)
{
	std::vector<Entity*> near_objects;
	for (Entity* obj : objects)
	{
		if (point.distance(obj->getPosition()) < dist)
			near_objects.push_back(obj);
	}
	return near_objects;
}

void GameStage::readPropFile(const char* filename)
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
			const char *option = &array[0].at(1);
			Entity* temp_ent = NULL;
			temp_ent = new House(option);
			if (atof(array[4].c_str()) != 0)
				temp_ent->rotate(atof(array[4].c_str()), Vector3(std::stof(array[5]), std::stof(array[6]), std::stof(array[7])));
			temp_ent->setTranslation(std::stof(array[1]), std::stof(array[2]), std::stof(array[3]));
			this->addObject(temp_ent);
			temp_ent->scale(12, 12, 12);
		}
		if (type == 'M') {
			mcdonalds = new McDonalds();
			mcdonalds->setTranslation(std::stof(array[1]), std::stof(array[2]), std::stof(array[3]));
			if (atof(array[4].c_str()) != 0)
				mcdonalds->rotate(atof(array[4].c_str()), Vector3(std::stof(array[5]), std::stof(array[6]), std::stof(array[7])));
			mcdonalds->scale(10, 10, 10);
			this->addObject(mcdonalds);
		}
		if (type == 'D') {
			hotdog_cart = new HotDog();
			hotdog_cart->setTranslation(std::stof(array[1]), std::stof(array[2]), std::stof(array[3]));
			if (atof(array[4].c_str()) != 0)
				hotdog_cart->rotate(atof(array[4].c_str()), Vector3(std::stof(array[5]), std::stof(array[6]), std::stof(array[7])));
			hotdog_cart->scale(10, 10, 10);
			hotdog_cart->setSeller();
			this->addObject(hotdog_cart);
		}
		if (type == 'T') {
			Entity* temp_ent = NULL;
			temp_ent = new Tree();
			if (atof(array[4].c_str()) != 0)
				temp_ent->rotate(atof(array[4].c_str()), Vector3(std::stof(array[5]), std::stof(array[6]), std::stof(array[7])));
			temp_ent->setTranslation(std::stof(array[1]), std::stof(array[2]), std::stof(array[3]));
			this->addObject(temp_ent);
			temp_ent->scale(12, 12, 12);
		}
		if (type == 'C')
		{
			Neighbour* temp_neig = new Neighbour();
			temp_neig->setTarget(player);
			temp_neig->setTranslation(std::stof(array[1]), std::stof(array[2]), std::stof(array[3]));
			if (atof(array[4].c_str()) != 0)
				temp_neig->rotate(atof(array[4].c_str()), Vector3(std::stof(array[5]), std::stof(array[6]), std::stof(array[7])));
			neighbours.push_back(temp_neig);
		}
		if (type == 'P')
		{
			player->setTranslation(std::stof(array[1]), std::stof(array[2]), std::stof(array[3]));
			if (atof(array[4].c_str()) != 0)
				player->rotate(atof(array[4].c_str()), Vector3(std::stof(array[5]), std::stof(array[6]), std::stof(array[7])));
		}
	
	}
}

EndStage::EndStage()
{
	quad_shader = Shader::Get("data/shaders/quad.vs", "data/shaders/texture.fs");
	select_end = 0;
	background_points.push_back(Vector2(-1, -1)); background_points.push_back(Vector2(1, -1)); background_points.push_back(Vector2(-1, 1)); background_points.push_back(Vector2(1, 1));
	end1 = new QuadImage(background_points, "data/icons/end1.tga");
	end2 = new QuadImage(background_points, "data/icons/end2.tga");

}

void EndStage::render(void)
{
	if (quad_shader)
	{
		//enable shader
		quad_shader->enable();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);

		//upload uniforms
		quad_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		quad_shader->setUniform("u_time", Game::instance->time);
		if (select_end == 1) {
			this->end2->render();
		}
		else
			this->end1->render();
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		//disable shader
		quad_shader->disable();
	}

}

void EndStage::update(double seconds_elapsed)
{
	if (Input::wasKeyPressed(SDL_SCANCODE_RETURN))
	{
		Game::instance->nextStage();
		HSAMPLE hSample = BASS_SampleLoad(false, "data/sounds/menu.wav", 0, 0, 3, 0);
		if (hSample == 0)
		{
			std::cout << "No lee el archivo";
			Game::instance->must_exit = true;
		}
		HCHANNEL hSampleChannel = BASS_SampleGetChannel(hSample, false);
		BASS_ChannelPlay(hSampleChannel, true); //Sound
	}

}