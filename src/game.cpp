#include "game.h"
#include "utils.h"
#include "fbo.h"
#include "input.h"
#include "shader.h"
#include "extra/bass.h"

#include <cmath>
#include <iostream>
#include <fstream>

Game* Game::instance = NULL;

InitialStage* initial_stage = NULL;
TransitionStage* transition_stage = NULL;
GameStage* game_stage = NULL;
EndStage* end_stage = NULL;

Stage* current_stage = NULL;


Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	fps = 0;
	frame = 0;
	time = 0.f;
	elapsed_time = 0.f;
	mouse_locked = false;

	difficulty = Difficulty::EASY;
	RENDER_DISTANCE = 1000.f;

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	//create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f,100.f, 100.f),Vector3(0.f,0.f,0.f), Vector3(0.f,1.f,0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f,window_width/(float)window_height,0.1f,10000.f); //set the projection, we want to be perspective

	initial_stage = new InitialStage();
	transition_stage = new TransitionStage();
	game_stage = new GameStage();
	end_stage = new EndStage();

	current_stage = initial_stage;

	//Abrir canal audio
	if (BASS_Init(1, 44100, 0, 0, NULL) == false) {
		must_exit = true;
	}
	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse

}

//what to do when the image has to be draw
void Game::render(void)
{
	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set the camera as default
	camera->enable();

	
	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	//Draw the floor grid
	//drawGrid();
	
	if(current_stage)
		current_stage->render();
	   
	//render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{

	current_stage->update(seconds_elapsed);

	float speed = seconds_elapsed * 100; //the speed is defined by the seconds_elapsed so it goes constant
	
	//async input to move the camera around
	if(Input::isKeyPressed(SDL_SCANCODE_LSHIFT) ) speed *= 10; //move faster with left shift

	//to navigate with the mouse fixed in the middle
	if (mouse_locked)
		Input::centerMouse();

}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
		case SDLK_F1: Shader::ReloadAll(); break; 
	}
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_LEFT) //middle mouse
	{
		current_stage->mouseButtonWasPressed();
	}
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
	if (current_stage != game_stage)
		return;
	float MAX_ZOOM = 99.f;
	float MIN_ZOOM = -300.f;

	float new_zoom = game_stage->getZoom() + Input::mouse_wheel_delta * 5.f;

	if (new_zoom < MAX_ZOOM && new_zoom > MIN_ZOOM)
		game_stage->setZoom(new_zoom);
}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}

void Game::nextStage(int value)
{
	if (current_stage == initial_stage) {
		current_stage = transition_stage;
	}
	else if (current_stage == transition_stage) {
		current_stage = game_stage;
		game_stage->init();
	}

	else if (value == 1) {
		end_stage->select_end = 1;
		current_stage = end_stage;
	}
	else if (value == -1)
	{
	end_stage->select_end = -1;
	current_stage = end_stage;
	}
	else if (current_stage == end_stage)
	{
		current_stage = initial_stage;
		game_stage = new GameStage();
	}
}
void Game::returnStage()
{
	current_stage = initial_stage;
}