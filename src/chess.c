#import <stdio.h>
#import <stdlib.h>

// Truth values
#define FALSE 0
#define TRUE 1
//Colours
#define WHITE 0
#define BLACK 1
// Flag info
#define W_KSIDE_CASTLE 1
#define W_QSIDE_CASTLE 2
#define B_KSIDE_CASTLE 4
#define B_QSIDE_CASTLE 8
#define FULL_CASTLING_RIGHTS (W_KSIDE_CASTLE | W_QSIDE_CASTLE | B_KSIDE_CASTLE | B_QSIDE_CASTLE)
// The following refer to the REPRESENTATION of the board
// Lookup 'mailbox representation' for details
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 12
#define BOARD_TOP_PADDING 2 // Rows of OUT OF BOARD spaces above
#define BOARD_LEFT_PADDING 1 // Columns of OUT OF BOARD spaces to left
#define BOARD_RIGHT_PADDING 1
#define BOARD_BOTTOM_PADDING 2
#define BOARD_SIZE (BOARD_WIDTH * BOARD_HEIGHT)
// Limits of actual playing board within representation
#define BOARD_TOP_LEFT ((BOARD_TOP_PADDING * BOARD_WIDTH) + BOARD_LEFT_PADDING)
#define BOARD_BOTTOM_RIGHT (BOARD_SIZE - ((BOARD_BOTTOM_PADDING * BOARD_WIDTH) + BOARD_RIGHT_PADDING))
// Square values
#define OOB -1 // Off edge of board; surround board to help move gen
#define EMPTY 0
#define W_PAWN 1
#define W_KNIGHT 2
#define W_BISHOP 3
#define W_ROOK 4
#define W_QUEEN 5
#define W_KING 6
#define B_PAWN 7
#define B_KNIGHT 8
#define B_BISHOP 9
#define B_ROOK 10
#define B_QUEEN 11
#define B_KING 12
// Squares (unwieldy, but avoids a lot of magic numbers later)
#define A1 91
#define B1 92
#define C1 93
#define D1 94
#define E1 95
#define F1 96
#define G1 97
#define H1 98

#define A2 81
#define B2 82
#define C2 83
#define D2 84
#define E2 85
#define F2 86
#define G2 87
#define H2 88

#define A3 71
#define B3 72
#define C3 73
#define D3 74
#define E3 75
#define F3 76
#define G3 77
#define H3 78

#define A4 61
#define B4 62
#define C4 63
#define D4 64
#define E4 65
#define F4 66
#define G4 67
#define H4 68

#define A5 51
#define B5 52
#define C5 53
#define D5 54
#define E5 55
#define F5 56
#define G5 57
#define H5 58

#define A6 41
#define B6 42
#define C6 43
#define D6 44
#define E6 45
#define F6 46
#define G6 47
#define H6 48

#define A7 31
#define B7 32
#define C7 33
#define D7 34
#define E7 35
#define F7 36
#define G7 37
#define H7 38

#define A8 21
#define B8 22
#define C8 23
#define D8 24
#define E8 25
#define F8 26
#define G8 27
#define H8 28

// Piece movement constants
#define WHITE_PAWN_ADVANCE -BOARD_WIDTH
#define WHITE_PAWN_CAPTURE_L (-BOARD_WIDTH)-1
#define WHITE_PAWN_CAPTURE_R (-BOARD_WIDTH)+1
#define BLACK_PAWN_ADVANCE BOARD_WIDTH
#define BLACK_PAWN_CAPTURE_L BOARD_WIDTH-1
#define BLACK_PAWN_CAPTURE_R BOARD_WIDTH+1

// Piece movement arrays
int* KNIGHT_MOVES;
int* BISHOP_MOVES;
int* ROOK_MOVES;
int* QUEEN_MOVES;
int* KING_MOVES;

typedef struct {
	int from;
	int to;
	int castling;
	// If e.p. is possible, holds DESTINATION square of possible e.p. capture
	int enPassant;
	int promotion;
} Move;

// Hold board position including side to move and flags
typedef struct {
	int square[BOARD_SIZE];
	int castlingFlags;		// 000...000[BQ][BK][WQ][WK] (Q=queenside,K=kingside)
	// If e.p. is possible, holds DESTINATION square of possible e.p. capture
	int enPassantFlag;
	int player; 					// WHITE/BLACK
} Board;

// Dynamically expanding array of Moves; for generation and AI
typedef struct {
	Move* list;
	int used;
	int size;
} MoveList;

// Dynamically expanding stack of Boards, for game tree traversal
typedef struct {
	Board* list;
	int used;
	int size;
} BoardStack;

// Forward declare everything so we don't have to worry about order of defns
Board createBoard(int*, int, int, int);
Board startingPosition();
Move createMove(int, int);
Move createSpecialMove(int, int, int, int, int);
Board makeMove(Board, Move);
void makeMoveInPlace(Board*, Move);
void testBoard();
void testPerft();
char pieceToChar(int);
char indexToRankChar(int);
int indexToRank(int);
void outputBoard(Board*);
int changePlayer(Board*);
void indexToAlgebraic(char*, int);
void printAlgebraic(int);
char rankNumToChar(int);
char fileNumToChar(int);
void boardToArray(int*, Board*);
void setupPosition(int*, Board*);
int outOfBounds(int);
void generateMoves(MoveList*, Board*);
void generateWhiteMoves(MoveList*, Board);
void generateBlackMoves(MoveList*, Board);
void initMoveList(MoveList*, int);
void insertMoveList(MoveList*, Move);
void insertMoveListReference(MoveList*, Move*);
void freeMoveList(MoveList*);
Move atIndexMoveList(MoveList*, int);
void initBoardStack(BoardStack*, int);
void pushBoardStack(BoardStack*, Board);
void pushBoardStackReference(BoardStack*, Board*);
Board* atIndexBoardStack(BoardStack*, int);
Board popBoardStack(BoardStack*);
void freeBoardStack(BoardStack*);
void moveToString(char*, Move*);
void outputMove(Move*);
void outputMoveList(MoveList*);
void initGlobalArrays();
void freeGlobalArrays();
int isWhitePiece(int);
int isBlackPiece(int);
int isPiece(int);
int attackedByBlack(Board*, int);
int attackedByWhite(Board*, int);
long perft(Board,int);

