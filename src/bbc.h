//Вернёт нам move который нужно передать в ai_makeMove или 0 если ход невозможен
int ai_isLegalMove(int source_square, int target_square, char promotedPiece);
void ai_makeMove(int move);
void ai_loadFen(char* command);
int ai_findBestMove(int depth);
void ai_init(void);
int ai_getSourceIndex(int move);
int ai_getTargetIndex(int move);
int ai_getPromoted(int move);
bool ai_isCapture(int move);
bool ai_isEnpassant(int move);
bool ai_isCastling(int move);
bool ai_isWhiteMove(void);

