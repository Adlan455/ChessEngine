#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <ctype.h>
typedef uint64_t U64;
typedef unsigned _BitInt(4) U4;
#define pond 0b0001
#define bishop 0b0010
#define knight 0b0011
#define rook 0b0100
#define queen 0b0101
#define king 0b0111
#define white 0b0000
#define black 0b1000



/*Todo: 
1) evaluate if own piece blocking path, hence cnt go.
2) capture piece / must implement diagonal taking for pawns
3) Checks


*/
// piece encoding UPPER = WHITE lower = black

// 8  0  0  0  0  0  0  0  0   (Rank 8: a8 - h8)
// 7  0  0  0  0  0  0  0  0   (Rank 7: a7 - h7)
// 6  0  0  0  0  0  0  0  0   (Rank 6: a6 - h6)
// 5  0  0  0  0  0  0  0  0   (Rank 5: a5 - h5)
// 4  0  0  0  0  0  0  0  0   (Rank 4: a4 - h4)
// 3  0  0  0  0  0  0  0  0   (Rank 3: a3 - h3)
// 2  1  1  1  1  1  1  1  1   (Rank 2: a2 - h2, White Pawns)
// 1  0  0  0  0  0  0  0  0   (Rank 1: a1 - h1)
   
//    A  B  C  D  E  F  G  H




// chest board representation
typedef struct  
{
    U64 chess_board;

    U64 WP;
    U64 WN;
    U64 WB;
    U64 WR;
    U64 WQ;
    U64 WK;

    U64 BP;
    U64 BN;
    U64 BB;
    U64 BR;
    U64 BQ;
    U64 BK;

    U64 ALLWHITE;
    U64 ALLBLACK;
    U64 OCCUPIED;
    U64 EMPTY;

}Board;
//----------------------------------------Prototyping---------------------------------------------//
void printBoard(Board *b);
U64 *checkMove(Board *b, U64 piece, U64 dest, U4* pointer);
U64 translateMove(char *p,int *row, int *col);
void updateBoard(Board *b);
U64 verifyMove(int rank,int file, U4);
//----------------------------------Actual Setup of Game----------------------------------------//

Board setupBoard(void) {

    Board A = {
        // BOARD
        0x0000000000000000ULL,

        // WHITE PIECES
        0x000000000000FF00ULL, //WP
        0x0000000000000042ULL, //WN
        0x0000000000000024ULL, //WB
        0x0000000000000081ULL, //WR
        0x0000000000000008ULL, //WQ
        0x0000000000000010ULL, //WK

        // BLACK PIECES
        0x00FF000000000000ULL, //BP
        0x4200000000000000ULL, //BN
        0x2400000000000000ULL, //BB
        0x8100000000000000ULL, //BR
        0x0800000000000000ULL, //BQ
        0x1000000000000000ULL, //BK

    };

    A.ALLWHITE = A.WP | A.WN | A.WB | A.WR | A.WQ | A.WK;
    A.ALLBLACK = A.BP | A.BN | A.BB | A.BR | A.BQ | A.BK;
    A.OCCUPIED = A.ALLWHITE | A.ALLBLACK;
    A.EMPTY = ~A.OCCUPIED;
    return A;


}


void printBoard(Board *b){
    for (int rank=7 ; rank >= 0; rank--)
    {   
       
        printf("%d  ", rank + 1);
        for (int file = 0; file < 8; file++)
            {
            int sq = rank*8 +file;
            if (((b->OCCUPIED >> sq) & 1ULL) != 0)
                printf("1 ");
            else
                printf("0 ");
            }
        
         printf("\n");


    }

    printf("\n   A B C D E F G H");
}

// U64 ALL_WHITE = WP | WN | WB | WR | WQ | WK;
// U64 ALL_BLACK = BP | BN | BB | BR | BQ | BK;
// U64 OCUUPIED = ALL_WHITE | ALL_BLACK;
// U64 EMPTY = ~OCCUPIED

void printPiece(U64 a){
    for (int rank=7 ; rank >= 0; rank--)
        {   
        
            printf("%d  ", rank+1);
            for (int file = 0; file < 8; file++)
                {
                int sq = rank*8 +file;
                if (((a >> sq) & 1ULL) != 0)
                    printf("1 ");
                else
                    printf("0 ");
                }
            
            printf("\n");


        }

    printf("\n   A B C D E F G H\n\n");
}

