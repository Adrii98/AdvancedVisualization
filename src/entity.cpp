#include "entity.h"
#include "game.h"
#include "input.h"
#include "order.h"

#include <stdlib.h>
#include <time.h>

Entity::Entity() 
{
	texture = new Texture();
	mesh = new Mesh();
	model = Matrix44();
	angle = 0;
	collision_model = newCollisionModel3D();;
}

void Entity::setCollisionModel()
{
	collision_model->setTriangleNumber(this->mesh->getNumVertices());

	for (std::vector<Vector3>::const_iterator it = this->mesh->vertices.begin(); it != this->mesh->vertices.end(); ++it) {
		Vector3 v1 = *it;
		it++;
		Vector3 v2 = *it;
		it++;
		Vector3 v3 = *it;
		collision_model->addTriangle(	v1.x, v1.y, v1.z,
										v2.x, v2.y, v2.z,
										v3.x, v3.y, v3.z);
	}
	collision_model->finalize();
}


FloorEntity::FloorEntity() : Entity()
{
	this->mesh->createPlane(floor_size);
	this->texture = Texture::Get("data/town/grass.tga");

	rTexture = new Texture();
	rTexture = Texture::Get("data/town/grassy2.tga");
	gTexture = new Texture(); 
	gTexture = Texture::Get("data/town/grassFlowers.tga");
	bTexture = new Texture(); 
	bTexture = Texture::Get("data/town/path.tga");
	blendMap = new Texture();
	blendMap = Texture::Get("data/town/blendMap.tga");
}

void FloorEntity::render()
{
	Shader::current->setUniform("backgroundTexture",this->texture);
	Shader::current->setUniform("rTexture", this->rTexture);
	Shader::current->setUniform("gTexture", this->gTexture);
	Shader::current->setUniform("bTexture", this->bTexture);
	Shader::current->setUniform("blendMap", this->blendMap);
	Shader::current->setUniform("u_model", this->model);
	mesh->render(GL_TRIANGLES);
}

Sky::Sky() : Entity()
{
	this->scale(1500, 1500, 1500);
	this->setMesh(Mesh::Get("data/sphere.obj"));
	this->texture = Texture::Get("data/cielo/cielo.TGA");
}
void Sky::render()
{
	Shader::current->setUniform("u_texture", texture);
	Shader::current->setUniform("u_model", model);
	mesh->render(GL_TRIANGLES);
}

Character::Character():Entity()
{
	mesh = Mesh::Get("data/characters/characters/male.mesh");
	texture = Texture::Get("data/characters/characters/male.png");
	animations.push_back(Animation::Get("data/characters/characters/idle.skanim"));
	animations.push_back(Animation::Get("data/characters/characters/walking.skanim"));
	animations.push_back(Animation::Get("data/characters/characters/running.skanim"));
	anim_state = AnimState::IDLE;
	prev_model = Matrix44();
	isInAir = false;
}

void Character::render()
{
	Animation* a = animations[(int) anim_state];
	a->assignTime(Game::instance->time);
	Shader::current->setUniform("u_texture", texture);
	Shader::current->setUniform("u_model", model);
	mesh->renderAnimated(GL_TRIANGLES, &a->skeleton);
}

void Character::update(double seconds_elapsed, std::vector<Entity*> *objects)
{

	// Check collisions
	Vector3 character_center = this->getPosition() + Vector3(0, 4, 0);
	Vector3 coll_point;
	Vector3 coll_norm;

	for (Entity* e : *objects)
	{
		if (e->getMesh()->testSphereCollision(e->getModel(), character_center, 3, coll_point, coll_norm) == true)
			model = prev_model;
	}
}

MainCharacter::MainCharacter() : Character()
{
	money = 50.f;
	order = NULL;
	reputation = 5;
}


enum OrderState MainCharacter::getOrderState()
{
	if(this->order != NULL)
		return this->order->getOrderState();
}