void initGlobalArrays() {

	// 2 rows 1 columns, or 2 columns 1 row
	KNIGHT_MOVES = malloc(8 * sizeof(int));
	KNIGHT_MOVES[0] = (2*BOARD_WIDTH)+1;
	KNIGHT_MOVES[1] = (2*BOARD_WIDTH)-1;
	KNIGHT_MOVES[2] = (-2*BOARD_WIDTH)+1;
	KNIGHT_MOVES[3] = (-2*BOARD_WIDTH)-1;
	KNIGHT_MOVES[4] = BOARD_WIDTH+2;
	KNIGHT_MOVES[5] = BOARD_WIDTH-2;
	KNIGHT_MOVES[6] = -BOARD_WIDTH+2;
	KNIGHT_MOVES[7] = -BOARD_WIDTH-2;

	// For Bishop/Rook/Queen we define only one square moves (directions)
	BISHOP_MOVES = malloc(4 * sizeof(int));
	BISHOP_MOVES[0] = BOARD_WIDTH + 1;
	BISHOP_MOVES[1] = BOARD_WIDTH - 1;
	BISHOP_MOVES[2] = -BOARD_WIDTH + 1;
	BISHOP_MOVES[3] = -BOARD_WIDTH - 1;

	ROOK_MOVES = malloc(4 * sizeof(int));
	ROOK_MOVES[0] = BOARD_WIDTH;
	ROOK_MOVES[1] = -BOARD_WIDTH;
	ROOK_MOVES[2] = 1;
	ROOK_MOVES[3] = -1;

	QUEEN_MOVES = malloc(8 * sizeof(int));
	QUEEN_MOVES[0] = BOARD_WIDTH + 1;
	QUEEN_MOVES[1] = BOARD_WIDTH - 1;
	QUEEN_MOVES[2] = -BOARD_WIDTH + 1;
	QUEEN_MOVES[3] = -BOARD_WIDTH - 1;
	QUEEN_MOVES[4] = BOARD_WIDTH;
	QUEEN_MOVES[5] = -BOARD_WIDTH;
	QUEEN_MOVES[6] = 1;
	QUEEN_MOVES[7] = -1;

	KING_MOVES = malloc(8 * sizeof(int));
	KING_MOVES[0] = BOARD_WIDTH + 1;
	KING_MOVES[1] = BOARD_WIDTH - 1;
	KING_MOVES[2] = -BOARD_WIDTH + 1;
	KING_MOVES[3] = -BOARD_WIDTH - 1;
	KING_MOVES[4] = BOARD_WIDTH;
	KING_MOVES[5] = -BOARD_WIDTH;
	KING_MOVES[6] = 1;
	KING_MOVES[7] = -1;

}

void freeGlobalArrays() {

	free(KNIGHT_MOVES);
	free(BISHOP_MOVES);
	free(ROOK_MOVES);
	free(QUEEN_MOVES);
	free(KING_MOVES);
}

// Assign some memory for ml
void initMoveList(MoveList* ml, int initSize) {
	ml->list = malloc(initSize * sizeof(Move));
	if(ml->list == NULL) {
		printf("ERROR: MALLOC FAILED\n"); return;
	}
	ml->used = 0;
	ml->size = initSize;
}

// Add another Move to ml, expanding as needed
void insertMoveList(MoveList* ml, Move m) {
	if(ml->used == ml->size) {
		ml->size *= 2;
		ml->list = realloc(ml->list, ml->size * sizeof(Move));
		if(ml->list == NULL) {
			printf("ERROR: REALLOC FAILED\n"); return;
		}
	}
	ml->list[ml->used++] = m;
}

// Add another Move to ml, expanding as needed
void insertMoveListReference(MoveList* ml, Move* m) {
	if(ml->used == ml->size) {
		ml->size *= 2;
		ml->list = realloc(ml->list, ml->size * sizeof(Move));
		if(ml->list == NULL) {
			printf("ERROR: REALLOC FAILED\n"); return;
		}
	}
	ml->list[ml->used++] = *m;
}

// Return pointer to Move at index
Move atIndexMoveList(MoveList* ml, int index) {
	if((index < 0) || (index >= ml->size)) {
		printf("ERROR: INDEX OUT OF BOUNDS"); return createMove(0,0);
	}
	return (ml->list[index]);
}

// Reset ml, freeing memory
void freeMoveList(MoveList* ml) {
	free(ml->list);
	ml->list = NULL;
	ml->used = ml->size = 0;
}

// Assign some memory for bs
void initBoardStack(BoardStack* bs, int initSize) {
	bs->list = malloc(initSize * sizeof(Board));
	if(bs->list == NULL) {
		printf("ERROR: MALLOC FAILED\n"); return;
	}
	bs->used = 0;
	bs->size = initSize;
}

// Push another Board to bs, expanding as needed
void pushBoardStack(BoardStack* bs, Board b) {
	if(bs->used == bs->size) {
		bs->size *= 2;
		bs->list = realloc(bs->list, bs->size * sizeof(Board));
		if(bs->list == NULL) {
			printf("ERROR: REALLOC FAILED\n"); return;
		}
	}
	bs->list[bs->used++] = b;
}

// Push another Board to bs, expanding as needed
void pushBoardStackReference(BoardStack* bs, Board* b) {
	if(bs->used == bs->size) {
		bs->size *= 2;
		bs->list = realloc(bs->list, bs->size * sizeof(Board));
		if(bs->list == NULL) {
			printf("ERROR: REALLOC FAILED\n"); return;
		}
	}
	bs->list[bs->used++] = *b;
}

// Return pointer to Board at index
Board* atIndexBoardStack(BoardStack* bs, int index) {
	if((index < 0) || (index >= bs->size)) {
		printf("ERROR: INDEX OUT OF BOUNDS"); return NULL;
	}
	return &(bs->list[index]);
}

// Remove and return Board from top of bs
Board popBoardStack(BoardStack* bs) {
	if(bs->used == 0) {
		printf("ERROR: EMPTY STACK"); return startingPosition();
	}
	return bs->list[--(bs->used)];
}

// Reset bs, freeing memory
void freeBoardStack(BoardStack* bs) {
	free(bs->list);
	bs->list = NULL;
	bs->used = bs->size = 0;
}


