// - **Shuja Uddin** (22i-2553) | SE-D | FAST NUCES Islamabad
// - **Amna Hassan** (22i-8759) | SE-D | FAST NUCES Islamabad  
// - **Samra Saleem** (22i-2727) | SE-D | FAST NUCES Islamabad




/**
 * @file Game.cpp
 * @brief Implementation of the Game class and related helper functions for the MultiLudo game
 */

#include "../include/Game.h"
#include "../include/Utils.h"
#include <iostream>
#include <string>
#include <cmath>

// Mutex locks for thread synchronization
extern pthread_mutex_t gameStateMutex;
extern pthread_mutex_t diceRollMutex;
extern pthread_mutex_t turnControlMutex;

/**
 * @brief Draws text centered horizontally on the screen
 * @param text The text to draw
 * @param y The vertical position
 * @param fontSize The font size to use
 * @param color The color to draw the text in
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
 * @param fontSize The font size to use
 * @param color1 The starting color of the gradient
 * @param color2 The ending color of the gradient
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
 * @brief Thread function for handling player moves
 * @param args Pointer to the Player object
 * @return NULL
 */
void* playerThread(void* args) {
    Player* p = (Player*)args;
    while (!WindowShouldClose()) {
        pthread_mutex_lock(&gameStateMutex);
        p->rollDice();
        p->move();
        pthread_mutex_unlock(&gameStateMutex);
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
 * Cleans up textures and closes window
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
 * @brief Initializes the game window and resources
 */
void Game::Initialize() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "MultiLudo - A Multithreaded Board Game");
    SetTargetFPS(60);
    LoadGameFont();
    LoadTextures();
}

/**
 * @brief Loads all game textures from files
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
 * Sets up player tokens, colors and starts game threads
 */
void Game::InitializePlayers() {
    if (Initial && numTokens > 0) {
        // Load player token textures
        Texture2D red = LoadTexture("assets/red-goti.png");
        Texture2D green = LoadTexture("assets/green-goti.png");
        Texture2D blue = LoadTexture("assets/blue-goti.png");
        Texture2D yellow = LoadTexture("assets/yellow-goti.png");

        // Initialize players with colors and tokens
        P1.setPlayer(0, RED, red);
        P2.setPlayer(1, GREEN, green);
        P3.setPlayer(2, YELLOW, yellow);
        P4.setPlayer(3, BLUE, blue);

        // Create player threads
        pthread_create(&th[0], NULL, &playerThread, &P1);
        pthread_create(&th[1], NULL, &playerThread, &P2);
        pthread_create(&th[2], NULL, &playerThread, &P3);
        pthread_create(&th[3], NULL, &playerThread, &P4);

        // Set up initial game state
        GeneratePlayerTurns();
        turn = nextTurn[nextTurn.size() - 1];
        nextTurn.pop_back();
        
        Initial = false;
    }
}

/**
 * @brief Draws text using the custom game font
 * Falls back to default font if custom font is unavailable
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
 */
