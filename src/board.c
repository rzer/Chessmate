#include "pd.h"
#include "board.h"
#include "bbc.h"

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
    int startX;
    int startY;
    int i;
    int j;
    bool isWhite;
    LCDSprite* sprite;
};

struct Frame {
    int startX;
    int startY;
    int i;
    int j;
    Piece* piece;
    LCDSprite* sprite;
};


static bool isWhiteMove = false;
static bool isShowed = false;
static bool isFlipped = false;

static LCDBitmap* boardBitmap;
static LCDSprite* board;

static LCDBitmap* whiteBitmaps[6];
static LCDBitmap* blackBitmaps[6];

static LCDBitmap* frameBitmap;
static Frame* frame;
static Tween* tweenFrame;

static Piece* pieces[64];

#define CENTER_X 288
#define CENTER_Y 112
#define START_X 190
#define START_Y 14
#define SQUARE_SIZE 28

#define NORMAL_PIECE_ZINDEX 3
#define TOOKED_PIECE_ZINDEX 4


void initBoard(){

    tweenFrame = NULL;
    boardBitmap = loadImage("images/board");
    frameBitmap = loadImage("images/frame");
	
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


    frame = (Frame*) malloc(sizeof(struct Frame));
    frame->piece = NULL;
    frame->i = 0;
    frame->j = 0;
    frame->sprite = pd->sprite->newSprite();
    
    pd->sprite->moveTo(frame->sprite, getX(frame->i), getY(frame->j));
    pd->sprite->setZIndex(frame->sprite, 2);
    pd->sprite->setImage(frame->sprite, frameBitmap, kBitmapUnflipped);

    for (int i = 0; i < 64; i++)
    {

        Piece* piece  = (Piece*) malloc(sizeof(struct Piece));
        piece->sprite = NULL;
        piece->type = None;
        piece->i = 0;
        piece->j = 0;
        pieces[i] = piece;
    }
    
}

int getX(int posI) {
    int i = posI;
    if (isFlipped) i = 7 - i;
    return START_X + i * SQUARE_SIZE;
}

int getY(int posJ) {
    int j = posJ;
    if (isFlipped) j = 7 - j;
    return START_Y + j * SQUARE_SIZE;
}

void updatePiecePosition(Piece* piece){
    if (piece->sprite == NULL) return;
    pd->sprite->moveTo(piece->sprite,getX(piece->i),getY(piece->j));
}

void showBoard(void){
    if (isShowed) return;
    isShowed = true;
	pd->sprite->addSprite(board);
    pd->sprite->addSprite(frame->sprite);
}

void hideBoard(void){
    if (!isShowed) return;
    isShowed = false;
    pd->sprite->removeSprite(board);
}

void updateBoard(void) {
    if (!isShowed) return;
    
    if (buttonsPushed & kButtonLeft) moveFrame(-1, 0);
    if (buttonsPushed & kButtonRight) moveFrame(1,0);

    if (buttonsPushed & kButtonUp) moveFrame(0, -1);
    if (buttonsPushed & kButtonDown) moveFrame(0, 1);

    if (buttonsPushed & kButtonA) touchPiece();
}

int toPosition(int i, int j) {
    return i + j * 8;
}

void touchPiece(void) {
    //Опускаем фигуру
    if (frame->piece != NULL) { 
        Piece* putPiece = frame->piece;
        frame->piece = NULL;
        pd->sprite->setZIndex(putPiece->sprite, NORMAL_PIECE_ZINDEX);

        int move = ai_isLegalMove(toPosition(putPiece->i, putPiece->j), toPosition(frame->i, frame->j), 'q');
        pd->system->logToConsole("%i -> %i (%i)", toPosition(putPiece->i, putPiece->j), toPosition(frame->i, frame->j), move);
        
        //Ход не валидный
        if (move == 0) {
            pd->sprite->moveTo(putPiece->sprite, getX(putPiece->i), getY(putPiece->j));
        }
        else {
            putPiece->i = frame->i;
            putPiece->j = frame->j;
            ai_makeMove(move);
        }

        return;
    }

    //Берём фигуру
    Piece* tookedPiece = getPieceAtPosition(frame->i, frame->j);
    if (tookedPiece == NULL) return;
    frame->piece = tookedPiece;
    pd->sprite->setZIndex(tookedPiece->sprite, TOOKED_PIECE_ZINDEX);
}