// Return ASCII representation of piece
char pieceToChar(int piece) {
	switch(piece) {
		case OOB:
			return 0;
		case EMPTY:
			return ' ';
		case W_PAWN:
			return 'P';
		case W_KNIGHT:
			return 'N';
		case W_BISHOP:
			return 'B';
		case W_ROOK:
			return 'R';
		case W_QUEEN:
			return 'Q';
		case W_KING:
			return 'K';
		case B_PAWN:
			return 'p';
		case B_KNIGHT:
			return 'n';
		case B_BISHOP:
			return 'b';
		case B_ROOK:
			return 'r';
		case B_QUEEN:
			return 'q';
		case B_KING:
			return 'k';
		default:
			return 0;
	}
}

int isWhitePiece(int p) {
	switch(p) {
		case W_PAWN:
		case W_KNIGHT:
		case W_BISHOP:
		case W_ROOK:
		case W_QUEEN:
		case W_KING:
			return 1;
		default:
			return 0;
	}
}

int isBlackPiece(int p) {
	switch(p) {
		case B_PAWN:
		case B_KNIGHT:
		case B_BISHOP:
		case B_ROOK:
		case B_QUEEN:
		case B_KING:
			return 1;
		default:
			return 0;
	}
}

int isPiece(int p) {
	return isWhitePiece(p) || isBlackPiece(p);
}

// Return the rank [1-8] of the square at ind
char indexToRankChar(int ind) {
	int num = indexToRank(ind);	// 29->8, 99->1
	return '0' + num;			// 8->'8', 1->'1'
}

int indexToRank(int ind) {
	return (8 + BOARD_TOP_PADDING) - (ind / BOARD_WIDTH);
}

char fileNumToChar(int f) {
	return 'a' + f - BOARD_LEFT_PADDING;
}

char rankNumToChar(int r) {
	return '0' + r;
}

// Make board index ind into notation like "a1"; put string into s
void indexToAlgebraic(char* s, int ind) {
	int file = (ind % BOARD_WIDTH) - (BOARD_LEFT_PADDING - 1);
	int rank = (8 + BOARD_TOP_PADDING) - (ind / BOARD_WIDTH);
	s[0] = fileNumToChar(file);
	s[1] = rankNumToChar(rank);
}

// Put a string representation of board index ind to std.out
void printAlgebraic(int ind) {
	char s[2];
	indexToAlgebraic(s, ind);
	printf("%c%c",s[0],s[1]);
}

// Return non-zero if i is a board index outside the playing area (i.e. padding)
int outOfBounds(int i) {
	int file = i % BOARD_WIDTH;
	// 1st two conditions are above/below
	return (i < BOARD_TOP_LEFT) || (i > BOARD_BOTTOM_RIGHT) || (file < BOARD_LEFT_PADDING) || (file >= BOARD_RIGHT_PADDING + 8);
}

// Put algebraic representation of m into first 4 chars of s
void moveToString(char* s, Move* m) {
	indexToAlgebraic(s, m->from);
	indexToAlgebraic(&s[2], m->to);
}

// Output ASCII representation of m to std.out
void outputMove(Move* m) {
	char s[4];
	moveToString(s, m);
	printf("%c%c-%c%c", s[0], s[1], s[2], s[3]);
}

void outputMoveList(MoveList* ml) {
	int i;
	Move m;
	for(i=0; i<ml->used; i++) {
		m = atIndexMoveList(ml, i);
		printf("%d. ",i);
		outputMove(&m);
		printf("\n");
	}
	printf("\n");
}

// Output ASCII representation of b to std.out
void outputBoard(Board* b) {
	int i, piece;
	char c;
	// Iterate over squares in playing area
	for(i=BOARD_TOP_LEFT; i<=BOARD_BOTTOM_RIGHT; i++) {
		if((i % BOARD_WIDTH) == 0) {
			printf("\n");
		} else if((i % BOARD_WIDTH) == (BOARD_WIDTH - 1)) {
			printf("%c|", indexToRankChar(i));
		} else {
			piece = b->square[i];
			c = pieceToChar(piece);
			if(c != 0) {
				printf("%c|", c);
			}
		}
	}
	printf("\na|b|c|d|e|f|g|h|");
	switch(b->player) {
		case WHITE:
			printf("W|\n"); break;
		case BLACK:
			printf("b|\n"); break;
	}
	if(b->castlingFlags&W_KSIDE_CASTLE) {
		printf("K");
	} else {
		printf("-");
	}
	if(b->castlingFlags&W_QSIDE_CASTLE) {
		printf("Q");
	} else {
		printf("-");
	}
	if(b->castlingFlags&B_KSIDE_CASTLE) {
		printf("k");
	} else {
		printf("-");
	}
	if(b->castlingFlags&B_QSIDE_CASTLE) {
		printf("q");
	} else {
		printf("-");
	}
	if(b->enPassantFlag != 0) {
		printf("\t (e.p. ");
		printAlgebraic(b->enPassantFlag);
		printf(")");
	}
	printf("\n\n");
}

// Return a new Board with given fields
Board createBoard(int pieces[], int castlingFlags, int enPassantFlag, int player) {
	Board b;
	int i;
	for(i=0; i<BOARD_SIZE; i++) {
		b.square[i] = pieces[i];
	}
	b.castlingFlags = castlingFlags;
	b.enPassantFlag = enPassantFlag;
	b.player = player;
	return b;
}

// Return a new Board representing starting chess position
Board startingPosition() {
	int pieces[BOARD_SIZE];
	int i;
	// Put EMPTY in the 64 playing squares and OOB outside
	for(i=0; i<BOARD_SIZE; i++) {
		if(outOfBounds(i)) {
			pieces[i] = OOB;
		} else {
			pieces[i] = EMPTY;
		}
	}
	// Place pieces on start squares
	pieces[A8] = B_ROOK;
	pieces[B8] = B_KNIGHT;
	pieces[C8] = B_BISHOP;
	pieces[D8] = B_QUEEN;
	pieces[E8] = B_KING;
	pieces[F8] = B_BISHOP;
	pieces[G8] = B_KNIGHT;
	pieces[H8] = B_ROOK;
	// Pawns across 7th and 2nd ranks
	for(i=A7; i<=H7; i++) {
		pieces[i] = B_PAWN;
		pieces[i+50] = W_PAWN; // A2 to H2
	}
	pieces[A1] = W_ROOK;
	pieces[B1] = W_KNIGHT;
	pieces[C1] = W_BISHOP;
	pieces[D1] = W_QUEEN;
	pieces[E1] = W_KING;
	pieces[F1] = W_BISHOP;
	pieces[G1] = W_KNIGHT;
	pieces[H1] = W_ROOK;
	// Both players can castle either side
	int castlingFlags = FULL_CASTLING_RIGHTS;
	int enPassantFlag = 0;	// No e.p. in starting position
	int player = WHITE; // White starts
	return createBoard(pieces, castlingFlags, enPassantFlag, player);
}