void MainCharacter::setOrderState(OrderState state)
{
	order->setOrderState(state);
	if (state == OrderState::DELIVERED)
	{
		if(reputation < 10)
			reputation++;
	}	
	else if (state == OrderState::STOLEN || state == OrderState::TIME_FINISHED)
	{
		if (reputation > 0)
			reputation--;
	}
	if (reputation == 0)
		Game::instance->nextStage(-1);
}

void MainCharacter::payOrder() 
{
	this->money = this->money - order->getCost(); 
}

void MainCharacter::chargeDelivery() 
{ 
	int MAX_AMOUNT = 150;
	this->money = this->money + order->getReward();
	if (money > money)
		Game::instance->nextStage(1);
}

void MainCharacter::addMoney(float amount)
{ 
	int MAX_AMOUNT = 150;
	this->money = this->money + amount;
	if (money > money)
		Game::instance->nextStage(1);
};

void MainCharacter::render()
{
	Character::render();
	int scale;
	scale = (Game::instance->window_height > 800 && Game::instance->window_width > 1000) ? 5 : 2;
	drawText(Game::instance->window_width * 0.8, Game::instance->window_height * 0.95, "Money: " + std::to_string(money).substr(0,4) + "$", Vector3(1, 1, 1), scale);
}


void MainCharacter::update(double seconds_elapsed, std::vector<Entity*>* objects)
{

	if (Input::isKeyPressed(SDL_SCANCODE_Q))
	{
		std::cout << getPosition().x;
		std::cout << " , ";
		std::cout << getPosition().z;
		std::cout << std::endl;
	}

	if (Input::wasKeyPressed(SDL_SCANCODE_Y))
	{
		reputation --;
		if (reputation == 0) {
			Game::instance->nextStage(-1);
		}
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_U))
	{
		reputation ++ ;
	}

	Matrix44 prev_model = Matrix44();

	float speed = 1.f;

	if (Input::isKeyPressed(SDL_SCANCODE_Z))
	{
		speed = 10.f;
	}

	if (isInAir);
		//this->jump();
	else if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_D))
	{
		if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) {
			this->run();
			speed = 3.f;
		}
		else
			this->walk();
	}
	else
		this->still();

	if (Input::isKeyPressed(SDL_SCANCODE_W))
	{
		this->currentSpeed = -RUN_SPEED * speed;
	}
	else if (Input::isKeyPressed(SDL_SCANCODE_S))
	{
		this->currentSpeed = RUN_SPEED * speed;
	}
	else {
		this->currentSpeed = 0;
	}

	if (Input::isKeyPressed(SDL_SCANCODE_D))
	{
		this->currentTurnSpeed = TURN_SPEED * speed;
		this->currentSpeed = -RUN_SPEED * speed;
	}
	else if (Input::isKeyPressed(SDL_SCANCODE_A))
	{
		this->currentTurnSpeed = -TURN_SPEED * speed;
		this->currentSpeed = -RUN_SPEED * speed;
	}
	else {
		this->currentTurnSpeed = 0;
	}

	this->rotate(currentTurnSpeed * seconds_elapsed, Vector3(0, 1, 0));

	prev_model = this->getModel();

	float distance = currentSpeed * seconds_elapsed;

	float dx = distance * sin(this->angle * DEG2RAD);
	float dz = distance * cos(this->angle * DEG2RAD);

	this->moveGlobal(dx, 0, -dz);

	// Jump
	if (Input::isKeyPressed(SDL_SCANCODE_SPACE))
	{
		if (!isInAir) {
			this->upwardsSpeed = JUMP_POWER;
			isInAir = true;
			//this->jump();
		}
	}

	upwardsSpeed += GRAVITY * seconds_elapsed;
	this->moveGlobal(0, upwardsSpeed * seconds_elapsed, 0);

	if (this->getPosition().y < TERRAIN_HEIGHT)
	{
		this->moveGlobal(0, -upwardsSpeed * seconds_elapsed, 0);
		upwardsSpeed = 0;
		isInAir = false;
	}

	// Check collisions
	Vector3 character_center = this->getPosition() + Vector3(0, 4, 0);
	Vector3 coll_point;
	Vector3 coll_norm;

	for (Entity* e : *objects)
	{
		if (e->getMesh()->testSphereCollision(e->getModel(), character_center, 1, coll_point, coll_norm) == true)
			model = prev_model;
	}

	if(this->getPosition().x > 1024 || this->getPosition().x < -1024)
		model = prev_model;
	if (this->getPosition().z > 1024 || this->getPosition().z < -1024)
		model = prev_model;


	if (order != NULL) {

		//Test if player is close to Shop
		if (order->getOrderState() == OrderState::NOT_PICKED_UP)
		{
			Entity* shop = order->getShop();
			if (shop->getMesh()->testSphereCollision(shop->getModel(), character_center, 5, coll_point, coll_norm) == true)
				this->setOrderState(OrderState::GO_TO_DELIVER);
		}
		else if (this->getOrderState() == OrderState::GO_TO_DELIVER)
		{
			House* house = order->getHouse();
			if (house->getMesh()->testSphereCollision(house->getModel(), character_center, 5, coll_point, coll_norm) == true)
			{
				HSAMPLE hSample = BASS_SampleLoad(false, "data/sounds/success.wav", 0, 0, 3, 0);
				HCHANNEL hSampleChannel = BASS_SampleGetChannel(hSample, false);
				BASS_ChannelPlay(hSampleChannel, true); //Sound
				this->setOrderState(OrderState::DELIVERED);
			}
				

		}

		if (order->getTime() < 0)
			this->setOrderState(OrderState::TIME_FINISHED);		
	}
	
	
}

