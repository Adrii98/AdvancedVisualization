#ifndef ENTITY_H
#define ENTITY_H

#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"
#include "animation.h"
#include "extra/coldet/coldet.h"

#include <cmath>

class Order;
enum class OrderState;

class Entity
{
public:
	Entity();

	void setTexture(Texture* t) { texture = t; };
	void setMesh(Mesh* m) { mesh = m; };
	void setModel(Matrix44 m) { model = m; };

	Matrix44 getModel() { return this->model; };
	Mesh* getMesh() { return this->mesh; };
	Texture* getTexture() { return this->texture; };
	int getAngle() { return angle; };
	Vector3 getPosition() { return model.getTranslation(); };

	void move(float x, float y, float z) { model.translate(x, y, z); };
	void moveGlobal(float x, float y, float z) { model.translateGlobal(x, y, z); };
	void rotate(float angle_in_deg, const Vector3& axis) { model.rotate(angle_in_deg * DEG2RAD, axis); angle = fmod(angle + angle_in_deg, 360); angle = (angle >= 0) ? angle : (360 + angle); };
	void scale(float x, float y, float z) { model.scale(x,y,z); };

	void setTranslation(float x, float y, float z) { model.setTranslation(x, y, z); };
	void setRotation(float angle_in_deg, const Vector3& axis) { model.setRotation(angle_in_deg * DEG2RAD, axis);  angle = fmod(angle_in_deg, 360);  angle = (angle >= 0) ? angle : (360 + angle);};
	void setScale(float x, float y, float z) { model.setScale(x, y, z); };
	void setCollisionModel();
	
	virtual void render() = 0;
	void update(double seconds_elapsed) {};

protected:
	Texture* texture;
	Mesh* mesh;
	Matrix44 model;
	CollisionModel3D* collision_model;
	float angle; // Rotation Y axis angle in deg
};

class FloorEntity : public Entity
{
public:
	FloorEntity();
	void render();

private:
	Texture* rTexture;
	Texture* gTexture;
	Texture* bTexture;
	Texture* blendMap;

	float floor_size = 1324.f;
};

class Sky : public Entity
{
public:
	Sky();
	void render();
};

class Character : public Entity
{
public:
	Character();

	Matrix44* getPrevModel() { return &this->prev_model; };
	Vector3 getPrevPosition() { return prev_model.getTranslation(); };

	void move(float x, float y, float z) { prev_model = model; model.translate(x, y, z); };
	void moveGlobal(float x, float y, float z) { prev_model = model; model.translateGlobal(x, y, z); };
	void rotate(float angle_in_deg, const Vector3& axis) { prev_model = model; model.rotate(angle_in_deg * DEG2RAD, axis); angle = fmod(angle + angle_in_deg, 360); angle = (angle >= 0) ? angle : (360 + angle); };
	void scale(float x, float y, float z) { prev_model = model; model.scale(x, y, z); };

	void setTranslation(float x, float y, float z) { prev_model = model; model.setTranslation(x, y, z); };
	void setRotation(float angle_in_deg, const Vector3& axis) { prev_model = model; model.setRotation(angle_in_deg * DEG2RAD, axis);  angle = fmod(angle_in_deg, 360);  angle = (angle >= 0) ? angle : (360 + angle); };
	void setScale(float x, float y, float z) { prev_model = model; model.setScale(x, y, z); };

	void addAnimation(Animation* a) { animations.push_back(a); };
	void still() { anim_state = AnimState::IDLE; };
	void walk() { anim_state = AnimState::WALKING; };
	void run() { anim_state = AnimState::RUNNING; };
	void render();
	void update(double seconds_elapsed, std::vector<Entity*>* objects);

protected:
	std::vector<Animation*> animations;
	enum class AnimState { IDLE, WALKING, RUNNING };
	AnimState anim_state;
	Matrix44 prev_model;

	float RUN_SPEED = 20.f;
	float TURN_SPEED = 160.f;
	float GRAVITY = -100.f;
	float JUMP_POWER = 50.f;
	float TERRAIN_HEIGHT = 0.f;

	float upwardsSpeed = 0.f;

	float currentSpeed = 0.f;
	float currentTurnSpeed = 0.f;

	bool isInAir;
};

class MainCharacter : public Character
{
public:
	MainCharacter();
	void render();
	void update(double seconds_elapsed, std::vector<Entity*>* objects);
	Order* getCurrentOrder() { return this->order; };
	OrderState getOrderState();
	void setOrderState(OrderState state);
	void newOrder(Order* order) { this->order = order; };

	float getMoney() { return this->money; };
	void payOrder();
	void chargeDelivery();
	void addMoney(float amount);
	int getReputation() { return reputation; };
	

private:
	Order* order;
	float money;
	int reputation;
};

class Neighbour : public Character
{
public:
	Neighbour();
	void update(double seconds_elapsed, std::vector<Entity*>* objects);
	bool canSeeTheTarget(std::vector<Entity*>* objects);
	void goToTarget(double seconds_elapsed, std::vector<Entity*>* objects);
	void walk(double seconds_elapsed, std::vector<Entity*>* objects);


	void setTarget(MainCharacter* c) { target = c; };
private:
	enum class State { WALK, GO_AFTER_PLAYER, CLOSE_TO_PLAYER, GO_HOME };
	State state;
	MainCharacter* target;
};

class House : public Entity
{
public:
	House(const char *type);
	void render();
};

class McDonalds : public Entity
{
public:
	McDonalds();
	void render();

private:
	Mesh* mesh2;
	Texture* texture2;
};

class HotDog : public Entity
{
public:
	HotDog();
	void render();
	void setSeller();
private:
	Character seller;
};

class Tree : public Entity
{
public:
	Tree();
	void render();

private:
	Mesh* mesh2;
	Texture* texture2;
};

class QuadImage : public Entity
{
public:
	QuadImage(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, const char* filename);
	QuadImage(std::vector <Vector2> points, const char* filename);
	void render();
	void setImage(const char* filename) { this->texture = Texture::Get(filename); };
	void setPosition(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3);
};

class ButtonImage : public QuadImage
{
public:
	ButtonImage(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, const char* filename1, const char* filename2);
	ButtonImage(std::vector <Vector2> points, const char* filename1, const char* filename2);
	void render();
	void select() { isSelected = true; };
	void unSelect() { isSelected = false; }; 
private:
	bool isSelected;
	Texture* presedButton;
};

class DollarBonus : public Entity
{
public:
		DollarBonus();
		void render();
		void update(double seconds_elapsed);
		float getBonus() { return bonus; };
private:
	float bonus;
};

#endif