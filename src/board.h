
typedef struct Piece Piece;
typedef enum PieceType PieceType;

void showBoard(void);
void hideBoard(void);
void swapBoard(void);
void menuBoard(void);
void initBoard(void);
void newGame(void);
void loadFen(const char* fen);
Piece* getPieceAtPosition(int position);

