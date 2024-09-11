#include <iostream>
#include "raylib.h"
#include <deque>
#include "raymath.h"

using namespace std;
Color green = {170,200,90,255};
Color darkGreen ={43,51,24,155};

int cellSize =30;
int cellCount =25;
int offset = 75;

double lastUpdatetime =0;

bool eventTriggered(double interval){

    double currentTime = GetTime();

    if(currentTime - lastUpdatetime >= interval){

        lastUpdatetime = currentTime;
        return true;
    }
    return false;

}

bool ElementInDeque(Vector2 element, deque<Vector2> deque){

    for(unsigned int i =0;i<deque.size();i++){

        if(Vector2Equals(deque[i],element)){
            return true;
        }
    }
    return false;
}

class Snake{
public:
     deque<Vector2> body = {Vector2{6,9},Vector2{5,9},Vector2{4,9}};

     Vector2 direction ={1,0};
     bool addSegment =false;
     Texture2D texture;

     Snake(){
         Image image =LoadImage("Texturs/snake.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
     }

    void Draw() {
    for (unsigned int i = 0; i < body.size(); i++) {
        float x = body[i].x;
        float y = body[i].y;

        if (i == 0) {
            // Determine the rotation based on the direction, starting with no default rotation
            float rotation = 0.0f;
            if (direction.x == 1 && direction.y == 0) {
                rotation = 0.0f;  // Right
            } else if (direction.x == -1 && direction.y == 0) {
                rotation = 180.0f; // Left
            } else if (direction.x == 0 && direction.y == -1) {
                rotation = 270.0f; // Up
            } else if (direction.x == 0 && direction.y == 1) {
                rotation = 90.0f;  // Down
            }

            // Draw the head of the snake using the texture with proper rotation
            Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width, (float)texture.height }; // Full texture
            // Correct the destination position to ensure no shift due to rotation
            Rectangle destRec = { offset + x * cellSize + cellSize / 2.0f, offset + y * cellSize + cellSize / 2.0f, (float)cellSize, (float)cellSize };
            Vector2 origin = { cellSize / 2.0f, cellSize / 2.0f }; // Set the origin to the center of the texture for proper rotation
            DrawTexturePro(texture, sourceRec, destRec, origin, rotation, WHITE); // Draw the texture with the calculated rotation
        } else {
            // Draw the body segments as rectangles
            Rectangle segment = Rectangle{ offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize };
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }
}



     void Update(){
        body.push_front(Vector2Add(body[0],direction));
        if(addSegment==true){

            addSegment =false;

        }else{

            body.pop_back();
        
        }
        
     }

     void Reset(){
        body ={Vector2{6,9},Vector2{5,9},Vector2{4,9}};
        direction ={1,0};
     }

};

class Food
{
public:
 Vector2 position ;
 float scale =0.05f;
Texture2D texture;
 Food( deque<Vector2> SnakeBody){

    Image image =LoadImage("Texturs/pngwing.com.png");
    texture = LoadTextureFromImage(image);
    UnloadImage(image);
    position =GenerateRandomspown(SnakeBody);
 }
 ~Food(){
    UnloadTexture(texture);
 }
 void Draw(){
    DrawTextureEx(texture, (Vector2){offset+position.x * cellSize,offset+ position.y * cellSize}, 0.0f, scale, WHITE);

 }
 Vector2 GenerateRandomCell(){
    float x = GetRandomValue(0,cellCount-1);
    float y = GetRandomValue(0,cellCount-1);
    return Vector2{x,y};
 }

 Vector2 GenerateRandomspown( deque<Vector2> snakeBody){
    float x = GetRandomValue(0,cellCount-1);
    float y = GetRandomValue(0,cellCount-1);
    Vector2 position = {x,y};
    while(ElementInDeque(position,snakeBody)){
        position=GenerateRandomCell();
    }
    return position;
 }
};
class Game{

public: 
    Snake snake =Snake();
    Food food= Food(snake.body);
    bool running =true;
    int score =0;
    Sound eatSound;
    Sound wallSound;

    Game(){
        InitAudioDevice();
        eatSound = LoadSound("Sounds/retro-video-game-coin-pickup-38299.mp3");
        wallSound = LoadSound("Sounds/classic-game-action-negative-3-224421.mp3");
    }
    ~Game(){
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    void Draw(){
        food.Draw();
        snake.Draw();
    }

    void Update(){
        if(running){
        snake.Update();
        CheckCollisionWhithFood();
        CheckCollisionWhithEdges();
        CheckCollisionInTail();
        }
    }

    void CheckCollisionWhithFood(){
        if(Vector2Equals(snake.body[0],food.position)){
            food.position = food.GenerateRandomspown(snake.body);
            snake.addSegment =true;
            score++;
            PlaySound(eatSound);
        }

    }

    void CheckCollisionWhithEdges(){

        if(snake.body[0].x==cellCount || snake.body[0].y == -1){
            GameOver();
        }
        if(snake.body[0].y==cellCount || snake.body[0].y== -1){
            GameOver();
        }
    }

    void CheckCollisionInTail(){
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if(ElementInDeque(snake.body[0],headlessBody)){
            GameOver();
        }
    }

    void GameOver(){
        snake.Reset();
        food.position = food.GenerateRandomspown(snake.body);
        running =false;
        score=0;
        PlaySound(wallSound);
    }

};

int main () {

    cout<< "Start game..."<<endl;
    InitWindow(2*offset + cellSize * cellCount,2*offset +cellSize*cellCount,"Snake Slash");
    SetTargetFPS(60);

        Game game = Game();
    
    while(WindowShouldClose()==false)
    {
        BeginDrawing();

        if(eventTriggered(0.2)){
        
        game.Update();
            
        }

        if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1){
            game.snake.direction={0,-1};
            game.running=true;
        }
        if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y !=-1){

            game.snake.direction={0,1};
            game.running=true;
        }
        if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1){
            game.snake.direction = {-1,0};
            game.running=true;
        }
        if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1){
            game.snake.direction = {1,0};
            game.running=true;
        }


        //Drowing
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offset-5,(float)offset-5,(float)cellSize*cellCount+10,(float)cellSize*cellCount+10} ,5, darkGreen);
        DrawText("Snake Slash",offset-5,20,40,darkGreen);
        DrawText(TextFormat("Score: %i",game.score),offset-5,offset+3+cellSize*cellCount,40,darkGreen);


        game.Draw();

        EndDrawing();
    }






    CloseWindow();
    return 0;
}