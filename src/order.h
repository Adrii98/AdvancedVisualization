#ifndef ORDER_H
#define ORDER_H

#include "entity.h"

enum class OrderState { NOT_PICKED_UP, GO_TO_DELIVER, DELIVERED, STOLEN, TIME_FINISHED };
enum class OrderType { BURGER, HOT_DOG};
class Order
{
public:

	Order(House* house, Entity* hotdog);
	void render(void);
	void update(double seconds_elapsed);

	void assingHouse(House* house) { this->house = house; };
	void assingShop(Entity* shop) { this->shop = shop; };

	House* getHouse() { return this->house; };
	Entity* getShop() { return this->shop; };

	OrderState getOrderState() { return this->state; };
	void setOrderState(OrderState state);
	OrderType getType() { return this->type; };
	float getCost() { return this->cost; };
	float getReward() { return this->reward; };
	float getTime() { return order_time; };

	bool messageIsShowing() { return messageIsShowed; };
	void showMessage() { messageIsShowed = true; };
	void unshowMessage() { messageIsShowed = false; };
	bool hasFinished() { return has_finished; };
	void finishOrder() { has_finished = true; };

	std::string format_duration(long milliseconds);

private:
	QuadImage* icon;
	QuadImage* message_box;
	bool messageIsShowed;
	OrderState state;
	long order_time; // In ms
	float cost;
	float reward;
	House* house;
	Entity* shop;
	OrderType type;
	bool has_finished;
};

#endif
