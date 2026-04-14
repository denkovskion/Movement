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
#include <set>
#include <string>
#include <vector>

namespace movement {

class Move;

class Piece {
  virtual int getType() const = 0;

 protected:
  const bool black_;

  Piece(bool black);

 public:
  virtual ~Piece();

  bool isBlack() const;
  virtual bool isRoyal() const;

  virtual bool generateMoves(
      int origin, const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::set<int>& castlingOrigins,
      const std::optional<int>& enPassantTarget,
      std::optional<std::reference_wrapper<std::vector<std::shared_ptr<Move>>>>
          moves) const = 0;

  virtual const char* getLanCode() const = 0;

  static void validate(const std::array<std::unique_ptr<Piece>, 128>& board,
                       bool blackToMove, const std::set<int>& castlingOrigins,
                       const std::optional<int>& enPassantTarget);
  static std::string toFormatted(
      const std::array<std::unique_ptr<Piece>, 128>& board, bool blackToMove,
      const std::set<int>& castlingOrigins,
      const std::optional<int>& enPassantTarget, const std::string& operation);
};

int isKingInCheck(
    const std::array<std::unique_ptr<Piece>, 128>& board, bool blackToMove,
    const std::set<int>& castlingOrigins,
    const std::optional<int>& enPassantTarget,
    std::optional<std::reference_wrapper<std::vector<std::shared_ptr<Move>>>>
        pseudoLegalMoves,
    bool count);
std::string toLanCode(int square);

}  // namespace movement
