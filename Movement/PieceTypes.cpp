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

#include "PieceTypes.h"

#include <sstream>
#include <stdexcept>

#include "MoveTypes.h"

namespace movement {

enum { KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN };

Leaper::Leaper(bool black) : Piece(black) {}

bool Leaper::doGenerateMoves(
    int origin, const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::set<int>& castlingOrigins,
    const std::optional<int>& enPassantTarget,
    std::optional<std::reference_wrapper<std::vector<std::shared_ptr<Move>>>>
        moves) const {
  const std::vector<int>& directions = getDirections();
  for (int direction : directions) {
    int target = origin + direction;
    if ((target & 136) == 0) {
      if (const std::unique_ptr<Piece>& captured = board[target]) {
        if (captured->isBlack() != black_) {
          if (captured->isRoyal()) {
            return false;
          } else {
            if (moves) {
              moves->get().push_back(std::make_shared<Capture>(origin, target));
            }
          }
        }
      } else {
        if (moves) {
          moves->get().push_back(std::make_shared<QuietMove>(origin, target));
        }
      }
    }
  }
  return true;
}

Rider::Rider(bool black) : Piece(black) {}

bool Rider::doGenerateMoves(
    int origin, const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::set<int>& castlingOrigins,
    const std::optional<int>& enPassantTarget,
    std::optional<std::reference_wrapper<std::vector<std::shared_ptr<Move>>>>
        moves) const {
  const std::vector<int>& directions = getDirections();
  for (int direction : directions) {
    for (int square = origin;;) {
      int target = square + direction;
      if ((target & 136) == 0) {
        if (const std::unique_ptr<Piece>& captured = board[target]) {
          if (captured->isBlack() != black_) {
            if (captured->isRoyal()) {
              return false;
            } else {
              if (moves) {
                moves->get().push_back(
                    std::make_shared<Capture>(origin, target));
              }
            }
          }
          break;
        } else {
          if (moves) {
            moves->get().push_back(std::make_shared<QuietMove>(origin, target));
          }
        }
        square = target;
      } else {
        break;
      }
    }
  }
  return true;
}

King::King(bool black) : Leaper(black) {}

bool King::isRoyal() const { return true; }

bool King::doGenerateMoves(
    int origin, const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::set<int>& castlingOrigins,
    const std::optional<int>& enPassantTarget,
    std::optional<std::reference_wrapper<std::vector<std::shared_ptr<Move>>>>
        moves) const {
  if (!Leaper::doGenerateMoves(origin, board, castlingOrigins, enPassantTarget,
                               moves)) {
    return false;
  }
  if (castlingOrigins.count(origin) != 0) {
    const int castlingDirections[] = {-16, 16};
    for (int direction : castlingDirections) {
      int target2 = origin + direction;
      if (!board[target2]) {
        int target = target2 + direction;
        if (!board[target]) {
          if (direction > 0) {
            int origin2 = target + direction;
            if (castlingOrigins.count(origin2) != 0) {
              if (moves) {
                moves->get().push_back(std::make_shared<ShortCastling>(
                    origin, target, origin2, target2));
              }
            }
          } else {
            int stop = target + direction;
            if (!board[stop]) {
              int origin2 = stop + direction;
              if (castlingOrigins.count(origin2) != 0) {
                if (moves) {
                  moves->get().push_back(std::make_shared<LongCastling>(
                      origin, target, origin2, target2));
                }
              }
            }
          }
        }
      }
    }
  }
  return true;
}

const std::vector<int>& King::getDirections() const {
  static const std::vector<int> directions = {-17, -16, -15, -1, 1, 15, 16, 17};
  return directions;
}

const char* King::getLanCode() const { return "K"; }

int King::getType() const { return KING; }

Queen::Queen(bool black) : Rider(black) {}

const std::vector<int>& Queen::getDirections() const {
  static const std::vector<int> directions = {-17, -16, -15, -1, 1, 15, 16, 17};
  return directions;
}

const char* Queen::getLanCode() const { return "Q"; }

int Queen::getType() const { return QUEEN; }

Rook::Rook(bool black) : Rider(black) {}

const std::vector<int>& Rook::getDirections() const {
  static const std::vector<int> directions = {-16, -1, 1, 16};
  return directions;
}

const char* Rook::getLanCode() const { return "R"; }

int Rook::getType() const { return ROOK; }

Bishop::Bishop(bool black) : Rider(black) {}

const std::vector<int>& Bishop::getDirections() const {
  static const std::vector<int> directions = {-17, -15, 15, 17};
  return directions;
}

const char* Bishop::getLanCode() const { return "B"; }

int Bishop::getType() const { return BISHOP; }

Knight::Knight(bool black) : Leaper(black) {}

const std::vector<int>& Knight::getDirections() const {
  static const std::vector<int> directions = {-33, -31, -18, -14,
                                              14,  18,  31,  33};
  return directions;
}

const char* Knight::getLanCode() const { return "N"; }

int Knight::getType() const { return KNIGHT; }

Pawn::Pawn(bool black) : Piece(black) {}

bool Pawn::doGenerateMoves(
    int origin, const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::set<int>& castlingOrigins,
    const std::optional<int>& enPassantTarget,
    std::optional<std::reference_wrapper<std::vector<std::shared_ptr<Move>>>>
        moves) const {
  int captureDirections[] = {black_ ? -17 : -15, black_ ? 15 : 17};
  for (int direction : captureDirections) {
    int target = origin + direction;
    if ((target & 136) == 0) {
      if (const std::unique_ptr<Piece>& captured = board[target]) {
        if (captured->isBlack() != black_) {
          if (captured->isRoyal()) {
            return false;
          } else {
            if (origin % 16 == (black_ ? 1 : 6)) {
              std::unique_ptr<Piece> pieces[] = {
                  std::make_unique<Queen>(black_),
                  std::make_unique<Rook>(black_),
                  std::make_unique<Bishop>(black_),
                  std::make_unique<Knight>(black_)};
              for (std::unique_ptr<Piece>& promoted : pieces) {
                if (moves) {
                  moves->get().push_back(std::make_shared<PromotionCapture>(
                      origin, target, promoted));
                }
              }
            } else {
              if (moves) {
                moves->get().push_back(
                    std::make_shared<Capture>(origin, target));
              }
            }
          }
        }
      } else {
        if (enPassantTarget) {
          if (target == enPassantTarget.value()) {
            int stop = (target / 16) * 16 + origin % 16;
            if (moves) {
              moves->get().push_back(
                  std::make_shared<EnPassant>(origin, target, stop));
            }
          }
        }
      }
    }
  }
  int direction = black_ ? -1 : 1;
  int target = origin + direction;
  if ((target & 136) == 0) {
    if (!board[target]) {
      if (origin % 16 == (black_ ? 1 : 6)) {
        std::unique_ptr<Piece> pieces[] = {
            std::make_unique<Queen>(black_), std::make_unique<Rook>(black_),
            std::make_unique<Bishop>(black_), std::make_unique<Knight>(black_)};
        for (std::unique_ptr<Piece>& promoted : pieces) {
          if (moves) {
            moves->get().push_back(
                std::make_shared<Promotion>(origin, target, promoted));
          }
        }
      } else {
        if (moves) {
          moves->get().push_back(std::make_shared<QuietMove>(origin, target));
        }
        if (origin % 16 == (black_ ? 6 : 1)) {
          int target2 = target + direction;
          if (!board[target2]) {
            if (moves) {
              moves->get().push_back(
                  std::make_shared<DoubleStep>(origin, target2, target));
            }
          }
        }
      }
    }
  }
  return true;
}

const char* Pawn::getLanCode() const { return ""; }

int Pawn::getType() const { return PAWN; }

void Piece::validate(const std::array<std::unique_ptr<Piece>, 128>& board,
                     bool blackToMove, const std::set<int>& castlingOrigins,
                     const std::optional<int>& enPassantTarget) {
  for (bool black : {false, true}) {
    int frequency = 0;
    for (const std::unique_ptr<Piece>& piece : board) {
      if (piece && piece->black_ == black && piece->getType() == KING) {
        ++frequency;
      }
    }
    if (!(frequency == 1)) {
      throw std::invalid_argument("Not accepted number of kings");
    }
  }
  for (int castlingOrigin : castlingOrigins) {
    int file = castlingOrigin / 16;
    int rank = castlingOrigin % 16;
    if (const std::unique_ptr<Piece>& piece = board[castlingOrigin];
        !(piece &&
          (file == 4 && piece->getType() == KING ||
           (file == 0 || file == 7) && piece->getType() == ROOK) &&
          (rank == 0 && !piece->black_ || rank == 7 && piece->black_))) {
      throw std::invalid_argument("Not accepted castling rights");
    }
  }
  if (enPassantTarget) {
    int doubleStepOrigin = enPassantTarget.value() + (blackToMove ? -1 : 1);
    int doubleStepTarget = enPassantTarget.value() + (blackToMove ? 1 : -1);
    int doubleStepStop = enPassantTarget.value();
    if (!(doubleStepStop % 16 == (blackToMove ? 2 : 5) &&
          !board[doubleStepOrigin] && !board[doubleStepStop] &&
          board[doubleStepTarget] &&
          board[doubleStepTarget]->black_ != blackToMove &&
          board[doubleStepTarget]->getType() == PAWN)) {
      throw std::invalid_argument("Not accepted en passant square");
    }
  }
}

std::string Piece::toFormatted(
    const std::array<std::unique_ptr<Piece>, 128>& board, bool blackToMove,
    const std::set<int>& castlingOrigins,
    const std::optional<int>& enPassantTarget, const std::string& operation) {
  std::stringstream output;
  for (int rank = 7; rank >= 0; --rank) {
    output << (rank + 1);
    for (int file = 0; file <= 7; ++file) {
      int square = file * 16 + rank;
      const std::unique_ptr<Piece>& piece = board[square];
      output << ' ';
      if (piece) {
        if (piece->getType() == KING) {
          output << (piece->black_ ? 'k' : 'K');
        } else if (piece->getType() == QUEEN) {
          output << (piece->black_ ? 'q' : 'Q');
        } else if (piece->getType() == ROOK) {
          output << (piece->black_ ? 'r' : 'R');
        } else if (piece->getType() == BISHOP) {
          output << (piece->black_ ? 'b' : 'B');
        } else if (piece->getType() == KNIGHT) {
          output << (piece->black_ ? 'n' : 'N');
        } else if (piece->getType() == PAWN) {
          output << (piece->black_ ? 'p' : 'P');
        }
      } else {
        output << '.';
      }
    }
    if (rank == 7) {
      output << "    Side to move: " << (blackToMove ? 'b' : 'w');
    } else if (rank == 6) {
      output << "    Castling rights: ";
      if (!castlingOrigins.empty()) {
        if (castlingOrigins.count(64) != 0) {
          if (castlingOrigins.count(112) != 0) {
            output << 'K';
          }
          if (castlingOrigins.count(0) != 0) {
            output << 'Q';
          }
        }
        if (castlingOrigins.count(71) != 0) {
          if (castlingOrigins.count(119) != 0) {
            output << 'k';
          }
          if (castlingOrigins.count(7) != 0) {
            output << 'q';
          }
        }
      } else {
        output << '-';
      }
    } else if (rank == 5) {
      output << "    En passant target: ";
      if (enPassantTarget) {
        output << std::string()
                      .append(1, 'a' + enPassantTarget.value() / 16)
                      .append(1, '1' + enPassantTarget.value() % 16);
      } else {
        output << '-';
      }
    } else if (rank == 3) {
      output << "    " << operation;
    }
    output << '\n';
  }
  output << ' ';
  for (char file = 'a'; file <= 'h'; ++file) {
    output << ' ' << file;
  }
  return output.str();
}

int generateMoves(
    const std::array<std::unique_ptr<Piece>, 128>& board, bool blackToMove,
    const std::set<int>& castlingOrigins,
    const std::optional<int>& enPassantTarget,
    std::optional<std::reference_wrapper<std::vector<std::shared_ptr<Move>>>>
        pseudoLegalMoves,
    bool count) {
  int nChecks = 0;
  for (int origin = 0; origin < 128; ++origin) {
    if ((origin & 136) == 0) {
      if (const std::unique_ptr<Piece>& piece = board[origin]) {
        if (piece->isBlack() == blackToMove) {
          if (!piece->doGenerateMoves(origin, board, castlingOrigins,
                                      enPassantTarget, pseudoLegalMoves)) {
            if (count) {
              ++nChecks;
            } else {
              return 0;
            }
          }
        }
      }
    }
  }
  return nChecks == 0 ? 1 : -nChecks;
}

std::string toLanCode(int square) {
  return std::string()
      .append(1, 'a' + square / 16)
      .append(1, '1' + square % 16);
}

}  // namespace movement
