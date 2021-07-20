#ifndef STAGE_H
#define STAGE_H

#include "texture.h"
#include "order.h"
#include "entity.h"
#include "extra/bass.h"

enum class MenuPos {START, DIFFICULTY, EXIT};

class Stage
{
public:
	Stage() {};
	virtual void render(void) = 0;
	virtual void update(double seconds_elapsed) = 0;
	virtual void mouseButtonWasPressed() = 0;
	bool hasClicked(std::vector<Vector2> points);
};

class InitialStage : public Stage
{
public:
	InitialStage();
	void render(void);
	void update(double seconds_elapsed);
	void mouseButtonWasPressed();
	
private:
	Shader* quad_shader;
	MenuPos menu_pos;
	QuadImage* background;
	QuadImage* title;
	std::vector <Vector2> title_points;
	std::vector <Vector2> background_points;
	ButtonImage* start_button;
	std::vector <Vector2> start_button_points;
	ButtonImage* easy_button;
	std::vector <Vector2> difficulty_button_points;
	ButtonImage* medium_button;
	ButtonImage* hard_button;
	ButtonImage* exit_button;
	std::vector <Vector2> exit_button_points;	

	HSAMPLE hSample;
	HCHANNEL hSampleChannel;
};
class TransitionStage :public Stage
{
public:
	TransitionStage();
	void render(void);
	void update(double seconds_elapsed);
	void mouseButtonWasPressed() {};
private:
	Shader* quad_shader;
	QuadImage* background;
	std::vector <Vector2> background_points;
};

class GameStage : public Stage
{
public:
	GameStage();
	void render(void);
	void update(double seconds_elapsed);
	void init();
	void mouseButtonWasPressed();
	
	void addObject(Entity* object) { this->objects.push_back(object); };
	void setObjects(std::vector<Entity*> objects) { this->objects = objects; };
	void addNeighbour(Neighbour* neighbour) { this->neighbours.push_back(neighbour); };
	void setNeighbours(std::vector<Neighbour*> neighbours) { this->neighbours = neighbours; };

	void setPlayer(MainCharacter* p) { this->player = p; };
	bool hasNeighbours() { return (neighbours.size() != 0); };

	void readPropFile(const char* filename);

	void setZoom(float zoom) { this->zoom = zoom; };
	float getZoom() { return zoom; };

	std::vector<Entity*> getObjectsInArea(Vector3 point, float dist);

	void lookAtCharacter();
	Vector3 calculateCameraPosition();

private:
	Shader* shader;
	Shader* anim_shader;
	Shader* terrain_shader;
	Shader* quad_shader;

	Sky* sky;
	FloorEntity* world_floor;
	Tree* tree;

	std::vector<Entity*> objects;
	std::vector<Neighbour*> neighbours; 

	bool show_map;

	QuadImage* map;
	QuadImage* player_dot;
	QuadImage* dest_dot;
	QuadImage* reputation_star;

	MainCharacter* player;
	Order* curr_order;
	std::vector <Vector2> message_box_points;

	McDonalds* mcdonalds;
	HotDog* hotdog_cart;

	DollarBonus* bonus;
	std::vector <Vector3> dollar_positions;

	float zoom, angle, pitch, yaw, roll;

	float time_for_next_order;
	float time_for_next_bonus;
};

class EndStage : public Stage
{
public:
	EndStage();
	void render(void);
	void update(double seconds_elapsed);
	void mouseButtonWasPressed() {};
	int select_end;
private:
	Shader* quad_shader;
	QuadImage* end1;
	QuadImage* end2;
	std::vector <Vector2> background_points;
};



#endif

