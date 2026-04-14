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

#include "MoveTypes.h"

#include "Piece.h"

namespace movement {

QuietMove::QuietMove(int origin, int target)
    : origin_(origin), target_(target) {}

void QuietMove::updateBoard(std::array<std::unique_ptr<Piece>, 128>& board) {
  board[target_] = std::move(board[origin_]);
}

void QuietMove::revertBoard(std::array<std::unique_ptr<Piece>, 128>& board) {
  board[origin_] = std::move(board[target_]);
}

void QuietMove::updateCastlingOrigins(std::set<int>& castlingOrigins) const {
  castlingOrigins.erase(origin_);
}

void QuietMove::preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                         std::ostream& lanBuilder) const {
  lanBuilder << board[origin_]->getLanCode() << toLanCode(origin_) << "-"
             << toLanCode(target_);
}

Capture::Capture(int origin, int target) : QuietMove(origin, target) {}

void Capture::updateBoard(std::array<std::unique_ptr<Piece>, 128>& board) {
  captured_ = std::move(board[target_]);
  board[target_] = std::move(board[origin_]);
}

void Capture::revertBoard(std::array<std::unique_ptr<Piece>, 128>& board) {
  board[origin_] = std::move(board[target_]);
  board[target_] = std::move(captured_);
}

void Capture::updateCastlingOrigins(std::set<int>& castlingOrigins) const {
  castlingOrigins.erase(origin_);
  castlingOrigins.erase(target_);
}

void Capture::preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                       std::ostream& lanBuilder) const {
  lanBuilder << board[origin_]->getLanCode() << toLanCode(origin_) << "x"
             << toLanCode(target_);
}

Castling::Castling(int origin, int target, int origin2, int target2)
    : QuietMove(origin, target), origin2_(origin2), target2_(target2) {}

bool Castling::preMake(Position& position) const {
  std::shared_ptr<Move> nullMove = std::make_shared<NullMove>();
  bool result = nullMove->make(position, std::nullopt, std::nullopt);
  nullMove->unmake(position);
  if (result) {
    std::shared_ptr<Move> quietMove =
        std::make_shared<QuietMove>(origin_, target2_);
    result = quietMove->make(position, std::nullopt, std::nullopt);
    quietMove->unmake(position);
  }
  return result;
}

void Castling::updateBoard(std::array<std::unique_ptr<Piece>, 128>& board) {
  board[target_] = std::move(board[origin_]);
  board[target2_] = std::move(board[origin2_]);
}

void Castling::revertBoard(std::array<std::unique_ptr<Piece>, 128>& board) {
  board[origin2_] = std::move(board[target2_]);
  board[origin_] = std::move(board[target_]);
}

void Castling::updateCastlingOrigins(std::set<int>& castlingOrigins) const {
  castlingOrigins.erase(origin_);
  castlingOrigins.erase(origin2_);
}

LongCastling::LongCastling(int origin, int target, int origin2, int target2)
    : Castling(origin, target, origin2, target2) {}

void LongCastling::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    std::ostream& lanBuilder) const {
  lanBuilder << "0-0-0";
}

ShortCastling::ShortCastling(int origin, int target, int origin2, int target2)
    : Castling(origin, target, origin2, target2) {}

void ShortCastling::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    std::ostream& lanBuilder) const {
  lanBuilder << "0-0";
}

DoubleStep::DoubleStep(int origin, int target, int stop)
    : QuietMove(origin, target), stop_(stop) {}

void DoubleStep::updateCastlingOrigins(std::set<int>& castlingOrigins) const {}

void DoubleStep::updateEnPassantTarget(
    std::optional<int>& enPassantTarget) const {
  enPassantTarget = stop_;
}

EnPassant::EnPassant(int origin, int target, int stop)
    : QuietMove(origin, target), Capture(origin, target), stop_(stop) {}

void EnPassant::updateBoard(std::array<std::unique_ptr<Piece>, 128>& board) {
  captured_ = std::move(board[stop_]);
  board[target_] = std::move(board[origin_]);
}

void EnPassant::revertBoard(std::array<std::unique_ptr<Piece>, 128>& board) {
  board[origin_] = std::move(board[target_]);
  board[stop_] = std::move(captured_);
}

void EnPassant::updateCastlingOrigins(std::set<int>& castlingOrigins) const {}

void EnPassant::preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                         std::ostream& lanBuilder) const {
  lanBuilder << board[origin_]->getLanCode() << toLanCode(origin_) << "x"
             << toLanCode(target_) << " e.p.";
}

Promotion::Promotion(int origin, int target, std::unique_ptr<Piece>& promoted)
    : QuietMove(origin, target), promoted_(std::move(promoted)) {}

void Promotion::updateBoard(std::array<std::unique_ptr<Piece>, 128>& board) {
  board[target_] = std::move(promoted_);
  promoted_ = std::move(board[origin_]);
}

void Promotion::revertBoard(std::array<std::unique_ptr<Piece>, 128>& board) {
  board[origin_] = std::move(promoted_);
  promoted_ = std::move(board[target_]);
}

void Promotion::updateCastlingOrigins(std::set<int>& castlingOrigins) const {}

void Promotion::preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                         std::ostream& lanBuilder) const {
  lanBuilder << board[origin_]->getLanCode() << toLanCode(origin_) << "-"
             << toLanCode(target_) << "=" << promoted_->getLanCode();
}

PromotionCapture::PromotionCapture(int origin, int target,
                                   std::unique_ptr<Piece>& promoted)
    : QuietMove(origin, target),
      Promotion(origin, target, promoted),
      Capture(origin, target) {}

void PromotionCapture::updateBoard(
    std::array<std::unique_ptr<Piece>, 128>& board) {
  captured_ = std::move(board[target_]);
  board[target_] = std::move(promoted_);
  promoted_ = std::move(board[origin_]);
}

void PromotionCapture::revertBoard(
    std::array<std::unique_ptr<Piece>, 128>& board) {
  board[origin_] = std::move(promoted_);
  promoted_ = std::move(board[target_]);
  board[target_] = std::move(captured_);
}

void PromotionCapture::updateCastlingOrigins(
    std::set<int>& castlingOrigins) const {
  castlingOrigins.erase(target_);
}

void PromotionCapture::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    std::ostream& lanBuilder) const {
  lanBuilder << board[origin_]->getLanCode() << toLanCode(origin_) << "x"
             << toLanCode(target_) << "=" << promoted_->getLanCode();
}

}  // namespace movement