// Put the values of the 64 game squares in array
void boardToArray(int* array, Board* b) {
	int i, j;
	j = 0;
	for(i=BOARD_TOP_LEFT; i<=BOARD_BOTTOM_RIGHT; i++) {
		// Only copy the 64 playing squares
		if(outOfBounds(i)) {
			continue;
		}
		array[j++] = b->square[i];
	}
}

// Put the 64 values in array into the 64 game squares of b
void setupPosition(int* array, Board* b) {
	int i, j;
	j = 0;
	for(i=BOARD_TOP_LEFT; i<=BOARD_BOTTOM_RIGHT; i++) {
		// Only copy the 64 playing squares
		if(outOfBounds(i)) {
			continue;
		}
		b->square[i] = array[j++];
	}
}

// Flip b.player
int changePlayer(Board* b) {
	b->player = (b->player == WHITE) ? BLACK : WHITE;
	return b->player;
}

// Create a normal move; i.e. no castling/promotion/en-passant rules involved
Move createMove(int from, int to) {
	Move m;
	m.from = from;
	m.to = to;
	m.castling = 0;
	m.enPassant = 0;
	m.promotion = 0;
	return m;
}

// Create a move involving a special rule. The 3 are mutually exclusive,
// so behaviour is undefined if multiple flags set
Move createSpecialMove(int from, int to, int castling, int enPassant, int promotion) {
	Move m;
	m.from = from;
	m.to = to;
	m.castling = castling;
	m.enPassant = enPassant;
	m.promotion = promotion;
	return m;
}

// Return a new board with the position of b after move m is made.
// Does not check the legality of the move; this is done by caller.
Board makeMove(Board b, Move m) {
	// // Output move, for debugging
	// printf("Moving: ");
	// printAlgebraic(m.from);
	// printf(", ");
	// printAlgebraic(m.to);
	// printf("\n");
	b.square[m.to] = b.square[m.from];	// Put piece in new square
	b.square[m.from] = EMPTY;						// ...and clear old square
	if(m.promotion != 0) {
		b.square[m.to] = m.promotion;			// Change piece to Q/R/B/N
	} else if(m.enPassant != 0) {
		// Capture the en passant pawn, which is 1 rank above/below the e.p. flag
		if(b.player) {
			b.square[m.enPassant-BOARD_WIDTH] = EMPTY;
		} else {
			b.square[m.enPassant+BOARD_WIDTH] = EMPTY;
		}
	} else if(m.castling != 0) {
		b.square[m.to] = EMPTY; // King doesn't actually go to m.to
		switch(m.castling) {
			case W_KSIDE_CASTLE:
				b.square[G1] = W_KING; // Put king and rook in right place
				b.square[F1] = W_ROOK;
				b.square[H1] = EMPTY;	 // ...and remove rook from corner
				// White can't castle again on either side
				b.castlingFlags -= (b.castlingFlags & (W_KSIDE_CASTLE | W_QSIDE_CASTLE));
				break;
			case W_QSIDE_CASTLE:
				b.square[C1] = W_KING;
				b.square[D1] = W_ROOK;
				b.square[A1] = EMPTY;
				b.castlingFlags -= (b.castlingFlags & (W_KSIDE_CASTLE | W_QSIDE_CASTLE));
				break;
			case B_KSIDE_CASTLE:
				b.square[G8] = B_KING;
				b.square[F8] = B_ROOK;
				b.square[H8] = EMPTY;
				b.castlingFlags -= (b.castlingFlags & (B_KSIDE_CASTLE | B_QSIDE_CASTLE));
				break;
			case B_QSIDE_CASTLE:
				b.square[C8] = B_KING;
				b.square[D8] = B_ROOK;
				b.square[A8] = EMPTY;
				b.castlingFlags -= (b.castlingFlags & (B_KSIDE_CASTLE | B_QSIDE_CASTLE));
				break;
		}
	} else {
		// Non-castling moves which destroy castling rights. Once all castling
		// rights are lost this will never be activated again
		if(b.castlingFlags) {
			// What piece was moved?
			switch(b.square[m.to]) {
				// King move loses all castling rights for that player
				case W_KING:
					b.castlingFlags -= (b.castlingFlags & (W_KSIDE_CASTLE | W_QSIDE_CASTLE)); break;
				case B_KING:
					b.castlingFlags -= (b.castlingFlags & (B_KSIDE_CASTLE | B_QSIDE_CASTLE)); break;
				// Rook move loses rights only on that side of board
				case W_ROOK:
					if(m.from == H1) {
						b.castlingFlags -= (b.castlingFlags & W_KSIDE_CASTLE);
					} else if(m.from == A1) {
						b.castlingFlags -= (b.castlingFlags & W_QSIDE_CASTLE);
					}
					break;
				case B_ROOK:
					if(m.from == H8) {
						b.castlingFlags -= (b.castlingFlags & B_KSIDE_CASTLE);
					} else if(m.from == A8) {
						b.castlingFlags -= (b.castlingFlags & B_QSIDE_CASTLE);
					}
					break;
			}
		}
	}
	// Condition for en-passant to become legal for black. Read as:
	// --We just moved a white pawn, and
	// --We moved it two spaces, and
	// 		--There's a black pawn just to our left, OR...
	//		--...a black pawn just to our right
	// (Who designed a game with such ugly rules? ;) )
	// There's no need to check out of bounds because of the padding
	if((b.square[m.to] == W_PAWN) && ((m.from - m.to) == (2*BOARD_WIDTH)) && ((b.square[m.to-1] == B_PAWN) || (b.square[m.to+1] == B_PAWN)) ) {
		// Record square where e.p. capture would finish
		b.enPassantFlag = m.to + BOARD_WIDTH;
	} else if((b.square[m.to] == B_PAWN) && ((m.to - m.from) == (2*BOARD_WIDTH)) && ((b.square[m.to-1] == W_PAWN) || (b.square[m.to+1] == W_PAWN)) ) {
		b.enPassantFlag = m.to - BOARD_WIDTH;
	} else {
		b.enPassantFlag = 0;
	}
	// Other player gets a turn now
	changePlayer(&b);
	return b;
}

