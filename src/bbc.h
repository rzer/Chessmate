//Вернёт нам move который нужно передать в ai_makeMove или 0 если ход невозможен
int ai_isLegalMove(int source_square, int target_square, char promotedPiece);
void ai_makeMove(int move);

void ai_loadFen(char* command);
void ai_findBestMove(int depth);

