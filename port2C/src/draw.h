#ifndef DRAW_H
#define DRAW_H
    void drawSpriteSimple(GameState* game, int objNum);
    void drawSprite(GameState* game, int objNum);
    void addBlock(GameState* game, int pos, int look);
    void removeBlock(GameState* game, int mapPos);
    void drawBlockSimple(GameState* game, int look, int x, int y);
    SDL_Texture* drawNewTexture(GameState* game);
    void drawSetTarget(GameState* game, SDL_Texture* target);
    void drawResetTarget(GameState* game);
#endif