void movePiece(Board *b,char *piece, char *dest) { // Todo: position of a piece, to move to a certain other position.
    // B2 to B3 | rank* width + file | B = file | 2 = rank 
    // getting initial position
    {
    int row = 0, col = 0; // initial
    int Drow = 0, Dcol = 0; // Dest
    
    U4 classifier; // classify piece and color
    U64 piece_position = translateMove(piece,&row,&col); //get A4 into binary
    U64 destination_position = translateMove(dest,&Drow,&Dcol);
    U64 *whichPiece = checkMove(b, piece_position,destination_position, &classifier); //classify and check whether its even a valid move, for example piece not even at that square.
    bool isBlack = classifier >> 3;
    U64 possible_moves = verifyMove(row, col, classifier); // check possible moves

    
    // This jst shows me which of the 64 bits is moving. but idk what type is it for example, idk if it is a knight or pond.
    if (whichPiece && possible_moves & destination_position) // Todo: add if black or white turn to move.
        {  
            *whichPiece ^= piece_position;
            *whichPiece |= destination_position;
            updateBoard(b);
            // printPiece(whichPiece);
        }
    
    }

}

U64 translateMove(char *p,int *rank, int *file){ //translate from A2 to binary

    int file_position = tolower(*p)- 'a';
    int rank_position = (atoi(p+1)-1);
    *file = file_position;
    *rank = rank_position;
    U64 piece_position = 1ULL << rank_position*8 + file_position;
    return piece_position;
}

U64 *checkMove(Board *b, U64 piece, U64 dest, U4 *pointer){ //check if piece even exists and determine what piece.
    if (b->OCCUPIED & piece) 
        if (b->ALLWHITE & piece) // IF WHITE PIECES // 0xxx //POND x000 /BISHOP x001 /KNIGHT x010 /ROOK x011 /QUEEN x100 /KING x101
           { if (b->WP & piece)
               { *pointer = pond | white;
                return &(b->WP);}
            else if (b->WN & piece)
                {*pointer = knight | white;
                return &(b->WN);}
            else if (b->WB & piece)
                {*pointer = bishop | white;
                return &(b->WB);}
            else if (b->WR & piece)
                {*pointer = rook | white;
                return &(b->WR);}
            else if (b->WQ & piece)
                {*pointer =  queen | white;
                return &(b->WQ);}
            else if (b->WK & piece)
                {*pointer = king | white;
                return &(b->WK);}}

        else if (b->ALLBLACK & piece) // IF BLACK PIECES  1xxx 
            {if (b->BP & piece)
                {*pointer = pond | black;
                return  &(b->BP);}
            else if (b->BN & piece) // change a string constant to "KNIGHT"
                {*pointer = knight | black;
                return &(b->BN);}
            else if (b->BB & piece)
                {*pointer = bishop | black;
                return &(b->BB);}
            else if (b->BR & piece)
                {*pointer = rook | black;
                return &(b->BR);}
            else if (b->BQ & piece)
                {*pointer = queen | black;
                return &(b->BQ);}
            else if (b->BK & piece)
                {*pointer = king | black;
                return &(b->BK);}}
}

void updateBoard(Board *A){

    A->ALLWHITE = A->WP | A->WN | A->WB | A->WR | A->WQ | A->WK;
    A->ALLBLACK = A->BP | A->BN | A->BB | A->BR | A->BQ | A->BK;
    A->OCCUPIED = A->ALLWHITE | A->ALLBLACK;
    A->EMPTY = ~A->OCCUPIED;
}

