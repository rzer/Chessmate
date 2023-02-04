#include "pd.h"
#include "board.h"

enum PieceType{
    None = 0,
    Pawn,
    Rook,
    Knight,
    Bishop,
    Queen,
    King
};

struct Piece{
    PieceType type;
    int oldX;
    int oldY;
    int x;
    int y;
    int position;
    bool isWhite;
    LCDSprite* sprite;
};


static bool isWhiteMove = false;
static bool isShowed = false;
static bool isFlipped = false;
static LCDBitmap* boardBitmap;

static LCDBitmap* whiteBitmaps[6];
static LCDBitmap* blackBitmaps[6];

static LCDSprite* board;

static Piece* pieces[64];

#define CENTER_X 288
#define CENTER_Y 112
#define START_X 190
#define START_Y 14
#define SQUARE_SIZE 28


void initBoard(){

    boardBitmap = loadImage("images/board");
	
    whiteBitmaps[0] = loadImage("images/pw");
    whiteBitmaps[1] = loadImage("images/rw");
    whiteBitmaps[2] = loadImage("images/nw");
    whiteBitmaps[3] = loadImage("images/bw");
    whiteBitmaps[4] = loadImage("images/qw");
    whiteBitmaps[5] = loadImage("images/kw");

    blackBitmaps[0] = loadImage("images/pb");
    blackBitmaps[1] = loadImage("images/rb");
    blackBitmaps[2] = loadImage("images/nb");
    blackBitmaps[3] = loadImage("images/bb");
    blackBitmaps[4] = loadImage("images/qb");
    blackBitmaps[5] = loadImage("images/kb");

    board = pd->sprite->newSprite();
    pd->sprite->moveTo(board, CENTER_X, CENTER_Y);
	pd->sprite->setZIndex(board,1);
    pd->sprite->setImage(board,boardBitmap, isFlipped ? kBitmapFlippedXY : kBitmapUnflipped);

    for (int i = 0; i < 64; i++)
    {

        Piece* piece  = malloc(sizeof(struct Piece));
        piece->position = 0;
        piece->sprite = NULL;
        piece->type = None;
        piece->x = 0;
        piece->y = 0;
        piece->oldX = 0;
        piece->oldY = 0;
        pieces[i] = piece;
    }
    
}

void updatePiecePosition(Piece* piece){
    if (piece->sprite == NULL) return;

    int i = isFlipped ? 7 - piece->x : piece->x;
    int j = isFlipped ? 7 - piece->y : piece->y;

    int x = START_X + i * SQUARE_SIZE;
    int y = START_Y + j * SQUARE_SIZE;
    
    pd->sprite->moveTo(piece->sprite,x,y);
}

void showBoard(void){
    if (isShowed) return;
    isShowed = true;
	pd->sprite->addSprite(board);
}

void hideBoard(void){
    if (!isShowed) return;
    isShowed = false;
    pd->sprite->removeSprite(board);
}

void swapBoard(void){
    isFlipped = !isFlipped;
    pd->sprite->setImage(board,boardBitmap, isFlipped ? kBitmapFlippedXY : kBitmapUnflipped);
    for (int i = 0; i < 64; i++)
    {
        updatePiecePosition(pieces[i]);
    }
}

void menuBoard(){
    if (!isShowed) return;
    pd->system->addMenuItem("Swap board", swapBoard, NULL);
}

void pieceTween(Piece* piece, float t){


    int i = isFlipped ? 7 - piece->x : piece->x;
    int j = isFlipped ? 7 - piece->y : piece->y;

    int newX = START_X + i * SQUARE_SIZE;
    int newY = START_Y + j * SQUARE_SIZE;

    i = isFlipped ? 7 - piece->oldX : piece->oldX;
    j = isFlipped ? 7 - piece->oldY : piece->oldY;

    int oldX = START_X + i * SQUARE_SIZE;
    int oldY = START_Y + j * SQUARE_SIZE;

    t = easeOutQuad(t);

    pd->sprite->moveTo(piece->sprite,lerpInt(oldX, newX, t),lerpInt(oldY, newY, t));
}

void tweenRandom(Piece* piece){
   piece->oldX = piece->x;
   piece->oldY = piece->y;
   piece->position = getRandomInRange(0,63);
   piece->x = piece->position % 8;
   piece->y = piece->position / 8;
   tween(piece, pieceTween, 20 + getRandomInRange(10,20), tweenRandom);
}

void newGame(void){
    loadFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    tweenRandom(getPieceAtPosition(56));
    tweenRandom(getPieceAtPosition(58));
    tweenRandom(getPieceAtPosition(4));
}

Piece* getPieceAtPosition(int position){
    Piece* emptyPiece = NULL;
    for (int i = 0; i < 64; i++)
    {
        Piece* result = pieces[i];
        if (result->position == position) return result;
        if (emptyPiece != NULL) continue;
        if (result->type == None) emptyPiece = result;
    }

    return emptyPiece;
}

LCDBitmap* getPieceBitmap(PieceType type, bool isWhite){
    if (isWhite) return whiteBitmaps[(int)type-1];
    return blackBitmaps[(int)type-1];
}

void setPiece(int position, enum PieceType type, bool isWhite){
    Piece* piece = getPieceAtPosition(position);
    piece->type = type;
    piece->isWhite = isWhite;
    piece->position = position;
    piece->x = position % 8;
    piece->y = position / 8;

    
    if (piece->sprite == NULL){
        
        piece->sprite = pd->sprite->newSprite();
        pd->sprite->addSprite(piece->sprite);
        pd->sprite->setZIndex(piece->sprite,2);
    }
    
    LCDBitmap* bitmap = getPieceBitmap(type,isWhite);
    pd->sprite->setImage(piece->sprite,bitmap,kBitmapUnflipped);
    updatePiecePosition(piece);
}


void loadFen(const char* fen){
    int position = 0;
    int currentMode =0;

    for(int i = 0; i < strlen(fen); i++) {
        char letter = fen[i];

        if (letter == ' '){
            currentMode++;
            continue;
        }

        if (currentMode == 0){
            switch (letter){
                case 'p' : setPiece(position, Pawn, false); break;
                case 'r' : setPiece(position, Rook, false); break;
                case 'n' : setPiece(position, Knight, false); break;
                case 'b' : setPiece(position, Bishop, false); break;
                case 'q' : setPiece(position, Queen, false); break;
                case 'k' : setPiece(position, King, false); break;
                case 'P' : setPiece(position, Pawn, true); break;
                case 'R' : setPiece(position, Rook, true); break;
                case 'N' : setPiece(position, Knight, true); break;
                case 'B' : setPiece(position, Bishop, true); break;
                case 'Q' : setPiece(position, Queen, true); break;
                case 'K' : setPiece(position, King, true); break;
                case '/' : position--; break;
                case '1' : break;
                case '2' : position++; break;
                case '3' : position+=2; break;
                case '4' : position+=3; break;
                case '5' : position+=4; break;
                case '6' : position+=5; break;
                case '7' : position+=6; break;
                case '8' : position+=7; break;
            }
            position++;
        }

        if (currentMode == 1){
            isWhiteMove = letter == 'w';
        }
        
    }
}