void Game::DrawScore(int p1, int p2, int p3, int p4) {
    // Draw scoreboard panel with gradient background
    DrawRectangleGradientV(900, 0, 300, SCREEN_HEIGHT, RAYWHITE, Fade(LIGHTGRAY, 0.2f));
    DrawRectangleLinesEx((Rectangle){900, 0, 300, (float)SCREEN_HEIGHT}, 3, DARKGRAY);

    // Draw decorative header
    DrawRectangleGradientH(900, 0, 300, 70, Fade(BLUE, 0.2f), Fade(RED, 0.2f));
    DrawTextEx("SCOREBOARD", 950, 20, 35, DARKGRAY);
    DrawLine(920, 70, 1180, 70, DARKGRAY);

    // Draw player scores with enhanced styling
    const char* playerNames[] = {"RED", "GREEN", "YELLOW", "BLUE"};
    int scores[] = {p1, p2, p3, p4};
    Color colors[] = {RED, GREEN, YELLOW, BLUE};
    
    for (int i = 0; i < 4; i++) {
        float yPos = 90 + i * 60;
        // Draw score panel with gradient
        DrawRectangleGradientH(920, (int)yPos, 240, 45, Fade(colors[i], 0.1f), Fade(colors[i], 0.3f));
        DrawRectangleLinesEx((Rectangle){920, yPos, 240, 45}, 2, colors[i]);
        
        // Draw player name and score with shadow effect
        DrawTextEx(playerNames[i], 932, (int)yPos + 12, 24, Fade(BLACK, 0.2f));
        DrawTextEx(playerNames[i], 930, (int)yPos + 10, 24, colors[i]);
        
        DrawTextEx(TextFormat("%d", scores[i]), 1122, (int)yPos + 12, 24, Fade(BLACK, 0.2f));
        DrawTextEx(TextFormat("%d", scores[i]), 1120, (int)yPos + 10, 24, DARKGRAY);
    }

    // Draw current turn section with enhanced styling
    DrawRectangleGradientH(900, 280, 300, 120, Fade(LIGHTGRAY, 0.1f), Fade(LIGHTGRAY, 0.3f));
    //DrawTextEx("CURRENT TURN", 950, 290, 28, DARKGRAY);
    DrawLine(920, 325, 1180, 325, DARKGRAY);
    
    Color turnColor = colors[turn - 1];
    DrawRectangleGradientH(920, 335, 240, 50, Fade(turnColor, 0.2f), Fade(turnColor, 0.4f));
    DrawRectangleLinesEx((Rectangle){920, 335, 240, 50}, 2, turnColor);
    DrawTextEx(playerNames[turn - 1], 952, 347, 32, Fade(BLACK, 0.2f));
    DrawTextEx(playerNames[turn - 1], 950, 345, 32, turnColor);

    // Draw dice values section with enhanced styling
    DrawRectangleGradientH(900, 410, 300, 200, Fade(LIGHTGRAY, 0.1f), Fade(LIGHTGRAY, 0.3f));
    DrawTextEx("DICE VALUES", 950, 420, 28, DARKGRAY);
    DrawLine(920, 455, 1180, 455, DARKGRAY);
    
    // Draw dice values in a grid layout
    int diceValueY = 470;
    for (unsigned int i = 0; i < diceVal.size(); i++) {
        if (diceVal[i] != 0) {
            int x = 930 + ((i % 3) * 60); // Reduced spacing between dice
            int y = diceValueY + ((i / 3) * 45); // Reduced vertical spacing
            DrawRectangleGradientV(x, y, 45, 35, WHITE, Fade(LIGHTGRAY, 0.5f)); // Smaller rectangle
            DrawRectangleLinesEx((Rectangle){(float)x, (float)y, 45, 35}, 2, DARKGRAY); // Smaller border
            DrawTextEx(TextFormat("%d", diceVal[i]), x + 15, y + 8, 22, DARKGRAY); // Smaller text and adjusted position
        }
    }

    // Draw instructions panel with enhanced styling
    DrawRectangleGradientV(920, 620, 260, 80, Fade(LIGHTGRAY, 0.2f), Fade(LIGHTGRAY, 0.4f));
    DrawRectangleLinesEx((Rectangle){920, 620, 260, 80}, 2, DARKGRAY);
    DrawTextEx("INSTRUCTIONS", 950, 625, 22, DARKGRAY);
    DrawTextEx("• Click dice to roll", 930, 650, 20, DARKGRAY);
    DrawTextEx("• Click token to move", 930, 670, 20, DARKGRAY);
}

/**
 * @brief Draws the game start screen
 * Displays title, token selection, and start button
 */