// Make a move without copying b. [UNFINISHED]
void makeMoveInPlace(Board* b, Move m) {

	b->square[m.to] = b->square[m.from];
	b->square[m.from] = 0;
	changePlayer(b);
	// TODO: Handle special moves
}

// Return non-zero iff some black piece attacks square
// Does not require the attacking move to be fully-legal: a pinned piece
// can give check, even though it couldn't take the King
int attackedByBlack(Board* b, int square) {
	// Idea: Pretend each black piece is at square; see if it can reach a
	// piece of the same type (exploits symmetry of moves)
	int i, j, toIndex, toPiece;

	// Can we reach a black pawn with a black pawn capture?
	// Pawn moves are asymmetric so we subtract the move, not add
	toIndex = square - BLACK_PAWN_CAPTURE_L;
	toPiece = b->square[toIndex];
	if(toPiece == B_PAWN) {
		return TRUE;
	}
	toIndex = square - BLACK_PAWN_CAPTURE_R;
	toPiece = b->square[toIndex];
	if(toPiece == B_PAWN) {
		return TRUE;
	}

	// Can we reach a black knight/king with a knight/king move?
	for(i=0; i<8; i++) {
		toIndex = square + KNIGHT_MOVES[i];
		toPiece = b->square[toIndex];
		if(toPiece == B_KNIGHT) {
			return TRUE;
		}
		toIndex = square + KING_MOVES[i];
		toPiece = b->square[toIndex];
		if(toPiece == B_KING) {
			return TRUE;
		}
	}

	// Note: cannot combine diagonal and lateral search because they may
	// break at different points! But we can cover all Queen moves as a
	// combination of Rook+Bishop moves.
	// Can we reach a black bishop/queen with a diagonal move?
	for(j=0; j<4; j++) {
		for(i=1; i<8; i++) {
			toIndex = square + (BISHOP_MOVES[j] * i);
			toPiece = b->square[toIndex];
			if((toPiece == B_BISHOP) || (toPiece == B_QUEEN)) {
				return TRUE;
			} else if(toPiece == EMPTY) {
				continue;
			} else {
				break; // Bishop/Queen cannot jump over white or black piece (or OOB)
			}
		}
	}

	// Can we reach a black rook/queen with a lateral move?
	for(j=0; j<4; j++) {
		for(i=1; i<8; i++) {
			toIndex = square + (ROOK_MOVES[j] * i);
			toPiece = b->square[toIndex];
			if((toPiece == B_ROOK) || (toPiece == B_QUEEN)) {
				return TRUE;
			} else if(toPiece == EMPTY) {
				continue;
			} else {
				break;
			}
		}
	}
	// Fell through, not attacked
	return FALSE;
}

// Return non-zero iff some white piece attacks square
int attackedByWhite(Board* b, int square) {
	int i, j, toIndex, toPiece;

	// Can we reach a white pawn with a white pawn capture?
	// Pawn moves are asymmetric so we subtract the move, not add
	toIndex = square - WHITE_PAWN_CAPTURE_L;
	toPiece = b->square[toIndex];
	if(toPiece == W_PAWN) {
		return TRUE;
	}
	toIndex = square - WHITE_PAWN_CAPTURE_R;
	toPiece = b->square[toIndex];
	if(toPiece == W_PAWN) {
		return TRUE;
	}

	// Can we reach a white knight/king with a knight/king move?
	for(i=0; i<8; i++) {
		toIndex = square + KNIGHT_MOVES[i];
		toPiece = b->square[toIndex];
		if(toPiece == W_KNIGHT) {
			return TRUE;
		}
		toIndex = square + KING_MOVES[i];
		toPiece = b->square[toIndex];
		if(toPiece == W_KING) {
			return TRUE;
		}
	}

	// Note: cannot combine diagonal and lateral search because they may
	// break at different points! But we can cover all Queen moves as a
	// combination of Rook+Bishop moves.
	// Can we reach a black bishop/queen with a diagonal move?
	for(j=0; j<4; j++) {
		for(i=1; i<8; i++) {
			toIndex = square + (BISHOP_MOVES[j] * i);
			toPiece = b->square[toIndex];
			if((toPiece == W_BISHOP) || (toPiece == W_QUEEN)) {
				return TRUE;
			} else if(toPiece == EMPTY) {
				continue;
			} else {
				break; // Bishop/Queen cannot jump over white or black piece (or OOB)
			}
		}
	}

	// Can we reach a black rook/queen with a lateral move?
	for(j=0; j<4; j++) {
		for(i=1; i<8; i++) {
			toIndex = square + (ROOK_MOVES[j] * i);
			toPiece = b->square[toIndex];
			if((toPiece == W_ROOK) || (toPiece == W_QUEEN)) {
				return TRUE;
			} else if(toPiece == EMPTY) {
				continue;
			} else {
				break;
			}
		}
	}
	// Fell through, not attacked
	return FALSE;
}

