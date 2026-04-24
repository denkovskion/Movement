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

#include "Piece.h"

namespace movement {

class Leaper : public Piece {
  virtual const std::vector<int>& getDirections() const = 0;

 protected:
  Leaper(bool black);

  bool doGenerateMoves(
      int origin, const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::set<int>& castlingOrigins,
      const std::optional<int>& enPassantTarget,
      std::optional<std::reference_wrapper<std::vector<std::shared_ptr<Move>>>>
          moves) const override;
};

class Rider : public Piece {
  virtual const std::vector<int>& getDirections() const = 0;

 protected:
  Rider(bool black);

  bool doGenerateMoves(
      int origin, const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::set<int>& castlingOrigins,
      const std::optional<int>& enPassantTarget,
      std::optional<std::reference_wrapper<std::vector<std::shared_ptr<Move>>>>
          moves) const override;
};

class King final : public Leaper {
  int getType() const override;

  const std::vector<int>& getDirections() const override;

 public:
  King(bool black);

  bool isRoyal() const override;

  bool doGenerateMoves(
      int origin, const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::set<int>& castlingOrigins,
      const std::optional<int>& enPassantTarget,
      std::optional<std::reference_wrapper<std::vector<std::shared_ptr<Move>>>>
          moves) const override;

  const char* getLanCode() const override;
};

class Queen final : public Rider {
  int getType() const override;

  const std::vector<int>& getDirections() const override;

 public:
  Queen(bool black);

  const char* getLanCode() const override;
};

class Rook final : public Rider {
  int getType() const override;

  const std::vector<int>& getDirections() const override;

 public:
  Rook(bool black);

  const char* getLanCode() const override;
};

class Bishop final : public Rider {
  int getType() const override;

  const std::vector<int>& getDirections() const override;

 public:
  Bishop(bool black);

  const char* getLanCode() const override;
};

class Knight final : public Leaper {
  int getType() const override;

  const std::vector<int>& getDirections() const override;

 public:
  Knight(bool black);

  const char* getLanCode() const override;
};

class Pawn final : public Piece {
  int getType() const override;

 public:
  Pawn(bool black);

  bool doGenerateMoves(
      int origin, const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::set<int>& castlingOrigins,
      const std::optional<int>& enPassantTarget,
      std::optional<std::reference_wrapper<std::vector<std::shared_ptr<Move>>>>
          moves) const override;

  const char* getLanCode() const override;
};

}  // namespace movement