U64 verifyMove(int rank,int file,U4 piece){ // To calculate what possible moves are there. maybe might just need type of piece and rank & file
//knight possible formula :  Top L: <<8*2 +/- 1 || Top left/right L:  <<8+2 , << 8-2, || bot left/right: -2*-8
    U64 possible_moves = 0;


    //knight moves

    //todo: make moves for ther pieces
    // translate() sends rank and file to here
    // need find a way to identify piece type

    if ((piece << 1) == (pond << 1))  //pond
        {
            U64 mask = 0;
        // if at starting position
        if ((piece >> 3) == (white >> 3))
            {

            if (rank == 1)
                possible_moves = 1ULL << ((rank+2)*8 + file); //double step

            U64 mask = 1ULL << ((rank+1)*8 +file); //single step
            possible_moves |= mask;
            } 
        else
            {
            
            printf("%d %d\n",rank,file);
            if (rank == 6)
                possible_moves = 1ULL << ((rank-2)*8 + file);

            U64 mask = 1ULL << ((rank-1)*8 +file); //single step
            possible_moves |= mask;

            }
            
        printPiece(possible_moves);
        return possible_moves;

        }


    else if ((piece << 1) == (king << 1)) // king
    {
        //printf("%d\n",rank);
        for (int row = -1; row < 2; row ++)
        {  
            if (0 <= rank+row&&rank+row < 8)
           { 
            for (int col = -1; col < 2; col++)
                {
                    U64 mask = 0;
                    if (row == 0 && col == 0)
                        continue;
                    if (0 <= col+file && col+file < 8)
                    {
                        mask = 1ULL << ((rank+row)*8 + (file+col));
                        possible_moves |= mask;
                    }}
            }
        }
        printPiece(possible_moves);
        return possible_moves;

    }
    
    else if ((piece << 1) == (knight << 1)) //knight
    {
        for (int row = -1; row > -3; row--)
        {
            int col;
            if (row == -1)
                col = -2;
            else
                col = -1;
            
            int pRow = abs(row);
            int pCol = abs(col);

            if (0 <= rank + row && rank + row < 8 ) // check whether the moves are within the top and bot.  BOT
            {


                if (0 <= file + col && file + col < 8 ) //check whether moves is between the sides left side. BOT LEFT
                {
                    U64 mask = 0;
                    mask = 1ULL << (8*(rank+row) + (file+col));
                    possible_moves |= mask;
                }
                if (0 <= file + pCol && file + pCol< 8 )
                {
                    U64 mask = 0;
                    mask = 1ULL << (8*(rank+row) + (file+pCol));
                    possible_moves |= mask;
                }
            }

            if (0<= rank + pRow && rank + pRow <8)
            {
                if (0 <= file + col && file + col < 8 )
                {
                    U64 mask = 0;
                    mask = 1ULL << (8*(rank+pRow) + (file+col));
                    possible_moves |= mask;
                }
                if (0 <= file + pCol && file + pCol< 8 )
                {
                    U64 mask = 0;
                    mask = 1ULL << (8*(rank+pRow) + (file+pCol));
                    possible_moves |= mask;
                }
            }
        }
        printPiece(possible_moves);
        return possible_moves;

    }


    if ((piece << 1) == (rook << 1 ) || (piece << 1) == (queen << 1)) // rook or queen
    {
        for (int num = -1; num > -8; num--)
        {
            int pNum = abs(num);
            if (rank + num >=  0) //down
            {
                U64 mask =0;
                mask = 1ULL << ((rank+num)*8 + file);
                possible_moves |= mask;
            }

            if (file + pNum < 8) //right
            {
                U64 mask = 0;
                mask = 1ULL << (rank*8 + file+pNum);
                possible_moves |= mask;
            }

            if (rank + pNum < 8) //up
            {
                U64 mask =0;
                mask = 1ULL << ((rank+pNum)*8 + file);
                possible_moves |= mask;
            } 

            if (file + num >= 0) //left
            {
                U64 mask = 0;
                mask = 1ULL << (rank*8 + file+num);
                possible_moves |= mask;
            }
            
        }
        
    }


    if ((piece << 1) == (bishop << 1) || (piece << 1) == (queen << 1)) //bishop or queen
        {
            for (int row = -1; row > -9; row--)
            {
                int col = row;
                int pRow = abs(row);
                int pCol = abs(col);
                if (rank + row >= 0 && file + col >= 0)// bot left
                {
                    U64 mask = 0;
                    mask = 1ULL << ((rank+row)*8 + (col+file));
                    possible_moves |= mask;
                }
                if (rank + row >= 0 && file + pCol < 8)// bot right
                    {
                        U64 mask = 0;
                        mask = 1ULL << ((rank+row)*8 + (pCol+file));
                        possible_moves |= mask;
                    }

                    if (rank + pRow < 8 && file + pCol < 8)// top right
                    {
                        U64 mask = 0;
                        mask = 1ULL << ((rank+pRow)*8 + (pCol+file));
                        possible_moves |= mask;
                    }


                    if (rank + pRow < 8 && file + col >= 0)// top left
                    {
                        U64 mask = 0;
                        mask = 1ULL << ((rank+pRow)*8 + (col+file));
                        possible_moves |= mask;
                    }

            }
        }
    printPiece(possible_moves);
    return possible_moves;

}

int main(void) {


    Board A  = setupBoard();
    movePiece(&A, "h8", "b6");
    //printBoard(&A);
    return 0;
}



