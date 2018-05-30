#include <SFML/Graphics.hpp>
#include "header.h"
#include "header1.h"
#include <iostream>
#include <sstream>
float count = 0;
using namespace sf;
class Entity {
public:
   float dx, dy, x, y, speed, moveTimer;
   int w, h, health;
   bool life, isMove, onGround;
   Texture texture;
   Sprite sprite;
   String name;
   Entity(Image &image, float X, float Y, int W, int H, String Name) {
      x = X; y = Y; w = W; h = H; name = Name; moveTimer = 0;
      speed = 0; health = 100; dx = 0; dy = 0;
      life = true; onGround = false; isMove = false;
      texture.loadFromImage(image);
      sprite.setTexture(texture);
      sprite.setOrigin(w / 2, h / 2);
   }
};

class Player :public Entity {
public:
   enum { left, right, up, down, jump, stay } state;
   int playerScore;

   Player(Image &image, float X, float Y, int W, int H, String Name) :Entity(image, X, Y, W, H, Name) {
      playerScore = 0; state = stay;
      if (name == "Player1") {
         sprite.setTextureRect(IntRect(0, 0, w, h));
      }
   }

   void control() {
      if (Keyboard::isKeyPressed) {
         if (Keyboard::isKeyPressed(Keyboard::Left)) {
            state = left; speed = 0.1;
         }
         if (Keyboard::isKeyPressed(Keyboard::Right)) {
            state = right; speed = 0.1;
         }

         if ((Keyboard::isKeyPressed(Keyboard::Up)) && (onGround)) {
            state = jump; dy = -0.6; onGround = false;
         }

         if (Keyboard::isKeyPressed(Keyboard::Down)) {
            state = down;
         }
      }
   }

   void checkCollisionWithMap(float Dx, float Dy)
   {
      for (int i = y / 32; i < (y + h) / 32; i++)
         for (int j = x / 32; j<(x + w) / 32; j++)
         {
            if (TileMap[i][j] == '0')
            {
               if (Dy>0) { y = i * 32 - h;  dy = 0; onGround = true; }
               if (Dy<0) { y = i * 32 + 32;  dy = 0; }
               if (Dx>0) { x = j * 32 - w; }
               if (Dx<0) { x = j * 32 + 32; }
            }
         }
   }

   void update(float time)
   {
      control();
      switch (state)
      {
      case right:dx = speed; break;
      case left:dx = -speed; break;
      case up: break;
      case down: dx = 0; break;
      case stay: break; 
      }
      x += dx * time*2;
      checkCollisionWithMap(dx, 0);
      y += dy * time*2;
      checkCollisionWithMap(0, dy);
      sprite.setPosition(x + w / 2, y + h / 2); 
      if (health <= 0) { life = false; }
      if (!isMove) { speed = 0; }
      
      if (life) { setPlayerCoordinateForView(x, y); }
      dy = dy + 0.0015*time;
   }
};


class Enemy :public Entity {
public:
   Enemy(Image &image, float X, float Y, int W, int H, String Name) :Entity(image, X, Y, W, H, Name) {
      if (name == "EasyEnemy") {
         sprite.setTextureRect(IntRect(0, 0, w, h));
         dx = 0.1;
      }
   }

   void checkCollisionWithMap(float Dx, float Dy)
   {
      for (int i = y / 32; i < (y + h) / 32; i++)
         for (int j = x / 32; j<(x + w) / 32; j++)
         {
            if (TileMap[i][j] == '0')
            {
               if (Dy>0) { y = i * 32 - h; }
               if (Dy<0) { y = i * 32 + 32; }
               if (Dx>0) { x = j * 32 - w; dx = -0.1; sprite.scale(-1, 1); }
               if (Dx<0) { x = j * 32 + 32; dx = 0.1; sprite.scale(-1, 1); }
            }
         }
   }

   virtual void update(float time)
   {
      if (name == "EasyEnemy") {

                          
         checkCollisionWithMap(dx, 0);
         x += dx * time;
         sprite.setPosition(x + w / 2, y + h / 2); 
         if (health <= 0) { life = false; }
      }
   }
};