void frameTween(Frame* frame, float t) {
    int newX = getX(frame->i);
    int newY = getY(frame->j);
    t = easeOutQuint(t);

    int x = lerpInt(frame->startX, newX, t);
    int y = lerpInt(frame->startY, newY, t);

    pd->sprite->moveTo(frame->sprite, x, y);

    if (frame->piece != NULL) {
        pd->sprite->moveTo(frame->piece->sprite, x, y);
    }
}

void moveFrame(int di, int dj) {

    if (isFlipped) {
        di = -di;
        dj = -dj;
    }

    stopTween(tweenFrame);
   
    float x,y;
    pd->sprite->getPosition(frame->sprite, &x, &y);
    frame->startX = (int)x;
    frame->startY = (int)y;

    
   
    if (di < 0 && frame->i >= -di) frame->i += di;
    if (di > 0 && frame->i <= 7 - di) frame->i += di;
    if (dj < 0 && frame->j >= -dj) frame->j += dj;
    if (dj > 0 && frame->j <= 7 - dj) frame->j += dj;

    
    tweenFrame = tween(frame, frameTween, 10, NULL);
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

    int newX = getX(piece->i);
    int newY = getY(piece->j);

    t = easeOutQuad(t);
    pd->sprite->moveTo(piece->sprite,lerpInt(piece->startX, newX, t),lerpInt(piece->startY, newY, t));
}


void tweenRandom(Piece* piece){
   
   float x, y;
   pd->sprite->getPosition(piece->sprite, &x, &y);
   piece->startX = (int)x;
   piece->startY = (int)y;

   pd->system->logToConsole("coords %f, %f", x, y);
   int pos = getRandomInRange(0,63);
   piece->i = pos % 8;
   piece->j = pos / 8;
   tween(piece, pieceTween, 20 + getRandomInRange(10,20), tweenRandom);
}

void newGame(void){
    loadFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ");

    //tweenRandom(getPieceAtPosition(0,0));
    //tweenRandom(getPieceAtPosition(1, 0));
    //tweenRandom(getPieceAtPosition(1, 1));
}

Piece* getPieceAtPosition(int i, int j){
    Piece* emptyPiece = NULL;
    for (int index = 0; index < 64; index++)
    {
        Piece* result = pieces[index];
        if (result->i == i && result->j == j) return result;
        if (emptyPiece != NULL) continue;
        if (result->type == None) emptyPiece = result;
    }

    return emptyPiece;
}

LCDBitmap* getPieceBitmap(PieceType type, bool isWhite){
    if (isWhite) return whiteBitmaps[(int)type-1];
    return blackBitmaps[(int)type-1];
}

void setPiece(int i, int j, enum PieceType type, bool isWhite){
    Piece* piece = getPieceAtPosition(i,j);
    piece->type = type;
    piece->isWhite = isWhite;
    piece->i = i;
    piece->j = j;

  
    if (piece->sprite == NULL){
        
        piece->sprite = pd->sprite->newSprite();
        pd->sprite->addSprite(piece->sprite);
        pd->sprite->setZIndex(piece->sprite,NORMAL_PIECE_ZINDEX);
    }
    
    LCDBitmap* bitmap = getPieceBitmap(type,isWhite);
    pd->sprite->setImage(piece->sprite,bitmap,kBitmapUnflipped);
    updatePiecePosition(piece);
}


void loadFen(const char* fen){
    int position = 0;
    int currentMode =0;

    ai_loadFen(fen);
    

    for(int i = 0; i < strlen(fen); i++) {
        char letter = fen[i];

        if (letter == ' '){
            currentMode++;
            continue;
        }

        if (currentMode == 0){
            int i = position % 8;
            int j = position / 8;
            switch (letter){
                case 'p' : setPiece(i, j, Pawn, false); break;
                case 'r' : setPiece(i, j, Rook, false); break;
                case 'n' : setPiece(i, j, Knight, false); break;
                case 'b' : setPiece(i, j, Bishop, false); break;
                case 'q' : setPiece(i, j, Queen, false); break;
                case 'k' : setPiece(i, j, King, false); break;
                case 'P' : setPiece(i, j, Pawn, true); break;
                case 'R' : setPiece(i, j, Rook, true); break;
                case 'N' : setPiece(i, j, Knight, true); break;
                case 'B' : setPiece(i, j, Bishop, true); break;
                case 'Q' : setPiece(i, j, Queen, true); break;
                case 'K' : setPiece(i, j, King, true); break;
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