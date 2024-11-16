#include <SFML/Graphics.hpp>
using namespace sf;

const int NUMBER_OF_CHECKPOINTS = 8; //checkpoints
int points[NUMBER_OF_CHECKPOINTS][2] = {300, 610,
                      1270,430,
                      1380,2380,
                      1900,2460,
                      1970,1700,
                      2550,1680,
                      2560,3150,
                      500, 3300};

struct Car
{
  float x,y,speed,angle; int checkpointCounter;

  Car() {speed=2; angle=0; checkpointCounter=0;}

  void move()
   {
    x += sin(angle) * speed;
    y -= cos(angle) * speed;
   }

  void findTarget()
  {
    const int CHECKPOINT_SIZE = 25;
    const float TURN_SPEED = 0.005;
    float xTarget=points[checkpointCounter][0]; // gets x position of the checkpoint
    float yTarget=points[checkpointCounter][1]; // gets y position of the checkpoint
    float angleDifferenceToTarget = angle-atan2(xTarget-x,-yTarget+y); // calculates the difference between angle of car and target
    if (sin(angleDifferenceToTarget)<0) angle+=TURN_SPEED*speed; else angle-=TURN_SPEED*speed; // if angle is less than 0, means target is to the left of the car, else its to the right
    if ((x-xTarget)*(x-xTarget)+(y-yTarget)*(y-yTarget)<CHECKPOINT_SIZE*CHECKPOINT_SIZE) checkpointCounter=(checkpointCounter+1)%NUMBER_OF_CHECKPOINTS; // calculates the squared distance between car and checkpoint, then checks if car is within 25 units of the checkpoint
   }
};

// struct for const movement related variables
struct Movement
{
   static constexpr float MAX_SPEED = 12.0f;
   static constexpr float ACCELERATION = 0.2f;
   static constexpr float DECELERATION = 0.3f;
   static constexpr float TURN_SPEED = 0.08f;
};
  // function which sets up textures
void setTextures(Texture &backGroundTexture, Texture &carTexture)
{
    backGroundTexture.loadFromFile("images/racing/background.png");
    carTexture.loadFromFile("images/racing/car.png");
    backGroundTexture.setSmooth(true);
    carTexture.setSmooth(true);
}
// function which sets positions of all the cars
void setNPCPositions(Car cars[], const int NUMBER_OF_CARS)
{
    const int NPC_CAR_POS_X = 300;
    const int NPC_CAR_POS_Y = 1700;
    const int NPC_OFFSET_X = 50;
    const int NPC_OFFSET_Y = 80;

    for (int i = 0; i < NUMBER_OF_CARS; i++)
    {
        cars[i].x = NPC_CAR_POS_X + i * NPC_OFFSET_X;
        cars[i].y = NPC_CAR_POS_Y + i * NPC_OFFSET_Y;
        cars[i].speed = 7 + i;
    }
}

