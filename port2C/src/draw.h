#ifndef DRAW_H
#define DRAW_H
    void IncrmentGameClip(GameState* game);
    void resetGameClip(GameState* game);
    void setClip(GameState* game, int x, int y, int w, int h);
    SDL_Texture* drawNewTexture(GameState* game);
    void drawSetTarget(GameState* game, SDL_Texture* target);
    void drawResetTarget(GameState* game);
    void drawImage(GameState* game, SDL_Texture* tex, int x, int y, int w, int h);
    void drawImageXY(GameState* game, SDL_Texture* tex, int x, int y);
    void drawSpriteSimple(GameState* game, int objNum);
    void drawSprite(GameState* game, int objNum);
    void addBlock(GameState* game, int pos, int look);
    void removeBlock(GameState* game, int mapPos);
    void drawBlockSimple(GameState* game, int look, int x, int y);
    int drawToBlack(GameState* game);
    void drawToPlayField(GameState* game);
    void drawShadowSimple(GameState* game, int objNum);
    void drawShadow(GameState* game, int objNum);
    SDL_Rect vPrint(GameState* game, int x, int y, SDL_Color color, const char* text);
    SDL_Rect vPrintCenter(GameState* game, int x, int y, SDL_Color color, const char* text);
    SDL_Rect GetPrintSize(GameState* game, const char* text);
    void drawGreenBackgound(GameState* game);
#endif