void generateWhiteMoves(MoveList* output, Board b) {
	int fromIndex, fromPiece, toIndex, toPiece, i, j, rank, kingPosition, oldKingPosition, removalCount;
	oldKingPosition = 0;
	Move m;
	Board tempBoard;
	int* removals;
	removalCount = 0;
	MoveList mlList;
	initMoveList(&mlList, 10);
	MoveList *ml = &mlList;
	for(fromIndex=BOARD_TOP_LEFT; fromIndex<BOARD_BOTTOM_RIGHT; fromIndex++) {
		fromPiece = b.square[fromIndex];
		switch(fromPiece) {
			case EMPTY:
			case OOB:
			case B_PAWN:
			case B_KNIGHT:
			case B_BISHOP:
			case B_ROOK:
			case B_QUEEN:
			case B_KING:
				continue;
			case W_PAWN:
				// Vertical advances (must be into empty square)
				toIndex = fromIndex + WHITE_PAWN_ADVANCE;
				toPiece = b.square[toIndex];
				if(toPiece == EMPTY) {
					rank = indexToRank(toIndex);
					if(rank == 8) {
						// Promotion
						insertMoveList(ml, createSpecialMove(fromIndex, toIndex, 0, 0, W_KNIGHT));
						insertMoveList(ml, createSpecialMove(fromIndex, toIndex, 0, 0, W_BISHOP));
						insertMoveList(ml, createSpecialMove(fromIndex, toIndex, 0, 0, W_ROOK));
						insertMoveList(ml, createSpecialMove(fromIndex, toIndex, 0, 0, W_QUEEN));
					} else {
						insertMoveList(ml, createMove(fromIndex, toIndex));
						if(rank == 3) {
							// Moving two squares on first move
							toIndex = toIndex + WHITE_PAWN_ADVANCE;
							toPiece = b.square[toIndex];
							if(toPiece == EMPTY) {
								insertMoveList(ml, createMove(fromIndex, toIndex));
							}
						}
					}
				}
				// Captures
				toIndex = fromIndex + WHITE_PAWN_CAPTURE_L;
				toPiece = b.square[toIndex];
				if(isBlackPiece(toPiece)) {
					insertMoveList(ml, createMove(fromIndex, toIndex));
				} else if(b.enPassantFlag == toIndex) {
					insertMoveList(ml, createSpecialMove(fromIndex, toIndex, 0, b.enPassantFlag, 0));
				}
				toIndex = fromIndex + WHITE_PAWN_CAPTURE_R;
				toPiece = b.square[toIndex];
				if(isBlackPiece(toPiece)) {
					insertMoveList(ml, createMove(fromIndex, toIndex));
				} else if(b.enPassantFlag == toIndex) {
					insertMoveList(ml, createSpecialMove(fromIndex, toIndex, 0, b.enPassantFlag, 0));
				}
				break;
			case W_KNIGHT:
				for(i=0; i<8; i++) {
					toIndex = fromIndex + KNIGHT_MOVES[i];
					toPiece = b.square[toIndex];
					if((toPiece == EMPTY) || isBlackPiece(toPiece)) {
						insertMoveList(ml, createMove(fromIndex, toIndex));
					}
				}
				break;
			case W_BISHOP:
				// 4 directions
				for(j=0; j<4; j++) {
					// Can move up to 7 squares in each direction
					for(i=1; i<8; i++) {
						toIndex = fromIndex + (BISHOP_MOVES[j] * i);
						toPiece = b.square[toIndex];
						if(toPiece == EMPTY) {
							// Keep going in this direction
							insertMoveList(ml, createMove(fromIndex, toIndex));
						} else if(isBlackPiece(toPiece)) {
							// Capture -> can move here but no further
							insertMoveList(ml, createMove(fromIndex, toIndex));
							break;
						} else {
							// White piece or OOB -> can't move here
							break;
						}
					}
				}
				break;
			case W_ROOK:
				// 4 directions
				for(j=0; j<4; j++) {
					// Can move up to 7 squares in each direction
					for(i=1; i<8; i++) {
						toIndex = fromIndex + (ROOK_MOVES[j] * i);
						toPiece = b.square[toIndex];
						if(toPiece == EMPTY) {
							// Keep going in this direction
							insertMoveList(ml, createMove(fromIndex, toIndex));
						} else if(isBlackPiece(toPiece)) {
							// Capture -> can move here but no further
							insertMoveList(ml, createMove(fromIndex, toIndex));
							break;
						} else {
							// White piece or OOB -> can't move here
							break;
						}
					}
				}
				break;
			case W_QUEEN:
				// 8 directions
				for(j=0; j<8; j++) {
					// Can move up to 7 squares in each direction
					for(i=1; i<8; i++) {
						toIndex = fromIndex + (QUEEN_MOVES[j] * i);
						toPiece = b.square[toIndex];
						if(toPiece == EMPTY) {
							// Keep going in this direction
							insertMoveList(ml, createMove(fromIndex, toIndex));
						} else if(isBlackPiece(toPiece)) {
							// Capture -> can move here but no further
							insertMoveList(ml, createMove(fromIndex, toIndex));
							break;
						} else {
							// White piece or OOB -> can't move here
							break;
						}
					}
				}
				break;
			case W_KING:
				oldKingPosition = fromIndex;
				for(i=0; i<8; i++) {
					toIndex = fromIndex + KING_MOVES[i];
					toPiece = b.square[toIndex];
					if(((toPiece == EMPTY) || isBlackPiece(toPiece)) && !attackedByBlack(&b, toIndex)) {
						insertMoveList(ml, createMove(fromIndex, toIndex));
					}
				}
				// Can white castle? We require:
				// --King on starting square
				// --Sqaures between King and Rook vacant
				// --King not in check, and wouldn't go through or into check
				// Castling rights -> rook is still in corner, so don't check this
				// attackedByBlack is expensive, so use lazy AND and check it last
				if((b.castlingFlags & W_KSIDE_CASTLE) && (fromIndex == E1) && (b.square[F1] == EMPTY) && (b.square[G1] == EMPTY) && !attackedByBlack(&b, E1) && !attackedByBlack(&b, F1) && !attackedByBlack(&b, G1)) {
					insertMoveList(ml, createSpecialMove(E1, G1, W_KSIDE_CASTLE, 0, 0));
				}
				if((b.castlingFlags & W_QSIDE_CASTLE) && (fromIndex == E1) && (b.square[D1] == EMPTY) && (b.square[C1] == EMPTY) && (b.square[B1] == EMPTY) && !attackedByBlack(&b, E1) && !attackedByBlack(&b, D1) && !attackedByBlack(&b, C1)) {
					insertMoveList(ml, createSpecialMove(E1, C1, W_QSIDE_CASTLE, 0, 0));
				}
				break;
		}
	}
	// outputMoveList(ml); // For debugging
	// Remove moves would put us in check
	removals = malloc((ml->used) * sizeof(int));
	for(i=0; i<(ml->used); i++) {
		removals[i] = FALSE;
		m = atIndexMoveList(ml, i);
		// King moves change kingPosition
		kingPosition = (b.square[m.from] == W_KING) ? m.to : oldKingPosition;
		tempBoard = makeMove(b, m);
		// outputBoard(&tempBoard); // For debugging
		if(attackedByBlack(&tempBoard, kingPosition)) {
			removals[i] = TRUE;
			removalCount++;
		}
	}
	initMoveList(output, (ml->used) - removalCount);
	for(i=0; i<(ml->used); i++) {
		if(!removals[i]) {
			insertMoveList(output, atIndexMoveList(ml, i));
		}
	}
	freeMoveList(ml);
	// outputMoveList(output); // For debugging
}

