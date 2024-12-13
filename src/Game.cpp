/**
 * @file Game.cpp
 * @brief Implementation of the Game class and related helper functions for the MultiLudo board game
 */

#include "../include/Game.h"
#include "../include/Utils.h"
#include <iostream>
#include <string>
#include <cmath>

// Mutex locks for thread synchronization
extern pthread_mutex_t mutex;        // General game state mutex
extern pthread_mutex_t mutexDice;    // Dice rolling mutex  
extern pthread_mutex_t mutexTurn;    // Turn management mutex

/**
 * @brief Draws text centered horizontally on the screen
 * @param text The text to draw
 * @param y The vertical position
 * @param fontSize Size of the font
 * @param color Color of the text
 */
void DrawCenteredText(const char* text, int y, int fontSize, Color color) {
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, (1200 - textWidth) / 2, y, fontSize, color);
}

/**
 * @brief Draws text with a gradient color effect
 * @param text The text to draw
 * @param x The horizontal position
 * @param y The vertical position 
 * @param fontSize Size of the font
 * @param color1 Starting color of the gradient
 * @param color2 Ending color of the gradient
 */
void DrawGradientText(const char* text, int x, int y, int fontSize, Color color1, Color color2) {
    int letterSpacing = fontSize / 2;
    int currentX = x;
    float blendFactor = 0.0f;
    
    for (int i = 0; text[i] != '\0'; i++) {
        char letter[2] = {text[i], '\0'};
        // Calculate blended color for each letter
        Color blendedColor = {
            (unsigned char)((1 - blendFactor) * color1.r + blendFactor * color2.r),
            (unsigned char)((1 - blendFactor) * color1.g + blendFactor * color2.g),
            (unsigned char)((1 - blendFactor) * color1.b + blendFactor * color2.b),
            255
        };
        DrawText(letter, currentX, y, fontSize, blendedColor);
        currentX += MeasureText(letter, fontSize) + letterSpacing;
        blendFactor += 0.2f;
        if (blendFactor > 1.0f) blendFactor = 0.0f;
    }
}

/**
 * @brief Thread function for handling player actions
 * @param args Pointer to Player object
 * @return NULL
 */
void* playerThread(void* args) {
    Player* p = (Player*)args;
    while (!WindowShouldClose()) {
        pthread_mutex_lock(&mutex);
        p->rollDice();
        p->move();
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

/**
 * @brief Constructor for Game class
 * Initializes game state variables
 */
Game::Game() : screen(1), Initial(true), FinishedThreads(4, false), WinnerScreen(false) {}

/**
 * @brief Destructor for Game class
 * Cleans up resources and closes the window
 */
Game::~Game() {
    UnloadTexture(LudoBoard);
    for (int i = 0; i < 6; i++) {
        UnloadTexture(Dice[i]);
    }
    UnloadFont(gameFont);
    CloseWindow();
}

/**
 * @brief Loads the custom game font
 * Falls back to default font if custom font fails to load
 */
void Game::LoadGameFont() {
    gameFont = LoadFont("assets/Roboto-Bold.ttf");
    if (gameFont.texture.id == 0) {
        std::cout << "Failed to load font! Using default font instead." << std::endl;
        gameFont = GetFontDefault();
    }
}

/**
 * @brief Initializes the game window and core components
 */
void Game::Initialize() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "MultiLudo - A Multithreaded Board Game");
    SetTargetFPS(60);
    LoadGameFont();
    LoadTextures();
}

/**
 * @brief Loads all game textures from assets
 */
void Game::LoadTextures() {
    LudoBoard = LoadTexture("assets/board1.png");
    for (int i = 0; i < 6; i++) {
        std::string path = "assets/" + std::to_string(i + 1) + "-dice.png";
        Dice[i] = LoadTexture(path.c_str());
    }
}

/**
 * @brief Initializes players and creates player threads
 * Sets up player tokens, colors and starts the game threads
 */
