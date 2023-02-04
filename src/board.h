
typedef struct Piece Piece;
typedef struct Frame Frame;
typedef enum PieceType PieceType;

int getX(int position);
int getY(int position);
void showBoard(void);
void hideBoard(void);
void swapBoard(void);
void menuBoard(void);
void initBoard(void);
void newGame(void);
void loadFen(const char* fen);
void updateBoard(void);
void moveFrame(int x, int y);
void touchPiece(void);
Piece* getPieceAtPosition(int i, int j);