void generateBlackMoves(MoveList* outputList, Board b) {
	int fromIndex, fromPiece, toIndex, toPiece, i, j, rank, kingPosition, oldKingPosition, removalCount;
	oldKingPosition = 0;
	Move m;
	Board tempBoard;
	int* removals;
	removalCount = 0;
	MoveList mlList;
	initMoveList(&mlList, 10);
	MoveList *ml = &mlList;
	for(fromIndex=BOARD_TOP_LEFT; fromIndex<BOARD_BOTTOM_RIGHT; fromIndex++) {
		fromPiece = b.square[fromIndex];
		switch(fromPiece) {
			case EMPTY:
			case OOB:
			case W_PAWN:
			case W_KNIGHT:
			case W_BISHOP:
			case W_ROOK:
			case W_QUEEN:
			case W_KING:
				continue;
			case B_PAWN:
				// Vertical advances (must be into empty square)
				toIndex = fromIndex + BLACK_PAWN_ADVANCE;
				toPiece = b.square[toIndex];
				if(toPiece == EMPTY) {
					rank = indexToRank(toIndex);
					if(rank == 1) {
						// Promotion
						insertMoveList(ml, createSpecialMove(fromIndex, toIndex, 0, 0, B_KNIGHT));
						insertMoveList(ml, createSpecialMove(fromIndex, toIndex, 0, 0, B_BISHOP));
						insertMoveList(ml, createSpecialMove(fromIndex, toIndex, 0, 0, B_ROOK));
						insertMoveList(ml, createSpecialMove(fromIndex, toIndex, 0, 0, B_QUEEN));
					} else {
						insertMoveList(ml, createMove(fromIndex, toIndex));
						if(rank == 6) {
							// Moving two squares on first move
							toIndex = toIndex + BLACK_PAWN_ADVANCE;
							toPiece = b.square[toIndex];
							if(toPiece == EMPTY) {
								insertMoveList(ml, createMove(fromIndex, toIndex));
							}
						}
					}
				}
				// Captures
				toIndex = fromIndex + BLACK_PAWN_CAPTURE_L;
				toPiece = b.square[toIndex];
				if(isWhitePiece(toPiece)) {
					insertMoveList(ml, createMove(fromIndex, toIndex));
				} else if(b.enPassantFlag == toIndex) {
					insertMoveList(ml, createSpecialMove(fromIndex, toIndex, 0, b.enPassantFlag, 0));
				}
				toIndex = fromIndex + BLACK_PAWN_CAPTURE_R;
				toPiece = b.square[toIndex];
				if(isWhitePiece(toPiece)) {
					insertMoveList(ml, createMove(fromIndex, toIndex));
				} else if(b.enPassantFlag == toIndex) {
					insertMoveList(ml, createSpecialMove(fromIndex, toIndex, 0, b.enPassantFlag, 0));
				}
				break;
			case B_KNIGHT:
				for(i=0; i<8; i++) {
					toIndex = fromIndex + KNIGHT_MOVES[i];
					toPiece = b.square[toIndex];
					if((toPiece == EMPTY) || isWhitePiece(toPiece)) {
						insertMoveList(ml, createMove(fromIndex, toIndex));
					}
				}
				break;
			case B_BISHOP:
				// 4 directions
				for(j=0; j<4; j++) {
					// Can move up to 7 squares in each direction
					for(i=1; i<8; i++) {
						toIndex = fromIndex + (BISHOP_MOVES[j] * i);
						toPiece = b.square[toIndex];
						if(toPiece == EMPTY) {
							// Keep going in this direction
							insertMoveList(ml, createMove(fromIndex, toIndex));
						} else if(isWhitePiece(toPiece)) {
							// Capture -> can move here but no further
							insertMoveList(ml, createMove(fromIndex, toIndex));
							break;
						} else {
							break;
						}
					}
				}
				break;
			case B_ROOK:
				// 4 directions
				for(j=0; j<4; j++) {
					// Can move up to 7 squares in each direction
					for(i=1; i<8; i++) {
						toIndex = fromIndex + (ROOK_MOVES[j] * i);
						toPiece = b.square[toIndex];
						if(toPiece == EMPTY) {
							// Keep going in this direction
							insertMoveList(ml, createMove(fromIndex, toIndex));
						} else if(isWhitePiece(toPiece)) {
							// Capture -> can move here but no further
							insertMoveList(ml, createMove(fromIndex, toIndex));
							break;
						} else {
							break;
						}
					}
				}
				break;
			case B_QUEEN:
				// 8 directions
				for(j=0; j<8; j++) {
					// Can move up to 7 squares in each direction
					for(i=1; i<8; i++) {
						toIndex = fromIndex + (QUEEN_MOVES[j] * i);
						toPiece = b.square[toIndex];
						if(toPiece == EMPTY) {
							// Keep going in this direction
							insertMoveList(ml, createMove(fromIndex, toIndex));
						} else if(isWhitePiece(toPiece)) {
							// Capture -> can move here but no further
							insertMoveList(ml, createMove(fromIndex, toIndex));
							break;
						} else {
							break;
						}
					}
				}
				break;
			case B_KING:
				oldKingPosition = fromIndex;
				for(i=0; i<8; i++) {
					toIndex = fromIndex + KING_MOVES[i];
					toPiece = b.square[toIndex];
					if(((toPiece == EMPTY) || isWhitePiece(toPiece)) && !attackedByWhite(&b, toIndex)) {
						insertMoveList(ml, createMove(fromIndex, toIndex));
					}
				}
				// Can black castle?
				if((b.castlingFlags & B_KSIDE_CASTLE) && (fromIndex == E8) && (b.square[F8] == EMPTY) && (b.square[G8] == EMPTY) && !attackedByWhite(&b, E8) && !attackedByWhite(&b, F8) && !attackedByWhite(&b, G8)) {
					insertMoveList(ml, createSpecialMove(E8, G8, B_KSIDE_CASTLE, 0, 0));
				}
				if((b.castlingFlags & B_QSIDE_CASTLE) && (fromIndex == E8) && (b.square[D8] == EMPTY) && (b.square[C8] == EMPTY) && (b.square[B8] == EMPTY) && !attackedByWhite(&b, E8) && !attackedByWhite(&b, D8) && !attackedByWhite(&b, C8)) {
					insertMoveList(ml, createSpecialMove(E8, C8, B_QSIDE_CASTLE, 0, 0));
				}
				break;

		}
	}
	// outputMoveList(ml); // For debugging
	// Remove moves would put us in check
	removals = malloc((ml->used) * sizeof(int));
	for(i=0; i<(ml->used); i++) {
		removals[i] = FALSE;
		m = atIndexMoveList(ml, i);

		kingPosition = (b.square[m.from] == B_KING) ? m.to : oldKingPosition;
		tempBoard = makeMove(b, m);
		// outputBoard(&tempBoard); // For debugging
		if(attackedByWhite(&tempBoard, kingPosition)) {
			removals[i] = TRUE;
			removalCount++;
		}
	}
	initMoveList(outputList, (ml->used) - removalCount);
	for(i=0; i<(ml->used); i++) {
		if(!removals[i]) {
			insertMoveList(outputList, atIndexMoveList(ml, i));
		}
	}
	freeMoveList(ml);
	// outputMoveList(outputList); // For debugging
}