// function which deals with player movement
void playerMove(Car cars[], float &speed, float &angle)
{
    bool Forward = 0, Right = 0, Backward = 0, Left = 0;
    if (Keyboard::isKeyPressed(Keyboard::Up)) Forward = 1;
    if (Keyboard::isKeyPressed(Keyboard::Right)) Right = 1;
    if (Keyboard::isKeyPressed(Keyboard::Down)) Backward = 1;
    if (Keyboard::isKeyPressed(Keyboard::Left)) Left = 1;

    // if moving forward
    if (Forward && speed < Movement::MAX_SPEED)
        if (speed < 0)  speed += Movement::DECELERATION;
        else  speed += Movement::ACCELERATION;
    // if moving back
    if (Backward && speed > -Movement::MAX_SPEED)
        if (speed > 0) speed -= Movement::DECELERATION;
        else  speed -= Movement::ACCELERATION;
    // if not moving back or forward
    if (!Forward && !Backward)
        if (speed - Movement::DECELERATION > 0) speed -= Movement::DECELERATION;
        else if (speed + Movement::DECELERATION < 0) speed += Movement::DECELERATION;
        else speed = 0;
    // turning right and left
    if (Right && speed != 0)  angle += Movement::TURN_SPEED * speed / Movement::MAX_SPEED;
    if (Left && speed != 0)   angle -= Movement::TURN_SPEED * speed / Movement::MAX_SPEED;
    // set the new speed and turn angle
    cars[0].speed = speed;
    cars[0].angle = angle;
}
// function which deals with npc movement
void npcMove(Car cars[], const int NUMBER_OF_CARS)
{
    for (int i = 0; i < NUMBER_OF_CARS; i++) cars[i].move();
    for (int i = 1; i < NUMBER_OF_CARS; i++) cars[i].findTarget();
}
// function which deals with collisions
void carCollision(Car npcCars[], const int NUMBER_OF_CARS, float radius)
{
    for (int carIndex = 0; carIndex < NUMBER_OF_CARS; carIndex++) // loop selects a car
        for (int otherCarIndex = 0; otherCarIndex < NUMBER_OF_CARS; otherCarIndex++) // compares selected car to other cars
        {
            int xDistance = 0, yDistance = 0; // horizontal and vertical distance between 2 cars
            while (xDistance * xDistance + yDistance * yDistance < 4 * radius * radius) // while the squared distance between between 2 cars is less than the set collision threshold
            {
                // moves the 2 cars in opposite directions to prevent overlapping
                npcCars[carIndex].x += xDistance / 10.0;
                npcCars[carIndex].x += yDistance / 10.0;
                npcCars[otherCarIndex].x -= xDistance / 10.0;
                npcCars[otherCarIndex].y -= xDistance / 10.0;
                // distances between the cars recalculated to check while loop
                xDistance = npcCars[carIndex].x - npcCars[otherCarIndex].x;
                yDistance = npcCars[carIndex].y - npcCars[otherCarIndex].y;
                if (!xDistance && !yDistance) break; // prevents infinite looping if cars are exactly on top of each other
            }
        }
}
// function which draws the game
void drawGame(RenderWindow &app, Sprite backgroundSprite, int &offsetX, int &offsetY, Sprite carSprite, Car cars[], const int NUMBER_OF_CARS)
{
    const float PI = 3.141593;
    app.clear(Color::White);
    app.draw(backgroundSprite);

    Color colors[10] = { Color::Red, Color::Green, Color::Magenta, Color::Blue, Color::White };
    // cycles through all cars setting their position, rotation, colour and sprite
    for (int i = 0; i < NUMBER_OF_CARS; i++)
    {
        carSprite.setPosition(cars[i].x - offsetX, cars[i].y - offsetY);
        carSprite.setRotation(cars[i].angle * 180 / PI);
        carSprite.setColor(colors[i]);
        app.draw(carSprite);
    }

    app.display();

}

// main game loop
int racing()
{
    RenderWindow app(VideoMode(640, 480), "Car Racing Game!");
    app.setFramerateLimit(60);

    Texture backGroundTexture,carTexture,t3;
    setTextures(backGroundTexture, carTexture);

    Sprite backgroundSprite(backGroundTexture), carSprite(carTexture);
    backgroundSprite.scale(2,2);

    carSprite.setOrigin(22, 22);
    float radius=22;

    const int NUMBER_OF_CARS = 5;
    Car cars[NUMBER_OF_CARS];

   float speed=0,angle=0;

   int offsetX=0,offsetY=0;

   setNPCPositions(cars, NUMBER_OF_CARS);
   
   // loops while app is open
    while (app.isOpen())
    {
        Event e;
        while (app.pollEvent(e))
        {
            if (e.type == Event::Closed)
                app.close();
        }

    playerMove(cars, speed, angle);
    npcMove(cars, NUMBER_OF_CARS); 

    carCollision(cars, NUMBER_OF_CARS, radius); 

    drawGame(app, backgroundSprite, offsetX, offsetY, carSprite, cars, NUMBER_OF_CARS);
    // moves the camera to follow the player
    if (cars[0].x > 320) offsetX = cars[0].x - 320;
    if (cars[0].y > 240) offsetY = cars[0].y - 240;
    // sets new position of the background sprite
    backgroundSprite.setPosition(-offsetX, -offsetY);
    
    }

    return 0;

};