Neighbour::Neighbour() : Character()
{
	state = State::WALK;
	anim_state = AnimState::IDLE;
	target = NULL;
}

void Neighbour::update(double seconds_elapsed, std::vector<Entity*>* objects)
{

	if (this == NULL)
		return;

	if (state == State::WALK)
	{
		if (canSeeTheTarget(objects) && target->getOrderState() == OrderState::GO_TO_DELIVER)
			state = State::GO_AFTER_PLAYER;
		else
			walk(seconds_elapsed, objects);
	}
	else if (state == State::GO_AFTER_PLAYER)
	{
		if(target->getOrderState() == OrderState::GO_TO_DELIVER)
			goToTarget(seconds_elapsed, objects);
		else
			state = State::WALK;
		
	}
	else if (state == State::CLOSE_TO_PLAYER)
	{
		target->setOrderState(OrderState::STOLEN);
		state = State::WALK;
	}		
}

bool Neighbour::canSeeTheTarget(std::vector<Entity*>* objects)
{
	float MAX_DISTANCE = 500.f;

	float distance = this->getPosition().distance(target->getPosition());
	if (distance > MAX_DISTANCE)
		return false;

	Vector3 character_head = this->getPosition() + Vector3(0, 8, 0);

	Vector3 toTarget = normalize(target->getPosition() - character_head);
	
	if (dot(this->model.frontVector(), toTarget) < 0.4)
		return false;

	Vector3 coll_point;
	Vector3 coll_norm;

	for (Entity* e : *objects)
	{
		if (e->getMesh()->testRayCollision(e->getModel(), this->getPosition(), this->model.frontVector(), coll_point, coll_norm) == true)
			return false;
	}


	return true;
}

