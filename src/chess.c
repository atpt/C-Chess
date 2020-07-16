#import <stdio.h>
#import <stdlib.h>

// Flag info
#define W_KSIDE_CASTLE 1
#define W_QSIDE_CASTLE 2
#define B_KSIDE_CASTLE 4
#define B_QSIDE_CASTLE 8
#define FULL_CASTLING_RIGHTS (W_KSIDE_CASTLE | W_QSIDE_CASTLE | B_KSIDE_CASTLE | B_QSIDE_CASTLE)
// The following 3 refer to the REPRESENTATION of the board
#define BOARD_SIZE 120 // Lookup 'mailbox representation' for details
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 12
// Limits of actual playing board within representation
#define TOP_LEFT 21
#define BOTTOM_RIGHT 99
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

// Forward declare everything so we don't have to worry about order of defns
struct board;
struct board makeBoard(int*, int, int, int);
struct board startingPosition();
void test();
char pieceToChar(int);
char indexToRankNumber(int);
void outputBoard(struct board);
int changePlayer(struct board*);

// Hold board position including side to move and flags
struct board {
	int square[BOARD_SIZE];
	int castlingFlags;		// 000...000[BQ][BK][WQ][WK] (Q=queenside,K=kingside)
	int enPassantFlags;		// 000...000hgfedcba
	int player; 					// 0=white; 1=black
};

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
	int num = 10 - (ind / 10);	// 29->8, 99->1
	return '0' + num;			// 8->'8', 1->'1'
}

// Output ASCII representation of b to std.out
void outputBoard(struct board b) {
	int i, piece;
	char c;
	for(i=TOP_LEFT; i<=BOTTOM_RIGHT; i++) {
		if((i % BOARD_WIDTH) == 0) {
			printf("\n");
		} else if((i % BOARD_WIDTH) == (BOARD_WIDTH - 1)) {
			printf("%c|", indexToRankNumber(i));
		} else {
			piece = b.square[i];
			c = pieceToChar(piece);
			if(c != 0) {
				printf("%c|", c);
			}
		}
	}
	printf("\na|b|c|d|e|f|g|h|");
	switch(b.player) {
		case 0:
			printf("W|\n\n"); break;
		case 1:
			printf("b|\n\n"); break;
	}
}

// Return a new struct board with given fields
struct board makeBoard(int pieces[], int castlingFlags, int enPassantFlags, int player) {
	struct board b;
	int i;
	for(i=0; i<BOARD_SIZE; i++) {
		b.square[i] = pieces[i];
	}
	b.castlingFlags = castlingFlags;
	b.enPassantFlags = enPassantFlags;
	b.player = player;
	return b;
}

// Return a new struct board representing starting chess position
struct board startingPosition() {
	int pieces[BOARD_SIZE];
	int i;
	for(i=0; i<BOARD_SIZE; i++) {
		pieces[i] = 0;
	}
	pieces[21] = B_ROOK;
	pieces[22] = B_KNIGHT;
	pieces[23] = B_BISHOP;
	pieces[24] = B_QUEEN;
	pieces[25] = B_KING;
	pieces[26] = B_BISHOP;
	pieces[27] = B_KNIGHT;
	pieces[28] = B_ROOK;
	for(i=31; i<=38; i++) {
		pieces[i] = B_PAWN;
		pieces[i+50] = W_PAWN;
	}
	pieces[91] = W_ROOK;
	pieces[92] = W_KNIGHT;
	pieces[93] = W_BISHOP;
	pieces[94] = W_QUEEN;
	pieces[95] = W_KING;
	pieces[96] = W_BISHOP;
	pieces[97] = W_KNIGHT;
	pieces[98] = W_ROOK;

	int castlingFlags = FULL_CASTLING_RIGHTS;
	int enPassantFlags = 0;
	int player = 0;
	return makeBoard(pieces, castlingFlags, enPassantFlags, player);
}

// Flip b->player
int changePlayer(struct board* b) {
	b->player = (b->player) ? 0 : 1;
	return b->player;
}

void test() {
	struct board pos = startingPosition();
	outputBoard(pos);
	printf("%d\n", changePlayer(&pos));
	outputBoard(pos);
	printf("%d\n", changePlayer(&pos));
	outputBoard(pos);
}

int main() {
	printf("%d\n", FULL_CASTLING_RIGHTS);
	test();
	return 0;
}
