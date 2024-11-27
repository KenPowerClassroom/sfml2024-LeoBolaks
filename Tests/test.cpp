#include "pch.h"
#define SFML_STATIC
#pragma comment(lib,"sfml-graphics-s-d.lib")
#pragma comment(lib,"sfml-audio-s-d.lib")
#pragma comment(lib,"sfml-system-s-d.lib")
#pragma comment(lib,"sfml-window-s-d.lib")
#pragma comment(lib,"sfml-network-s-d.lib")

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"freetype.lib")


#include"../16_SFML_Games/racing.cpp"


TEST(Car, movesCorrectly)
{
	Car car;
	car.x = 0;
	car.y = 0;
	car.speed = 5;
	car.angle = 0; // car facing directly upwards

	car.move();

	ASSERT_FLOAT_EQ(car.x, 0); 
	ASSERT_FLOAT_EQ(car.y, -5);
}

TEST(Car, findsCorrectTarget)
{
	Car car;

	car.x = 1380;
	car.y = 2380;
	car.checkpointCounter = 2;

	car.findTarget();

	EXPECT_EQ(car.checkpointCounter, 3); 
}

TEST(GameSetup, SetNPCPositions)
{
	Car cars[5];
	setNPCPositions(cars, 5);

	// check if initial position was set
	ASSERT_FLOAT_EQ(cars[0].x, 300);
	ASSERT_FLOAT_EQ(cars[0].y, 1700);

	// check for correct offset
	ASSERT_FLOAT_EQ(cars[1].x, 350);
	ASSERT_FLOAT_EQ(cars[1].y, 1780); 
}

TEST(GamePhysics, CarCollision)
{
	Car cars[2];
	cars[0].x = 100; cars[0].y = 100; 
	cars[1].x = 110; cars[1].y = 110; 
	// radius of car
	float radius = 15;

	carCollision(cars, 2, radius); 

	// calculates the distance squared between the 2 cars
	float distanceSquared = (cars[0].x - cars[1].x) * (cars[0].x - cars[1].x) + (cars[0].y - cars[1].y) * (cars[0].y - cars[1].y); 

	ASSERT_GE(distanceSquared, 4 * radius * radius); 
}