void Neighbour::walk(double seconds_elapsed, std::vector<Entity*>* objects)
{
	float speed;

	switch (Game::instance->difficulty)
	{
	case Difficulty::EASY:
		speed = 1.f;
		break;
	case Difficulty::MEDIUM:
		speed = 1.2f;
		break;
	case Difficulty::HARD:
		speed = 1.5f;
		break;
	default:
		speed = 1.f;
		break;
	}

	srand(time(NULL));

	// Next action: 0 & 1 rotate; 2 be still; 3-9 move
	int action = std::rand() % 10;

	if (action > 2)
	{
		this->currentSpeed = -RUN_SPEED * speed;
		anim_state = AnimState::WALKING;
	}
	else
	{
		this->currentSpeed = 0;
		anim_state = AnimState::IDLE;
	}

	if (action == 0) 
	{
		this->currentTurnSpeed = TURN_SPEED * speed;
		this->currentSpeed = -RUN_SPEED * speed;
		anim_state = AnimState::WALKING;
	}
	else if (action == 1)
	{
		this->currentTurnSpeed = -TURN_SPEED * speed;
		this->currentSpeed = -RUN_SPEED * speed;
		anim_state = AnimState::WALKING;
	}
	else
		this->currentTurnSpeed = 0;

	this->rotate(currentTurnSpeed * seconds_elapsed, Vector3(0, 1, 0));

	prev_model = this->getModel();

	float distance = currentSpeed * seconds_elapsed;

	float dx = distance * sin(this->angle * DEG2RAD);
	float dz = distance * cos(this->angle * DEG2RAD);

	this->moveGlobal(dx, 0, -dz);

	// Check collisions
	Vector3 character_center = this->getPosition() + Vector3(0, 4, 0);
	Vector3 coll_point;
	Vector3 coll_norm;

	for (Entity* e : *objects)
	{
		if (e->getMesh()->testSphereCollision(e->getModel(), character_center, 1, coll_point, coll_norm) == true)
		{
			model = prev_model;
			this->currentTurnSpeed = -TURN_SPEED * speed;
			this->rotate(currentTurnSpeed * seconds_elapsed, Vector3(0, 1, 0));
			anim_state = AnimState::WALKING;
		}
	}

	if (this->getPosition().x > 1024 || this->getPosition().x < -1024)
		model = prev_model;
	if (this->getPosition().z > 1024 || this->getPosition().z < -1024)
		model = prev_model;
}

void Neighbour::goToTarget(double seconds_elapsed, std::vector<Entity*>* objects)
{

	if (!canSeeTheTarget(objects))
		state = State::WALK;

	float speed;

	switch (Game::instance->difficulty)
	{
	case Difficulty::EASY:
		speed = 3.f;
		break;
	case Difficulty::MEDIUM:
		speed = 4.5f;
		break;
	case Difficulty::HARD:
		speed = 6.f;
		break;
	default:
		speed = 3.f;
		break;
	}

	Vector3 target_pos = target->getPosition();
	Vector3 curr_pos = this->getPosition();

	Vector3 v = target_pos - curr_pos;

	float vector_angle = atan2(v.z, v.x); // In Rads

	float diff_angle = vector_angle * RAD2DEG - (this->angle) - 90;

	this->rotate(diff_angle, Vector3(0,1,0));

	float MIN_DISTANCE = 5.f;

	float remaining_distance = curr_pos.distance(target_pos);

	if (remaining_distance < MIN_DISTANCE)
	{
		state = State::CLOSE_TO_PLAYER;
		return;
	}
	else
	{
		state = State::GO_AFTER_PLAYER;
		anim_state = AnimState::RUNNING;
	}

	

	float distance = currentSpeed * seconds_elapsed * speed;

	float dx = distance * sin(this->angle * DEG2RAD);
	float dz = distance * cos(this->angle * DEG2RAD);

	this->moveGlobal(dx, 0, -dz);

	
	// Check collisions
	Vector3 character_center = this->getPosition() + Vector3(0, 4, 0);
	Vector3 coll_point;
	Vector3 coll_norm;

	for (Entity* e : *objects)
	{
		if (e->getMesh()->testSphereCollision(e->getModel(), character_center, 1, coll_point, coll_norm) == true)
		{
			model = prev_model;
			this->currentTurnSpeed = -TURN_SPEED * speed;
			this->rotate(currentTurnSpeed * seconds_elapsed, Vector3(0, 1, 0));
			anim_state = AnimState::WALKING;
		}
	}

	if (this->getPosition().x > 1024 || this->getPosition().x < -1024)
		model = prev_model;
	if (this->getPosition().z > 1024 || this->getPosition().z < -1024)
		model = prev_model;
	

}

House::House(const char *type):Entity()
{
	std::string root = "data/town/house_";
	std::string m, t;
	root += type ;
	m = root + ".obj";
	const char* mobj = m.c_str();
	mesh = Mesh::Get(mobj);
	t = root + ".tga";
	const char* text = t.c_str();
	texture->load(text);
}

