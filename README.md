Chess Engine

A chess engine built from scratch, without any AI or machine learning, using bitboards.

About

This project is my attempt to build a chess engine the classic way. There are no neural networks and no trained models. It relies on traditional programming techniques and a fast board representation.

Why bitboards?

A bitboard stores the chess board as 64-bit integers, where each bit represents one square. Instead of looping over an 8x8 array, the engine can use fast bitwise operations (AND, OR, XOR, shifts) to:
1) Generate moves
2) Detect attacks and checks
3) Evaluate positions
4) Each piece type and colour gets its own bitboard (for example, white pawns, black knights), which keeps things compact and quick.

Goals
1) Represent the board using bitboards
2) Generate legal moves
3 Search for the best move
4) Evaluate positions with hand-written rules

Status
Work in progress.

Disclaimer
No AI is used in the engine itself. The goal is to learn how chess engines work from the ground up. But this readme is AI-generated.