void Game::InitializePlayers() {
    if (Initial && numTokens > 0) {
        // Load player token textures
        Texture2D red = LoadTexture("assets/red-goti.png");
        Texture2D green = LoadTexture("assets/green-goti.png");
        Texture2D blue = LoadTexture("assets/blue-goti.png");
        Texture2D yellow = LoadTexture("assets/yellow-goti.png");

        // Initialize players with their colors and tokens
        P1.setPlayer(0, RED, red);
        P2.setPlayer(1, GREEN, green);
        P3.setPlayer(2, YELLOW, yellow);
        P4.setPlayer(3, BLUE, blue);

        // Create threads for each player
        pthread_create(&th[0], NULL, &playerThread, &P1);
        pthread_create(&th[1], NULL, &playerThread, &P2);
        pthread_create(&th[2], NULL, &playerThread, &P3);
        pthread_create(&th[3], NULL, &playerThread, &P4);

        // Set up initial turn order
        GenerateTurns();
        turn = nextTurn[nextTurn.size() - 1];
        nextTurn.pop_back();
        
        Initial = false;
    }
}

/**
 * @brief Draws text using the custom font
 * Falls back to default DrawText if custom font isn't loaded
 */
void Game::DrawTextEx(const char* text, int x, int y, int fontSize, Color color) {
    if (gameFont.texture.id != 0) {
        ::DrawTextEx(gameFont, text, (Vector2){(float)x, (float)y}, fontSize, 1, color);
    } else {
        DrawText(text, x, y, fontSize, color);
    }
}

/**
 * @brief Draws text centered horizontally using custom font
 * @param game Pointer to Game instance
 * @param text Text to draw
 * @param y Vertical position
 * @param fontSize Font size
 * @param color Text color
 */
void DrawCenteredTextEx(Game* game, const char* text, int y, int fontSize, Color color) {
    Vector2 textSize = MeasureTextEx(game->gameFont, text, fontSize, 1);
    game->DrawTextEx(text, (Game::SCREEN_WIDTH - textSize.x) / 2, y, fontSize, color);
}

/**
 * @brief Draws the game scoreboard
 * Displays player scores, current turn, and dice values
 * @param p1 Score for player 1
 * @param p2 Score for player 2
 * @param p3 Score for player 3
 * @param p4 Score for player 4
 */
void Game::DrawScore(int p1, int p2, int p3, int p4) {
    // Draw scoreboard background
    DrawRectangle(900, 0, 300, SCREEN_HEIGHT, RAYWHITE);
    DrawRectangleLinesEx((Rectangle){900, 0, 300, (float)SCREEN_HEIGHT}, 2, LIGHTGRAY);

    // Draw scoreboard title
    DrawTextEx("SCOREBOARD", 950, 20, 30, DARKGRAY);
    DrawLine(920, 60, 1180, 60, LIGHTGRAY);

    // Draw player scores with colored rectangles
    const char* playerNames[] = {"RED", "GREEN", "YELLOW", "BLUE"};
    int scores[] = {p1, p2, p3, p4};
    Color colors[] = {RED, GREEN, YELLOW, BLUE};
    
    for (int i = 0; i < 4; i++) {
        float yPos = 80 + i * 50;
        DrawRectangle(920, (int)yPos, 240, 35, Fade(colors[i], 0.2f));
        DrawRectangleLinesEx((Rectangle){920, yPos, 240, 35}, 1, colors[i]);
        DrawTextEx(playerNames[i], 930, (int)yPos + 8, 20, colors[i]);
        DrawTextEx(TextFormat("%d", scores[i]), 1120, (int)yPos + 8, 20, DARKGRAY);
    }

    // Draw current turn indicator
    DrawTextEx("CURRENT TURN", 950, 290, 25, DARKGRAY);
    DrawLine(920, 325, 1180, 325, LIGHTGRAY);
    
    Color turnColor = colors[turn - 1];
    DrawRectangle(920, 335, 240, 45, Fade(turnColor, 0.3f));
    DrawTextEx(playerNames[turn - 1], 950, 345, 30, turnColor);

    // Draw dice values section
    DrawTextEx("DICE VALUES", 950, 400, 25, DARKGRAY);
    DrawLine(920, 435, 1180, 435, LIGHTGRAY);
    
    // Draw dice values in a more compact way
    int diceValueY = 445;
    for (unsigned int i = 0; i < diceVal.size(); i++) {
        if (diceVal[i] != 0) {
            DrawRectangle(930 + (i * 60), diceValueY, 50, 50, LIGHTGRAY);
            DrawTextEx(TextFormat("%d", diceVal[i]), 947 + (i * 60), diceValueY + 15, 25, DARKGRAY);
        }
    }

    // Draw instructions
    DrawRectangle(920, 620, 260, 70, Fade(LIGHTGRAY, 0.3f));
    DrawRectangleLinesEx((Rectangle){920, 620, 260, 70}, 1, DARKGRAY);
    DrawTextEx("INSTRUCTIONS", 950, 625, 20, DARKGRAY);
    DrawTextEx("Click dice to roll", 930, 650, 18, DARKGRAY);
    DrawTextEx("Click token to move", 930, 670, 18, DARKGRAY);
}