void Game::DrawStartScreen() {
    // Draw enhanced gradient background with multiple layers
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE, Fade(LIGHTGRAY, 0.4f));
    DrawRectangleGradientH(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLUE, 0.05f), Fade(RED, 0.05f));
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(RED, 0.1f), Fade(LIGHTGRAY, 0.2f));
    // Draw decorative corners with Ludo colors and gradient effect
    int cornerSize = 250;
    int borderThickness = 4;
    
    // Top-left corner (Red)
    DrawRectangleGradientH(0, 0, cornerSize, cornerSize, RED, Fade(RED, 0.1f));
    DrawRectangleLinesEx((Rectangle){0, 0, cornerSize, cornerSize}, borderThickness, Fade(RED, 0.8f));
    
    // Top-right corner (Green) 
    DrawRectangleGradientH(SCREEN_WIDTH - cornerSize, 0, cornerSize, cornerSize, GREEN, Fade(GREEN, 0.1f));
    DrawRectangleLinesEx((Rectangle){SCREEN_WIDTH - cornerSize, 0, cornerSize, cornerSize}, borderThickness, Fade(GREEN, 0.8f));
    
    // Bottom-left corner (Blue)
    DrawRectangleGradientH(0, SCREEN_HEIGHT - cornerSize, cornerSize, cornerSize, BLUE, Fade(BLUE, 0.1f));
    DrawRectangleLinesEx((Rectangle){0, SCREEN_HEIGHT - cornerSize, cornerSize, cornerSize}, borderThickness, Fade(BLUE, 0.8f));
    
    // Bottom-right corner (Yellow)
    DrawRectangleGradientH(SCREEN_WIDTH - cornerSize, SCREEN_HEIGHT - cornerSize, cornerSize, cornerSize, YELLOW, Fade(YELLOW, 0.1f));
    DrawRectangleLinesEx((Rectangle){SCREEN_WIDTH - cornerSize, SCREEN_HEIGHT - cornerSize, cornerSize, cornerSize}, borderThickness, Fade(YELLOW, 0.8f));

    // Draw animated title with shadow and glow effect
    const char* titleText[] = {"M", "U", "L", "T", "I"};
    Color titleColors[] = {RED, GREEN, YELLOW, BLUE, RED};
    int titleX = 350;
    int spacing = 120;
    float time = GetTime() * 2;
    
    for (int i = 0; i < 5; i++) {
        Color glowColor = Fade(titleColors[i], 0.3f + 0.2f * sinf(time + i));
        // Draw glow effect
        DrawTextEx(titleText[i], titleX + i * spacing + 2, 52, 120, glowColor);
        DrawTextEx(titleText[i], titleX + i * spacing - 2, 48, 120, glowColor);
        // Draw main text with pulsing effect
        DrawTextEx(titleText[i], titleX + i * spacing, 50, 120, 
                  Fade(titleColors[i], 0.8f + 0.2f * sinf(time + i)));
    }

    // Draw "LUDO" with animated gradient background
    Rectangle ludoBox = {350, 170, 500, 120};
    DrawRectangleGradientH(ludoBox.x, ludoBox.y, ludoBox.width, ludoBox.height, 
                          Fade(BLUE, 0.2f + 0.1f * sinf(time)), 
                          Fade(RED, 0.2f + 0.1f * sinf(time)));
    DrawRectangleLinesEx(ludoBox, 4, Fade(DARKGRAY, 0.8f + 0.2f * sinf(time)));
    Vector2 ludoSize = MeasureTextEx(gameFont, "LUDO", 120, 1);
    DrawTextEx("LUDO", ludoBox.x + (ludoBox.width - ludoSize.x)/2, ludoBox.y + 10, 120, 
               Fade(DARKGRAY, 0.8f + 0.2f * sinf(time)));

    // Draw subtitle with animated decorative lines
    float lineWidth = 2 + sinf(time) * 1;
    DrawLineEx((Vector2){300, 320}, (Vector2){900, 320}, lineWidth, LIGHTGRAY);
    DrawTextEx("A Multithreaded Board Game", 400, 330, 30, 
               Fade(DARKGRAY, 0.8f + 0.2f * sinf(time)));
    DrawLineEx((Vector2){300, 370}, (Vector2){900, 370}, lineWidth, LIGHTGRAY);

    // Draw token selection area with glowing effect
    Rectangle tokenBox = {350, 400, 500, 150};
    DrawRectangleGradientV(tokenBox.x, tokenBox.y, tokenBox.width, tokenBox.height,
                          Fade(LIGHTGRAY, 0.4f + 0.1f * sinf(time)),
                          Fade(WHITE, 0.4f + 0.1f * sinf(time)));
    DrawRectangleLinesEx(tokenBox, 3, Fade(DARKGRAY, 0.8f + 0.2f * sinf(time)));
    
    DrawTextEx("Select Number of Players", tokenBox.x + 80, tokenBox.y + 20, 35, 
               Fade(DARKGRAY, 0.8f + 0.2f * sinf(time)));
    
    // Draw animated token number display
    if (numTokens != -1) {
        DrawCircleGradient(600, tokenBox.y + 100, 45,
                          Fade(DARKGRAY, 0.3f + 0.1f * sinf(time)),
                          Fade(WHITE, 0.3f + 0.1f * sinf(time)));
        DrawTextEx(TextFormat("%d", numTokens), 590, tokenBox.y + 80, 50, 
                  Fade(MAROON, 0.8f + 0.2f * sinf(time)));
    }

    DrawTextEx("Press 1-4 to select players", tokenBox.x + 120, tokenBox.y + 120, 25,
               Fade(GRAY, 0.8f + 0.2f * sinf(time)));

    // Handle token selection
    if (IsKeyPressed('1')) numTokens = 1;
    if (IsKeyPressed('2')) numTokens = 2;
    if (IsKeyPressed('3')) numTokens = 3;
    if (IsKeyPressed('4')) numTokens = 4;

    // Draw animated start button
    Rectangle startBtn = {450, 600, 300, 80};
    Color btnColor = CheckCollisionPointRec(GetMousePosition(), startBtn) ? 
                    Fade(GREEN, 0.8f + 0.2f * sinf(time)) : 
                    Fade(GREEN, 0.6f + 0.2f * sinf(time));
    
    DrawRectangleGradientH(startBtn.x, startBtn.y, startBtn.width, startBtn.height,
                          btnColor, Fade(DARKGREEN, 0.7f + 0.2f * sinf(time)));
    DrawRectangleLinesEx(startBtn, 3, DARKGREEN);
    
    Vector2 startSize = MeasureTextEx(gameFont, "START", 40, 1);
    DrawTextEx("START", 
              startBtn.x + (startBtn.width - startSize.x)/2, 
              startBtn.y + (startBtn.height - startSize.y)/2, 
              40, Fade(WHITE, 0.9f + 0.1f * sinf(time)));

    // Draw credits with gradient effect
    DrawRectangleGradientH(0, SCREEN_HEIGHT - 40, SCREEN_WIDTH, 40, 
                          Fade(DARKGRAY, 0.2f), Fade(DARKGRAY, 0.1f));
    DrawTextEx("Created by:", 20, SCREEN_HEIGHT - 30, 20, DARKGRAY);
    DrawTextEx("Shuja, Amna & Samra", 120, SCREEN_HEIGHT - 30, 20, 
               Fade(MAROON, 0.8f + 0.2f * sinf(time)));

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
 * Displays winners and rankings with animations
 */
