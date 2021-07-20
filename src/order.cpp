#include "order.h"
#include "game.h"

#include <iomanip>
#include <stdlib.h>
#include <time.h>

Order::Order(House* house, Entity* shop)
{
	state = OrderState::NOT_PICKED_UP;
	this->order_time = 180000.f / (int) Game::instance->difficulty;

	srand(time(NULL));

	cost = 5.f * (rand() % 5 + 1);
	reward = cost + (5.f * (rand() % 5 + 1));
	this->house = house;
	this->shop = shop;

	if (dynamic_cast<McDonalds*>(shop) != nullptr)
		this->type = OrderType::BURGER;
	else
		this->type = OrderType::HOT_DOG;

	if(this->type == OrderType::BURGER)
		icon = new QuadImage(Vector2(-0.95, -0.95), Vector2(-0.85, -0.95), Vector2(-0.95, -0.85), Vector2(-0.85, -0.85), "data/icons/burger_bw.tga");
	else
		icon = new QuadImage(Vector2(-0.95, -0.95), Vector2(-0.85, -0.95), Vector2(-0.95, -0.85), Vector2(-0.85, -0.85), "data/icons/hotdog_bw.tga");

	message_box = new QuadImage(Vector2(0.3,0.5), Vector2(1, 0.5), Vector2(0.3, 1), Vector2(1, 1), "data/message_box.tga");

	messageIsShowed = true;

	has_finished = false;
}

void Order::render(void) 
{
	icon->render();
	if(messageIsShowed)
		message_box->render();
	std::string remaining_time = format_duration(order_time);

	int scale;
	scale = (Game::instance->window_height > 800 && Game::instance->window_width > 1000) ? 5 : 2;

	drawText(Game::instance->window_width * 0.65, Game::instance->window_height * 0.85, remaining_time, Vector3(1, 1, 1), scale);
	if (messageIsShowed)
	{
		if (state == OrderState::NOT_PICKED_UP || state == OrderState::GO_TO_DELIVER)
		{
			std::string message;
			if (this->type == OrderType::BURGER)
				message = "New Burgers Order.\nCost: " + std::to_string(cost).substr(0, 4) + "$\nRewards: " + std::to_string(reward).substr(0, 4) + "$";
			else
				message = "New HotDogs Order.\nCost: " + std::to_string(cost).substr(0, 4) + "$\nRewards: " + std::to_string(reward).substr(0, 4) + "$";
			drawText(Game::instance->window_width * 0.7, Game::instance->window_height * 0.05, message, Vector3(1, 1, 1), scale);
		}
		else if (state == OrderState::STOLEN)
		{
			std::string message = "Your order was\nstolen by a neighbor";
			drawText(Game::instance->window_width * 0.7, Game::instance->window_height * 0.05, message, Vector3(1, 1, 1), scale);
		}
		else if (state == OrderState::TIME_FINISHED)
		{
			std::string message = "Time is over.\nYou cannot deliver\nthis order";
			drawText(Game::instance->window_width * 0.7, Game::instance->window_height * 0.05, message, Vector3(1, 1, 1), scale);
		}
		else if (state == OrderState::DELIVERED)
		{
			std::string message = "Order delivered\non time!";
			drawText(Game::instance->window_width * 0.7, Game::instance->window_height * 0.05, message, Vector3(1, 1, 1), scale);
		}
	}

	
}
void Order::update(double seconds_elapsed)
{
	if(state == OrderState::NOT_PICKED_UP || state == OrderState::GO_TO_DELIVER)

	order_time = order_time - (seconds_elapsed * 1000);
}

void Order::setOrderState(OrderState state)
{
	this->state = state;

	if (state == OrderState::GO_TO_DELIVER && this->type == OrderType::BURGER)
		icon->setImage("data/icons/burger.tga");
	else if (state == OrderState::GO_TO_DELIVER && this->type == OrderType::HOT_DOG)
		icon->setImage("data/icons/hotdog.tga");

	if (state != OrderState::NOT_PICKED_UP && state != OrderState::GO_TO_DELIVER)
		messageIsShowed = true;
}

std::string Order::format_duration(long ms) {

	long min = ms / 60000;
	ms = ms - 60000 * min;

	long sec = ms / 1000;

	std::stringstream ss;
	ss << " Remaining Time: " << std::setw(2) << std::setfill('0') << min << ":" << std::setw(2) << std::setfill('0') << sec;
	return ss.str();
}