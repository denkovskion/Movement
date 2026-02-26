/*
 * MIT License
 *
 * Copyright (c) 2026 Ivan Denkovski
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Move.h"

#include "Position.h"

namespace movement {

void postWrite(
    Position& position,
    std::optional<
        std::reference_wrapper<const std::vector<std::shared_ptr<Move>>>>
        pseudoLegalMoves,
    std::ostream& lanBuilder);

Move::~Move() {}

bool Move::make(
    Position& position,
    std::optional<std::reference_wrapper<std::vector<std::shared_ptr<Move>>>>
        pseudoLegalMoves,
    std::optional<std::reference_wrapper<std::ostream>> lanBuilder) {
  if (lanBuilder) {
    preWrite(position.board, lanBuilder->get());
  }
  bool legal = preMake(position);
  updateBoard(position.board);
  position.memory.push_back(
      {position.castlingOrigins, position.enPassantTarget});
  updateCastlingOrigins(position.castlingOrigins);
  updateEnPassantTarget(position.enPassantTarget);
  position.blackToMove = !position.blackToMove;
  if (legal) {
    legal = isPositionLegal(position, pseudoLegalMoves);
  }
  if (lanBuilder) {
    if (legal) {
      postWrite(position, pseudoLegalMoves, lanBuilder->get());
    }
  }
  return legal;
}

void Move::unmake(Position& position) {
  position.blackToMove = !position.blackToMove;
  std::tie(position.castlingOrigins, position.enPassantTarget) =
      position.memory.back();
  position.memory.pop_back();
  revertBoard(position.board);
}

bool NullMove::preMake(Position& position) const { return true; }
void NullMove::updateBoard(std::array<std::unique_ptr<Piece>, 64>& board) {}
void NullMove::revertBoard(std::array<std::unique_ptr<Piece>, 64>& board) {}
void NullMove::updateCastlingOrigins(std::set<int>& castlingOrigins) const {}
void NullMove::updateEnPassantTarget(
    std::optional<int>& enPassantTarget) const {
  enPassantTarget.reset();
}
void NullMove::preWrite(const std::array<std::unique_ptr<Piece>, 64>& board,
                        std::ostream& lanBuilder) const {
  lanBuilder << "null";
}

void postWrite(
    Position& position,
    std::optional<
        std::reference_wrapper<const std::vector<std::shared_ptr<Move>>>>
        pseudoLegalMoves,
    std::ostream& lanBuilder) {
  std::vector<std::shared_ptr<Move>> pseudoLegalMovesNext;
  if (pseudoLegalMoves) {
    pseudoLegalMovesNext = pseudoLegalMoves->get();
  } else {
    for (int origin = 0; origin < 64; ++origin) {
      if (const std::unique_ptr<Piece>& piece = position.board[origin]) {
        if (piece->isBlack() == position.blackToMove) {
          piece->generateMoves(origin, position.board, position.castlingOrigins,
                               position.enPassantTarget, pseudoLegalMovesNext);
        }
      }
    }
  }
  bool terminal = true;
  for (const std::shared_ptr<Move>& moveNext : pseudoLegalMovesNext) {
    if (moveNext->make(position, std::nullopt, std::nullopt)) {
      terminal = false;
    }
    moveNext->unmake(position);
    if (!terminal) {
      break;
    }
  }
  int nChecks = 0;
  std::shared_ptr<Move> nullMove = std::make_shared<NullMove>();
  nullMove->make(position, std::nullopt, std::nullopt);
  for (int origin = 0; origin < 64; ++origin) {
    if (const std::unique_ptr<Piece>& piece = position.board[origin]) {
      if (piece->isBlack() == position.blackToMove) {
        if (!piece->generateMoves(origin, position.board,
                                  position.castlingOrigins,
                                  position.enPassantTarget, std::nullopt)) {
          ++nChecks;
        }
      }
    }
  }
  nullMove->unmake(position);
  if (terminal) {
    if (nChecks > 0) {
      if (nChecks > 1) {
        for (int i = 0; i < nChecks; ++i) {
          lanBuilder << "+";
        }
      }
      lanBuilder << "#";
    } else {
      lanBuilder << "=";
    }
  } else {
    if (nChecks > 0) {
      for (int i = 0; i < nChecks; ++i) {
        lanBuilder << "+";
      }
    }
  }
}

}  // namespace movement