void Game::DrawWinScreen() {
    // Draw background with a subtle pattern
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
    for (int i = 0; i < SCREEN_WIDTH; i += 40) {
        for (int j = 0; j < SCREEN_HEIGHT; j += 40) {
            DrawRectangle(i, j, 20, 20, Fade(GOLD, 0.05f));
        }
    }
    
    // Draw decorative corners with gradient effect
    int cornerSize = 200;
    DrawRectangleGradientH(0, 0, cornerSize, cornerSize, Fade(GOLD, 0.3f), Fade(ORANGE, 0.2f));
    DrawRectangleGradientH(SCREEN_WIDTH - cornerSize, 0, cornerSize, cornerSize, Fade(ORANGE, 0.2f), Fade(GOLD, 0.3f));
    DrawRectangleGradientV(0, SCREEN_HEIGHT - cornerSize, cornerSize, cornerSize, Fade(GOLD, 0.3f), Fade(ORANGE, 0.2f));
    DrawRectangleGradientV(SCREEN_WIDTH - cornerSize, SCREEN_HEIGHT - cornerSize, cornerSize, cornerSize, Fade(ORANGE, 0.2f), Fade(GOLD, 0.3f));

    // Draw title with enhanced styling
    Rectangle titleBox = {300, 30, 600, 120};
    DrawRectangleGradientH(titleBox.x, titleBox.y, titleBox.width, titleBox.height, 
                          Fade(GOLD, 0.4f), Fade(ORANGE, 0.4f));
    DrawRectangleLinesEx(titleBox, 4, GOLD);
    DrawRectangleLinesEx({titleBox.x + 4, titleBox.y + 4, titleBox.width - 8, titleBox.height - 8}, 2, WHITE);
    
    Vector2 gameOverSize = MeasureTextEx(gameFont, "GAME OVER!", 100, 1);
    DrawTextEx("GAME OVER!", 
              titleBox.x + (titleBox.width - gameOverSize.x)/2, 
              titleBox.y + (titleBox.height - gameOverSize.y)/2, 
              100, GOLD);

    // Draw "WINNERS" section with enhanced decoration
    DrawLineEx((Vector2){250, 180}, (Vector2){950, 180}, 5, GOLD);
    DrawLineEx((Vector2){250, 182}, (Vector2){950, 182}, 2, WHITE);
    Vector2 winnersSize = MeasureTextEx(gameFont, "WINNERS", 60, 1);
    DrawTextEx("WINNERS", (SCREEN_WIDTH - winnersSize.x)/2, 200, 60, GOLD);
    DrawLineEx((Vector2){250, 280}, (Vector2){950, 280}, 5, GOLD);
    DrawLineEx((Vector2){250, 282}, (Vector2){950, 282}, 2, WHITE);

    // Draw winners list with enhanced visual hierarchy
    const char* playerNames[] = {"RED", "GREEN", "YELLOW", "BLUE"};
    Color playerColors[] = {RED, GREEN, YELLOW, BLUE};
    const char* trophies[] = {"🏆", "🥈", "🥉", "4th"};
    
    for (int i = 0; i < winners.size() && i < 4; i++) {
        float yPos = 320 + (i * 120);
        int index = winners[i] - 1;
        Color color = playerColors[index];
        const char* name = playerNames[index];
        
        // Draw winner box with double border effect
        Rectangle winnerBox = {300, yPos, 600, 100};
        DrawRectangleGradientH(winnerBox.x, winnerBox.y, winnerBox.width, winnerBox.height, 
                             Fade(color, 0.15f), Fade(color, 0.05f));
        DrawRectangleLinesEx(winnerBox, 3, Fade(color, 0.7f));
        DrawRectangleLinesEx({winnerBox.x + 3, winnerBox.y + 3, winnerBox.width - 6, winnerBox.height - 6}, 
                            1, Fade(WHITE, 0.5f));
        
        // Draw position indicator with enhanced styling
        DrawCircle(350, yPos + 50, 35, Fade(color, 0.3f));
        DrawCircleLines(350, yPos + 50, 35, color);
        DrawCircleLines(350, yPos + 50, 33, Fade(WHITE, 0.5f));
        
        // Draw position number and trophy with shadow effect
        if (i == 0) {
            DrawTextEx(trophies[i], 332, yPos + 32, 40, Fade(BLACK, 0.2f));
            DrawTextEx(trophies[i], 330, yPos + 30, 40, GOLD);
        } else {
            DrawTextEx(trophies[i], 337, yPos + 32, 40, Fade(BLACK, 0.2f));
            DrawTextEx(trophies[i], 335, yPos + 30, 40, color);
        }
        
        // Draw player name with shadow effect
        DrawTextEx(name, 452, yPos + 37, 50, Fade(BLACK, 0.2f));
        DrawTextEx(name, 450, yPos + 35, 50, color);
        
        // Draw winner badge
        if (i == 0) {
            DrawTextEx("CHAMPION!", 700, yPos + 40, 30, GOLD);
        } else {
            DrawTextEx("FINALIST", 700, yPos + 40, 30, Fade(color, 0.7f));
        }
    }

    // Draw return instruction with box
    Rectangle instructBox = {400, SCREEN_HEIGHT - 70, 400, 40};
    DrawRectangleGradientH(instructBox.x, instructBox.y, instructBox.width, instructBox.height,
                          Fade(DARKGRAY, 0.1f), Fade(DARKGRAY, 0.2f));
    DrawRectangleLinesEx(instructBox, 2, Fade(DARKGRAY, 0.3f));
    DrawTextEx("Press SPACE to return to menu", 
              450, SCREEN_HEIGHT - 60, 
              25, DARKGRAY);
}

