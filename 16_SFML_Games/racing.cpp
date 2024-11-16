#include <SFML/Graphics.hpp>
using namespace sf;

const int numCheckpoints=8; //checkpoints
int points[numCheckpoints][2] = {300, 610,
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
    float xTarget=points[checkpointCounter][0]; // gets x position of the checkpoint
    float yTarget=points[checkpointCounter][1]; // gets y position of the checkpoint
    float angleDifferenceToTarget = angle-atan2(xTarget-x,-yTarget+y); // calculates the difference between angle of car and target
    if (sin(angleDifferenceToTarget)<0) angle+=0.005*speed; else angle-=0.005*speed; // if angle is less than 0, means target is to the left of the car, else its to the right
    if ((x-xTarget)*(x-xTarget)+(y-yTarget)*(y-yTarget)<25*25) checkpointCounter=(checkpointCounter+1)%numCheckpoints; // calculates the squared distance between car and checkpoint, then checks if car is within 25 units of the checkpoint
   }
};
  


void setNPCPositions(Car npcCars[], int numNpcCars)
{
    for (int i = 0; i < numNpcCars; i++)
    {
        npcCars[i].x = 300 + i * 50;
        npcCars[i].y = 1700 + i * 80;
        npcCars[i].speed = 7 + i;
    }
}

void playerMove(Car npcCars[], float &speed, float &angle, float maxSpeed, float acceleration, float deceleration , float turnSpeed)
{
    bool Up = 0, Right = 0, Down = 0, Left = 0;
    if (Keyboard::isKeyPressed(Keyboard::Up)) Up = 1;
    if (Keyboard::isKeyPressed(Keyboard::Right)) Right = 1;
    if (Keyboard::isKeyPressed(Keyboard::Down)) Down = 1;
    if (Keyboard::isKeyPressed(Keyboard::Left)) Left = 1;

    //car movement
    if (Up && speed < maxSpeed)
        if (speed < 0)  speed += deceleration;
        else  speed += acceleration;

    if (Down && speed > -maxSpeed)
        if (speed > 0) speed -= deceleration;
        else  speed -= acceleration;

    if (!Up && !Down)
        if (speed - deceleration > 0) speed -= deceleration;
        else if (speed + deceleration < 0) speed += deceleration;
        else speed = 0;

    if (Right && speed != 0)  angle += turnSpeed * speed / maxSpeed;
    if (Left && speed != 0)   angle -= turnSpeed * speed / maxSpeed;

    npcCars[0].speed = speed;
    npcCars[0].angle = angle;
}

void npcMove(Car npcCars[], float numNpcCars)
{
    for (int i = 0; i < numNpcCars; i++) npcCars[i].move();
    for (int i = 1; i < numNpcCars; i++) npcCars[i].findTarget();
}

void carCollision(Car npcCars[], float numNpcCars, float radius)
{
    for (int carIndex = 0; carIndex < numNpcCars; carIndex++) // loop selects a car
        for (int otherCarIndex = 0; otherCarIndex < numNpcCars; otherCarIndex++) // compares selected car to other cars
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

void drawGame(RenderWindow &app, Sprite backgroundSprite, int &offsetX, int &offsetY, Sprite carSprite, Car npcCars[], float numNpcCars)
{
    app.clear(Color::White);
    app.draw(backgroundSprite);

    Color colors[10] = { Color::Red, Color::Green, Color::Magenta, Color::Blue, Color::White };

    for (int i = 0; i < numNpcCars; i++)
    {
        carSprite.setPosition(npcCars[i].x - offsetX, npcCars[i].y - offsetY);
        carSprite.setRotation(npcCars[i].angle * 180 / 3.141593);
        carSprite.setColor(colors[i]);
        app.draw(carSprite);
    }

    app.display();

}


int racing()
{
    RenderWindow app(VideoMode(640, 480), "Car Racing Game!");
    app.setFramerateLimit(60);

    Texture backGroundTexture,carTexture,t3;
    backGroundTexture.loadFromFile("images/racing/background.png");
    carTexture.loadFromFile("images/racing/car.png");
    backGroundTexture.setSmooth(true);
    carTexture.setSmooth(true);

    Sprite backgroundSprite(backGroundTexture), carSprite(carTexture);
    backgroundSprite.scale(2,2);

    carSprite.setOrigin(22, 22);
    float radius=22;

    const int numNpcCars=5;
    Car npcCars[numNpcCars];

   float speed=0,angle=0;
   float maxSpeed=12.0;
   float acceleration=0.2, deceleration=0.3;
   float turnSpeed=0.08;

   int offsetX=0,offsetY=0;

   setNPCPositions(npcCars, numNpcCars);
   

    while (app.isOpen())
    {
        Event e;
        while (app.pollEvent(e))
        {
            if (e.type == Event::Closed)
                app.close();
        }

    playerMove(npcCars, speed, angle, maxSpeed, acceleration, deceleration, turnSpeed);
    npcMove(npcCars, numNpcCars);

    //collision
    carCollision(npcCars, numNpcCars, radius);

    drawGame(app, backgroundSprite, offsetX, offsetY, carSprite, npcCars, numNpcCars);

    if (npcCars[0].x > 320) offsetX = npcCars[0].x - 320;
    if (npcCars[0].y > 240) offsetY = npcCars[0].y - 240;

    backgroundSprite.setPosition(-offsetX, -offsetY);
    
    }

    return 0;

};