void House::render()
{
	Shader::current->setUniform("u_texture", texture);
	Shader::current->setUniform("u_model", model);
	mesh->render(GL_TRIANGLES);
}

McDonalds::McDonalds() : Entity()
{
	mesh2 = new Mesh();
	texture2 = new Texture();
	mesh = Mesh::Get("data/town/mcdonalds_building.OBJ");
	mesh2 = Mesh::Get("data/town/mcdonalds_building2.OBJ");
	texture = Texture::Get("data/town/mcdonalds_texture.tga");
	texture2 = Texture::Get("data/town/mcdonalds_texture2.tga");
}

void McDonalds::render()
{
	Shader::current->setUniform("u_model", model);
	Shader::current->setUniform("u_texture", texture);
	mesh->render(GL_TRIANGLES);
	Shader::current->setUniform("u_texture", texture2);
	mesh2->render(GL_TRIANGLES);
}

HotDog::HotDog() : Entity()
{
	mesh = Mesh::Get("data/town/hotdog.OBJ");
	texture = Texture::Get("data/town/hotdog_texture.tga");
}

void HotDog::render()
{
	Shader::current->setUniform("u_model", model);
	Shader::current->setUniform("u_texture", texture);
	mesh->render(GL_TRIANGLES);

	//Render Seller
	Shader::current->setUniform("u_model", seller.getModel());
	Shader::current->setUniform("u_texture", seller.getTexture());
	seller.getMesh()->render(GL_TRIANGLES);
}

void HotDog::setSeller()
{
	seller.rotate(90, Vector3(0,1,0));
	seller.moveGlobal(this->getPosition().x + 15, this->getPosition().y, this->getPosition().z - 5);
}

Tree::Tree() : Entity()
{
	mesh2 = new Mesh();
	texture2 = new Texture();
	mesh = Mesh::Get("data/trees/trunk.obj");
	mesh2 = Mesh::Get("data/trees/leaves.OBJ");
	texture = Texture::Get("data/trees/trunk.tga");
	texture2 = Texture::Get("data/trees/leaves_olive.tga");
}

void Tree::render()
{
	//for (int i = -1300; i < 1000; i + 10) {
		//this->setScale(12, 12, 12);
		//this->setTranslation(i, 0, -1000);
		Shader::current->setUniform("u_model", model);
		Shader::current->setUniform("u_texture", texture);
		mesh->render(GL_TRIANGLES);
		Shader::current->setUniform("u_texture", texture2);
		mesh2->render(GL_TRIANGLES);
	//}
	//Shader::current->setUniform("u_model", model);
	//Shader::current->setUniform("u_texture", texture);
	//mesh->render(GL_TRIANGLES);
	//Shader::current->setUniform("u_texture", texture2);
	//mesh2->render(GL_TRIANGLES);
}

QuadImage::QuadImage(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, const char* filename):Entity()
{
	//tres vertices del primer triangulo
	mesh->vertices.push_back(Vector3(p2.x, p2.y, 0));
	mesh->uvs.push_back(Vector2(0, 1));
	mesh->vertices.push_back(Vector3(p0.x, p0.y, 0));
	mesh->uvs.push_back(Vector2(0, 0));
	mesh->vertices.push_back(Vector3(p1.x, p1.y, 0));
	mesh->uvs.push_back(Vector2(1, 0));

	//tres vértices del segundo triángulo
	mesh->vertices.push_back(Vector3(p2.x, p2.y, 0));
	mesh->uvs.push_back(Vector2(0, 1));
	mesh->vertices.push_back(Vector3(p1.x, p1.y, 0));
	mesh->uvs.push_back(Vector2(1, 0));
	mesh->vertices.push_back(Vector3(p3.x, p3.y, 0));
	mesh->uvs.push_back(Vector2(1, 1));

	texture->load(filename);
}