/**
 * @brief Draws the current dice value
 */
void Game::DrawDice() {
    DrawTexture(Dice[dice - 1], 990, 500, WHITE);
}

/**
 * @brief Updates the game state
 * Handles player turns, completion checks, and game over conditions
 */
void Game::Update() {
    if (screen == 2) {
        DrawTexture(LudoBoard, 0, 0, WHITE);
        DrawScore(P1.score, P2.score, P3.score, P4.score);
        DrawDice();

        int count = 0;
        int index = 0;

        // Check and handle player completion states
        if (!P1.completed) P1.Start();
        else if (!FinishedThreads[0]) {
            pthread_cancel(th[0]);
            diceVal.resize(3);
            std::fill(diceVal.begin(), diceVal.end(), 0);
            turn = getTurn();
            movePlayer = false;
            FinishedThreads[0] = true;
        }

        if (!P2.completed) P2.Start();
        else if (!FinishedThreads[1]) {
            pthread_cancel(th[1]);
            turn = getTurn();
            movePlayer = false;
            diceVal.resize(3);
            std::fill(diceVal.begin(), diceVal.end(), 0);
            FinishedThreads[1] = true;
        }

        if (!P3.completed) P3.Start();
        else if (!FinishedThreads[2]) {
            pthread_cancel(th[2]);
            turn = getTurn();
            movePlayer = false;
            diceVal.resize(3);
            std::fill(diceVal.begin(), diceVal.end(), 0);
            FinishedThreads[2] = true;
        }

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