/**
 * @brief Draws the game start screen
 * Handles token selection and game start
 */
void Game::DrawStartScreen() {
    // Draw background
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
    
    // Draw decorative corners with Ludo colors
    int cornerSize = 200;
    DrawRectangle(0, 0, cornerSize, cornerSize, Fade(RED, 0.2f));
    DrawRectangle(SCREEN_WIDTH - cornerSize, 0, cornerSize, cornerSize, Fade(GREEN, 0.2f));
    DrawRectangle(0, SCREEN_HEIGHT - cornerSize, cornerSize, cornerSize, Fade(BLUE, 0.2f));
    DrawRectangle(SCREEN_WIDTH - cornerSize, SCREEN_HEIGHT - cornerSize, cornerSize, cornerSize, Fade(YELLOW, 0.2f));

    // Draw title with Ludo colors
    const char* titleText[] = {"M", "U", "L", "T", "I"};
    Color titleColors[] = {RED, GREEN, YELLOW, BLUE, RED};
    int titleX = 350;
    int spacing = 120;
    
    for (int i = 0; i < 5; i++) {
        Color shadowColor = Fade(titleColors[i], 0.3f);
        // Draw shadow
        DrawTextEx(titleText[i], titleX + i * spacing + 4, 54, 120, shadowColor);
        // Draw main text
        DrawTextEx(titleText[i], titleX + i * spacing, 50, 120, titleColors[i]);
    }

    // Draw "LUDO" with gradient background
    Rectangle ludoBox = {350, 170, 500, 120};
    DrawRectangleGradientH(ludoBox.x, ludoBox.y, ludoBox.width, ludoBox.height, 
                          Fade(BLUE, 0.2f), Fade(RED, 0.2f));
    DrawRectangleLinesEx(ludoBox, 3, DARKGRAY);
    Vector2 ludoSize = MeasureTextEx(gameFont, "LUDO", 120, 1);
    DrawTextEx("LUDO", ludoBox.x + (ludoBox.width - ludoSize.x)/2, ludoBox.y + 10, 120, DARKGRAY);

    // Draw subtitle with decorative line
    DrawLineEx((Vector2){300, 320}, (Vector2){900, 320}, 2, LIGHTGRAY);
    DrawTextEx("A Multithreaded Board Game", 400, 330, 30, DARKGRAY);
    DrawLineEx((Vector2){300, 370}, (Vector2){900, 370}, 2, LIGHTGRAY);

    // Draw token selection area with better styling
    Rectangle tokenBox = {350, 400, 500, 150};
    DrawRectangle(tokenBox.x, tokenBox.y, tokenBox.width, tokenBox.height, Fade(LIGHTGRAY, 0.3f));
    DrawRectangleLinesEx(tokenBox, 2, DARKGRAY);
    
    DrawTextEx("Select Number of Tokens", tokenBox.x + 100, tokenBox.y + 20, 35, DARKGRAY);
    
    // Draw token number buttons
    if (numTokens != -1) {
        DrawCircle(600, tokenBox.y + 100, 40, Fade(DARKGRAY, 0.2f));
        DrawTextEx(TextFormat("%d", numTokens), 590, tokenBox.y + 80, 50, MAROON);
    }

    // Draw token selection hint
    DrawTextEx("Press 1-4 to select", tokenBox.x + 150, tokenBox.y + 120, 25, GRAY);

    // Handle token selection
    if (IsKeyPressed('1')) numTokens = 1;
    if (IsKeyPressed('2')) numTokens = 2;
    if (IsKeyPressed('3')) numTokens = 3;
    if (IsKeyPressed('4')) numTokens = 4;

    // Draw start button with animation
    Rectangle startBtn = {450, 600, 300, 80};
    Color btnColor = CheckCollisionPointRec(GetMousePosition(), startBtn) ? 
                    Fade(GREEN, 0.7f) : Fade(GREEN, 0.5f);
    
    DrawRectangle(startBtn.x, startBtn.y, startBtn.width, startBtn.height, btnColor);
    DrawRectangleLinesEx(startBtn, 2, DARKGREEN);
    
    Vector2 startSize = MeasureTextEx(gameFont, "START", 40, 1);
    DrawTextEx("START", 
              startBtn.x + (startBtn.width - startSize.x)/2, 
              startBtn.y + (startBtn.height - startSize.y)/2, 
              40, WHITE);

    // Draw credits with style
    DrawRectangle(0, SCREEN_HEIGHT - 40, SCREEN_WIDTH, 40, Fade(DARKGRAY, 0.1f));
    DrawTextEx("Created by:", 20, SCREEN_HEIGHT - 30, 20, DARKGRAY);
    DrawTextEx("Amna , Shuja ,Samra", 120, SCREEN_HEIGHT - 30, 20, MAROON);

    // Handle start button click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), startBtn)) {
            if (numTokens <= 4 && numTokens >= 1) {
                screen = 2;
                LudoGrid = new std::tuple<int, int, int>*[4];
                for (int i = 0; i < 4; i++) {
                    LudoGrid[i] = new std::tuple<int, int, int>[numTokens];
                }
                InitializePlayers();
            }
        }
    }
}

