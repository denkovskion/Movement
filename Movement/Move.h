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

#pragma once
#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <ostream>
#include <set>
#include <vector>

namespace movement {

class Piece;
struct Position;

class Move {
  virtual bool preMake(Position& position) const = 0;
  virtual void updateBoard(std::array<std::unique_ptr<Piece>, 64>& board) = 0;
  virtual void revertBoard(std::array<std::unique_ptr<Piece>, 64>& board) = 0;
  virtual void updateCastlingOrigins(std::set<int>& castlingOrigins) const = 0;
  virtual void updateEnPassantTarget(
      std::optional<int>& enPassantTarget) const = 0;
  virtual void preWrite(const std::array<std::unique_ptr<Piece>, 64>& board,
                        std::ostream& lanBuilder) const = 0;

 public:
  virtual ~Move();
  bool make(
      Position& position,
      std::optional<std::reference_wrapper<std::vector<std::shared_ptr<Move>>>>
          pseudoLegalMoves,
      std::optional<std::reference_wrapper<std::ostream>> lanBuilder);
  void unmake(Position& position);
};

class NullMove : public Move {
  bool preMake(Position& position) const override;
  void updateBoard(std::array<std::unique_ptr<Piece>, 64>& board) override;
  void revertBoard(std::array<std::unique_ptr<Piece>, 64>& board) override;
  void updateCastlingOrigins(std::set<int>& castlingOrigins) const override;
  void updateEnPassantTarget(
      std::optional<int>& enPassantTarget) const override;
  void preWrite(const std::array<std::unique_ptr<Piece>, 64>& board,
                std::ostream& lanBuilder) const override;
};

}  // namespace movement