QuadImage::QuadImage(std::vector <Vector2> points, const char* filename) :Entity()
{

	Vector2 p0 = points[0];
	Vector2 p1 = points[1];
	Vector2 p2 = points[2];
	Vector2 p3 = points[3];

	//tres vertices del primer triangulo
	mesh->vertices.push_back(Vector3(p2.x, p2.y, 0));
	mesh->uvs.push_back(Vector2(0, 1));
	mesh->vertices.push_back(Vector3(p0.x, p0.y, 0));
	mesh->uvs.push_back(Vector2(0, 0));
	mesh->vertices.push_back(Vector3(p1.x, p1.y, 0));
	mesh->uvs.push_back(Vector2(1, 0));

	//tres vértices del segundo triángulo
	mesh->vertices.push_back(Vector3(p2.x, p2.y, 0));
	mesh->uvs.push_back(Vector2(0, 1));
	mesh->vertices.push_back(Vector3(p1.x, p1.y, 0));
	mesh->uvs.push_back(Vector2(1, 0));
	mesh->vertices.push_back(Vector3(p3.x, p3.y, 0));
	mesh->uvs.push_back(Vector2(1, 1));

	texture->load(filename);
}


void QuadImage::render()
{
	Camera camera2D;
	camera2D.setOrthographic(0, Game::instance->window_width, 0, Game::instance->window_height, -1, 1);
	Shader::current->setUniform("u_viewprojection", camera2D.viewprojection_matrix);
	Shader::current->setUniform("u_model", model);
	Shader::current->setUniform("u_texture", texture);
	mesh->render(GL_TRIANGLES);
}

void QuadImage::setPosition(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3)
{

	mesh->vertices.clear();
	mesh->uvs.clear();

	//tres vertices del primer triangulo
	mesh->vertices.push_back(Vector3(p2.x, p2.y, 0));
	mesh->uvs.push_back(Vector2(0, 1));
	mesh->vertices.push_back(Vector3(p0.x, p0.y, 0));
	mesh->uvs.push_back(Vector2(0, 0));
	mesh->vertices.push_back(Vector3(p1.x, p1.y, 0));
	mesh->uvs.push_back(Vector2(1, 0));

	//tres vértices del segundo triángulo
	mesh->vertices.push_back(Vector3(p2.x, p2.y, 0));
	mesh->uvs.push_back(Vector2(0, 1));
	mesh->vertices.push_back(Vector3(p1.x, p1.y, 0));
	mesh->uvs.push_back(Vector2(1, 0));
	mesh->vertices.push_back(Vector3(p3.x, p3.y, 0));
	mesh->uvs.push_back(Vector2(1, 1));
}

ButtonImage::ButtonImage(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, const char* filename1, const char* filename2):QuadImage(p0, p1, p2, p3, filename1)
{
	presedButton = new Texture();
	presedButton->load(filename2);
	isSelected = false;
}

ButtonImage::ButtonImage(std::vector <Vector2> points, const char* filename1, const char* filename2):QuadImage(points, filename1)
{
	presedButton = new Texture();
	presedButton->load(filename2);
	isSelected = false;
}

void ButtonImage::render()
{
	Camera camera2D;
	camera2D.setOrthographic(0, Game::instance->window_width, 0, Game::instance->window_height, -1, 1);
	Shader::current->setUniform("u_viewprojection", camera2D.viewprojection_matrix);
	Shader::current->setUniform("u_model", model);
	if(isSelected)
		Shader::current->setUniform("u_texture", presedButton);
	else
		Shader::current->setUniform("u_texture", texture);
	mesh->render(GL_TRIANGLES);

}

DollarBonus::DollarBonus() :Entity()
{
	mesh = Mesh::Get("data/bonus/dollar.OBJ");
	texture = Texture::Get("data/bonus/dollar_texture.tga");
	bonus = rand() % 5 * 5;
}

void DollarBonus::render()
{
	Shader::current->setUniform("u_texture", texture);
	Shader::current->setUniform("u_model", model);
	mesh->render(GL_TRIANGLES);
}

void DollarBonus::update(double seconds_elapsed)
{
	this->rotate((float)seconds_elapsed * 20.0f, Vector3(0,1,0));
}