/**
 * @brief Draws the game win screen
 * Displays winners and their rankings
 */
void Game::DrawWinScreen() {
    // Draw background
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
    
    // Draw decorative corners with Ludo colors
    int cornerSize = 200;
    DrawRectangle(0, 0, cornerSize, cornerSize, Fade(GOLD, 0.2f));
    DrawRectangle(SCREEN_WIDTH - cornerSize, 0, cornerSize, cornerSize, Fade(GOLD, 0.2f));
    DrawRectangle(0, SCREEN_HEIGHT - cornerSize, cornerSize, cornerSize, Fade(GOLD, 0.2f));
    DrawRectangle(SCREEN_WIDTH - cornerSize, SCREEN_HEIGHT - cornerSize, cornerSize, cornerSize, Fade(GOLD, 0.2f));

    // Draw title with gradient effect
    Rectangle titleBox = {300, 30, 600, 120};
    DrawRectangleGradientH(titleBox.x, titleBox.y, titleBox.width, titleBox.height, 
                          Fade(GOLD, 0.3f), Fade(ORANGE, 0.3f));
    DrawRectangleLinesEx(titleBox, 3, GOLD);

    // Animate title color
    float time = GetTime() * 2;
    Color titleColor = {
        (unsigned char)(255 * (0.7f + 0.3f * sin(time))),
        (unsigned char)(215 * (0.7f + 0.3f * sin(time))),
        0,
        255
    };
    
    Vector2 gameOverSize = MeasureTextEx(gameFont, "GAME OVER!", 100, 1);
    DrawTextEx("GAME OVER!", 
              titleBox.x + (titleBox.width - gameOverSize.x)/2, 
              titleBox.y + (titleBox.height - gameOverSize.y)/2, 
              100, titleColor);

    // Draw "WINNERS" text with decorative lines
    DrawLineEx((Vector2){300, 180}, (Vector2){900, 180}, 3, GOLD);
    Vector2 winnersSize = MeasureTextEx(gameFont, "WINNERS", 60, 1);
    DrawTextEx("WINNERS", (SCREEN_WIDTH - winnersSize.x)/2, 200, 60, DARKGRAY);
    DrawLineEx((Vector2){300, 280}, (Vector2){900, 280}, 3, GOLD);

    // Draw winners list with enhanced styling
    const char* playerNames[] = {"RED", "GREEN", "YELLOW", "BLUE"};
    Color playerColors[] = {RED, GREEN, YELLOW, BLUE};
    const char* trophies[] = {"🏆", "🥈", "🥉", "4th"};
    
    for (int i = 0; i < winners.size() && i < 4; i++) {
        float yPos = 320 + (i * 120);
        float time = GetTime() * 2;
        float scale = 1.0f + 0.1f * sin(time + i);
        int index = winners[i] - 1;
        Color color = playerColors[index];
        const char* name = playerNames[index];
        
        // Draw winner box
        Rectangle winnerBox = {300, yPos, 600, 100};
        DrawRectangle(winnerBox.x, winnerBox.y, winnerBox.width, winnerBox.height, 
                     Fade(color, 0.1f));
        DrawRectangleLinesEx(winnerBox, 2, Fade(color, 0.5f));
        
        // Draw position circle
        DrawCircle(350, yPos + 50, 35 * scale, Fade(color, 0.2f));
        DrawCircleLines(350, yPos + 50, 35 * scale, color);
        
        // Draw position number and trophy
        if (i == 0) {
            DrawTextEx(trophies[i], 330, yPos + 30, 40, GOLD);
        } else {
            DrawTextEx(trophies[i], 335, yPos + 30, 40, color);
        }
        
        // Draw player name with trophy
        DrawTextEx(name, 450, yPos + 35, 50, color);
        
        // Draw score or other stats if needed
        DrawTextEx("Winner!", 700, yPos + 40, 30, Fade(color, 0.7f));
    }

    // Draw return to menu hint with blinking effect
    float blinkTime = sin(GetTime() * 3) * 0.5f + 0.5f;
    DrawTextEx("Press SPACE to return to menu", 
              450, SCREEN_HEIGHT - 50, 
              25, Fade(DARKGRAY, blinkTime));
}

