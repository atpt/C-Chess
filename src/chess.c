#import <stdio.h>
#import <stdlib.h>

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

// Dynamically expanding array of moves; for generation and AI
typedef struct {
	Move* list;
	int used;
	int size;
} MoveList;

// Forward declare everything so we don't have to worry about order of defns
Board createBoard(int*, int, int, int);
Board startingPosition();
Move createMove(int, int);
Move createSpecialMove(int, int, int, int, int);
Board makeMove(Board, Move);
void makeMoveInPlace(Board*, Move);
void test();
char pieceToChar(int);
char indexToRankNumber(int);
void outputBoard(Board*);
int changePlayer(Board*);
void indexToAlgebraic(char*, int);
void printAlgebraic(int);
char rankNumToChar(int);
char fileNumToChar(int);
void boardToArray(int*, Board*);
void setupPosition(int*, Board*);
int outOfBounds(int);

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

// Reset ml, freeing memory
void freeMoveList(MoveList* ml) {
	free(ml->list);
	ml->list = NULL;
	ml->used = ml->size = 0;
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

// Return the rank [1-8] of the square at ind
char indexToRankNumber(int ind) {
	int num = (8 + BOARD_TOP_PADDING) - (ind / BOARD_WIDTH);	// 29->8, 99->1
	return '0' + num;			// 8->'8', 1->'1'
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

// Output ASCII representation of b to std.out
void outputBoard(Board* b) {
	int i, piece;
	char c;
	// Iterate over squares in playing area
	for(i=BOARD_TOP_LEFT; i<=BOARD_BOTTOM_RIGHT; i++) {
		if((i % BOARD_WIDTH) == 0) {
			printf("\n");
		} else if((i % BOARD_WIDTH) == (BOARD_WIDTH - 1)) {
			printf("%c|", indexToRankNumber(i));
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

void test() {
	Board pos = startingPosition();
	outputBoard(&pos);
	Board pos2 = makeMove(pos, createMove(A2, A3));
	outputBoard(&pos2);
	pos2 = makeMove(pos2, createMove(D7, D5));
	outputBoard(&pos2);
	pos2 = makeMove(pos2, createMove(A3, A4));
	outputBoard(&pos2);
	pos2 = makeMove(pos2, createMove(D5, D4));
	outputBoard(&pos2);
	pos2 = makeMove(pos2, createMove(A1, A3));
	outputBoard(&pos2);
	pos2 = makeMove(pos2, createMove(F7, F5));
	outputBoard(&pos2);
	pos2 = makeMove(pos2, createMove(A3, A1));
	outputBoard(&pos2);
	pos2 = makeMove(pos2, createMove(F5, F4));
	outputBoard(&pos2);
	pos2 = makeMove(pos2, createMove(E2, E4));
	outputBoard(&pos2);
	pos2 = makeMove(pos2, createSpecialMove(D4, E3, 0, pos2.enPassantFlag, 0));
	outputBoard(&pos2);

	int array[64];
	boardToArray(array, &pos2);
	array[0] = B_BISHOP;
	array[1] = B_ROOK;
	setupPosition(array, &pos2);
	outputBoard(&pos2);
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

int main() {
	test();
	return 0;
}