void generateMoves(MoveList* ml, Board* b) {
	switch(b->player) {
		case WHITE:
			generateWhiteMoves(ml, *b); break;
		case BLACK:
			generateBlackMoves(ml, *b); break;
	}
}

// Count all legal move sequences of length <= depth, from position b
long perft(Board b, int depth) {
	// printf("Depth: %d\n", depth);
	// outputBoard(&b);
	if(depth == 0) {
		return 1;
	} else {
		MoveList ml;
		generateMoves(&ml, &b);
		int i;
		long total = 0;
		for(i=0; i<ml.used; i++) {
			total += perft(makeMove(b, atIndexMoveList(&ml, i)), depth - 1);
		}
		return total;
	}
}

void testBoard() {
	Board pos = startingPosition();

	pos = makeMove(pos, createMove(E2, E4));
	pos = makeMove(pos, createMove(E7, E5));
	pos = makeMove(pos, createMove(G1, H3));
	pos = makeMove(pos, createMove(B8, C6));
	pos = makeMove(pos, createMove(F1, C4));
	pos = makeMove(pos, createMove(F8, C5));
	pos = makeMove(pos, createSpecialMove(E1, G1, W_KSIDE_CASTLE, 0, 0));
	pos = makeMove(pos, createMove(G8, H6));
	pos = makeMove(pos, createMove(A2, A3));
	pos = makeMove(pos, createSpecialMove(E8, G8, B_KSIDE_CASTLE, 0, 0));
	// pos = makeMove(pos, createMove(C4, A2));
	// pos = makeMove(pos, createMove(F2, F4));
	// pos = makeMove(pos, createMove(C5, B6));
	// pos = makeMove(pos, createSpecialMove(E1, G1, W_KSIDE_CASTLE, 0, 0));
	// pos = makeMove(pos, createSpecialMove(E8, G8, B_KSIDE_CASTLE, 0, 0));
	// pos = makeMove(pos, createMove(B1, C3));
	// pos = makeMove(pos, createMove(B8, A6));

	outputBoard(&pos);

	MoveList out;
	// initMoveList(&ml, 10);
	generateMoves(&out, &pos);

	outputMoveList(&out);

	freeMoveList(&out);

	// Board pos2 = makeMove(pos, createMove(A2, A3));
	// outputBoard(&pos2);
	// pos2 = makeMove(pos2, createMove(D7, D5));
	// outputBoard(&pos2);
	// pos2 = makeMove(pos2, createMove(A3, A4));
	// outputBoard(&pos2);
	// pos2 = makeMove(pos2, createMove(D5, D4));
	// outputBoard(&pos2);
	// pos2 = makeMove(pos2, createMove(A1, A3));
	// outputBoard(&pos2);
	// pos2 = makeMove(pos2, createMove(F7, F5));
	// outputBoard(&pos2);
	// pos2 = makeMove(pos2, createMove(A3, A1));
	// outputBoard(&pos2);
	// pos2 = makeMove(pos2, createMove(F5, F4));
	// outputBoard(&pos2);
	// pos2 = makeMove(pos2, createMove(E2, E4));
	// outputBoard(&pos2);
	// pos2 = makeMove(pos2, createSpecialMove(D4, E3, 0, pos2.enPassantFlag, 0));
	// outputBoard(&pos2);
	//
	// int array[64];
	// boardToArray(array, &pos2);
	// array[0] = B_BISHOP;
	// array[1] = B_ROOK;
	// setupPosition(array, &pos2);
	// outputBoard(&pos2);
	// int i;
	// for(i=0; i<64; i++) {
	// 	printf("%c ", pieceToChar(array[i]));
	// }


	// pos2 = makeMove(pos2, createSpecialMove(E1, G1, W_KSIDE_CASTLE, 0, 0));
	// // pos2 = makeMove(pos2, createMove(H1, F1));
	// outputBoard(&pos2);
	// pos2 = makeMove(pos2, createSpecialMove(E8, G8, B_KSIDE_CASTLE, 0, 0));
	// outputBoard(&pos2);
	// printf("%d\n", changePlayer(&pos));
	// outputBoard(&pos);
	// printf("%d\n", changePlayer(&pos));
	// outputBoard(&pos);

}

void testPerft() {
	Board b = startingPosition();
	long result = perft(b, 6);
	printf("%ld\n", result);
}

int main() {
	initGlobalArrays();
	// testBoard();
	testPerft();
	freeGlobalArrays();
	return 0;
}