/**
 * @brief Draws the current dice face
 */
void Game::DrawDice() {
    DrawTexture(Dice[dice - 1], 990, 500, WHITE);
}

/**
 * @brief Updates the game state
 * Handles player turns, movement, and game completion
 */
void Game::Update() {
    if (screen == 2) {
        DrawTexture(LudoBoard, 0, 0, WHITE);
        DrawScore(P1.score, P2.score, P3.score, P4.score);
        DrawDice();

        int count = 0;
        int index = 0;

        // Handle player 1
        if (!P1.completed) P1.Start();
        else if (!FinishedThreads[0]) {
            pthread_cancel(th[0]);
            diceVal.resize(3);
            std::fill(diceVal.begin(), diceVal.end(), 0);
            turn = getTurn();
            movePlayer = false;
            FinishedThreads[0] = true;
        }

        // Handle player 2
        if (!P2.completed) P2.Start();
        else if (!FinishedThreads[1]) {
            pthread_cancel(th[1]);
            turn = getTurn();
            movePlayer = false;
            diceVal.resize(3);
            std::fill(diceVal.begin(), diceVal.end(), 0);
            FinishedThreads[1] = true;
        }

        // Handle player 3
        if (!P3.completed) P3.Start();
        else if (!FinishedThreads[2]) {
            pthread_cancel(th[2]);
            turn = getTurn();
            movePlayer = false;
            diceVal.resize(3);
            std::fill(diceVal.begin(), diceVal.end(), 0);
            FinishedThreads[2] = true;
        }

        // Handle player 4
        if (!P4.completed) P4.Start();
        else if (!FinishedThreads[3]) {
            pthread_cancel(th[3]);
            diceVal.resize(3);
            std::fill(diceVal.begin(), diceVal.end(), 0);
            turn = getTurn();
            movePlayer = false;
            FinishedThreads[3] = true;
        }

        // Check for game completion
        for (int g = 0; g < 4; g++) {
            if (FinishedThreads[g] == true)
                count++;
            else
                index = g;
        }

        if (count >= 3) {
            winners.push_back(index + 1);
            screen = 3;
        }
    }
}

/**
 * @brief Main game loop
 * Handles drawing and updating game state
 */
void Game::Run() {
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (screen == 1) {
            DrawStartScreen();
        }
        else if (screen == 2) {
            Update();
        }
        else {
            DrawWinScreen();
        }

        EndDrawing();
    }

    // Clean up threads
    pthread_join(th[0], NULL);
    pthread_join(th[1], NULL);
    pthread_join(th[2], NULL);
    pthread_join(th[3], NULL);
} 