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
#include "Move.h"

namespace movement {

class QuietMove : public NullMove {
  void updateBoard(std::array<std::unique_ptr<Piece>, 64>& board) override;
  void revertBoard(std::array<std::unique_ptr<Piece>, 64>& board) override;
  void updateCastlingOrigins(std::set<int>& castlingOrigins) const override;
  void preWrite(const std::array<std::unique_ptr<Piece>, 64>& board,
                std::ostream& lanBuilder) const override;

 protected:
  const int origin_;
  const int target_;

 public:
  QuietMove(int origin, int target);
};

class Capture : virtual public QuietMove {
  void updateBoard(std::array<std::unique_ptr<Piece>, 64>& board) override;
  void revertBoard(std::array<std::unique_ptr<Piece>, 64>& board) override;
  void updateCastlingOrigins(std::set<int>& castlingOrigins) const override;
  void preWrite(const std::array<std::unique_ptr<Piece>, 64>& board,
                std::ostream& lanBuilder) const override;

 protected:
  std::unique_ptr<Piece> captured_;

 public:
  Capture(int origin, int target);
};

class Castling : public QuietMove {
  bool preMake(Position& position) const override;
  void updateBoard(std::array<std::unique_ptr<Piece>, 64>& board) override;
  void revertBoard(std::array<std::unique_ptr<Piece>, 64>& board) override;
  void updateCastlingOrigins(std::set<int>& castlingOrigins) const override;

 protected:
  const int origin2_;
  const int target2_;

  Castling(int origin, int target, int origin2, int target2);
};

class LongCastling : public Castling {
  void preWrite(const std::array<std::unique_ptr<Piece>, 64>& board,
                std::ostream& lanBuilder) const override;

 public:
  LongCastling(int origin, int target, int origin2, int target2);
};

class ShortCastling : public Castling {
  void preWrite(const std::array<std::unique_ptr<Piece>, 64>& board,
                std::ostream& lanBuilder) const override;

 public:
  ShortCastling(int origin, int target, int origin2, int target2);
};

class DoubleStep : public QuietMove {
  void updateCastlingOrigins(std::set<int>& castlingOrigins) const override;
  void updateEnPassantTarget(
      std::optional<int>& enPassantTarget) const override;

 protected:
  const int stop_;

 public:
  DoubleStep(int origin, int target, int stop);
};

class EnPassant : public Capture {
  void updateBoard(std::array<std::unique_ptr<Piece>, 64>& board) override;
  void revertBoard(std::array<std::unique_ptr<Piece>, 64>& board) override;
  void updateCastlingOrigins(std::set<int>& castlingOrigins) const override;
  void preWrite(const std::array<std::unique_ptr<Piece>, 64>& board,
                std::ostream& lanBuilder) const override;

 protected:
  const int stop_;

 public:
  EnPassant(int origin, int target, int stop);
};

class Promotion : virtual public QuietMove {
  void updateBoard(std::array<std::unique_ptr<Piece>, 64>& board) override;
  void revertBoard(std::array<std::unique_ptr<Piece>, 64>& board) override;
  void updateCastlingOrigins(std::set<int>& castlingOrigins) const override;
  void preWrite(const std::array<std::unique_ptr<Piece>, 64>& board,
                std::ostream& lanBuilder) const override;

 protected:
  std::unique_ptr<Piece> promoted_;

 public:
  Promotion(int origin, int target, std::unique_ptr<Piece>& promoted);
};

class PromotionCapture : public Promotion, public Capture {
  void updateBoard(std::array<std::unique_ptr<Piece>, 64>& board) override;
  void revertBoard(std::array<std::unique_ptr<Piece>, 64>& board) override;
  void updateCastlingOrigins(std::set<int>& castlingOrigins) const override;
  void preWrite(const std::array<std::unique_ptr<Piece>, 64>& board,
                std::ostream& lanBuilder) const override;

 public:
  PromotionCapture(int origin, int target, std::unique_ptr<Piece>& promoted);
};

}  // namespace movement