class HardEnemy : public Enemy
{
public:
   float koef;
   HardEnemy(Image &image, float X, float Y, int W, int H, String Name, float k) : Enemy(image, X, Y, W, H, Name), koef(k) { }
   void update(float time)
   {
      static float timer; timer += time;
      if (name == "HardEnemy") {
         checkCollisionWithMap(0.2, 0);
         x += 0.5*koef;
         sprite.setPosition(x +w/2, y+h/2); 
         if (health <= 0) { life = false; }
         if (timer > 800) {x -= 200; timer = 0;}
      }
   }
};

class Obama : public HardEnemy
{
public:
   Obama(Image &image, float X, float Y, int W, int H, String Name, float k) : HardEnemy(image, X, Y, W, H, Name, k) { }
   void update(float time)
   {
      static float timer; timer += time;
      if (name == "Obama") {
         checkCollisionWithMap(0, 0.2);
         y += 0.5*koef;
         sprite.setPosition(x + w / 2, y + h / 2); 
         if (health <= 0) { life = false; }
         if (timer > 800) { y -= 175; timer = 0; }
      }
   }
};

class McGregor : public Obama
{
public:
   McGregor(Image &image, float X, float Y, int W, int H, String Name, float k) : Obama(image, X, Y, W, H, Name, k) { }
   void update(float time)
   {
      static float timer; timer += time;
      if (name == "McGregor") {
         checkCollisionWithMap(0.2, 0.2);
         y += 0.2*time;
         x += 0.2*time;
         sprite.setPosition(x + w / 2, y + h / 2);
         if (health <= 0) { life = false; }
         if (timer > 1400) { y = 300; x = 100; timer = 0; }
      }
   }
};

int main()
{
   RenderWindow window(VideoMode(640, 480), "The Bullet Story");
   view.reset(FloatRect(0, 0, 640, 480));

   Image map_image;
   map_image.loadFromFile("ггг.png");
   Texture map;
   map.loadFromImage(map_image);
   Sprite s_map;
   s_map.setTexture(map);

   Image heroImage;
   heroImage.loadFromFile("sha.png");
   heroImage.createMaskFromColor(Color(255, 255, 255));

   Image easyEnemyImage;
   easyEnemyImage.loadFromFile("pp.png");
   easyEnemyImage.createMaskFromColor(Color(0, 0, 0));

   Image JumpEnemyImage;
   JumpEnemyImage.loadFromFile("шш.png");
   JumpEnemyImage.createMaskFromColor(Color(0, 0, 0));

   Image Obm;
   Obm.loadFromFile("obama.png");

   Image Mcg;
   Mcg.loadFromFile("mcgregor.png");

   Player p(heroImage, 750, 500, 148, 55, "Player1");
   Enemy easyEnemy(easyEnemyImage, 850, 671, 192, 97, "EasyEnemy");
   HardEnemy HEnemy(JumpEnemyImage, 850,671, 32, 32, "HardEnemy",2);
   Obama Obam(Obm, 850, 671, 32, 32, "Obama", 2);
   McGregor Mc(Mcg,300,100,32,32,"McGregor", 2);
   Enemy* mass[4]; mass[0] = &easyEnemy; mass[1] = &HEnemy; mass[2] = &Obam; mass[3] = &Mc;
   
   Clock clock;
   while (window.isOpen())
   {

      float time = clock.getElapsedTime().asMicroseconds();

      clock.restart();
      time = time / 800;

      Event event;
      while (window.pollEvent(event))
      {
         if (event.type == sf::Event::Closed)
            window.close();
      }
      p.update(time); 
      mass[0]->update(time);
      mass[1]->update(time);
      mass[2]->update(time);
      mass[3]->update(time);
      window.setView(view);
      window.clear();


      for (int i = 0; i < HEIGHT_MAP; i++)
         for (int j = 0; j < WIDTH_MAP; j++)
         {
            if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 0, 32, 32));
            if (TileMap[i][j] == 's')  s_map.setTextureRect(IntRect(32, 0, 32, 32));
            if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(64, 0, 32, 32));
            if ((TileMap[i][j] == 'f')) s_map.setTextureRect(IntRect(96, 0, 32, 32));
            if ((TileMap[i][j] == 'h')) s_map.setTextureRect(IntRect(128, 0, 32, 32));
            s_map.setPosition(j * 32, i * 32);

            window.draw(s_map);
         }
      window.draw(mass[0]->sprite);
      window.draw(mass[1]->sprite);
      window.draw(mass[2]->sprite);
      window.draw(mass[3]->sprite);
      window.draw(p.sprite);
      window.display();
   }
   return 0;
}
