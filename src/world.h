/*

#ifndef WORLD_H
#define WORLD_H

#include "entity.h"
#include "utils.h"

class World
{
public:
	World();

	static World* instance;

	void render(void);
	void update(double seconds_elapsed);

	void setEntities();
	void readPropFile(const char* filename, std::vector<Entity*>* objects, std::vector<Character*>* characters);

};

#endif

*/