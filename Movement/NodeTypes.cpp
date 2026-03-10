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

#include "NodeTypes.h"

#include "Move.h"
#include "Position.h"

namespace movement {

DivideRoot::DivideRoot(unsigned long long count,
                       const std::vector<std::shared_ptr<Node>>& children)
    : count_(count), children_(children) {}
void DivideRoot::doFormat(Position& position, std::ostream& output, int moveNo,
                          bool inlyne) const {
  for (const std::shared_ptr<Node>& child : children_) {
    child->doFormat(position, output, moveNo, false);
    output << "\n";
  }
  output << count_;
}

DivideLeaf::DivideLeaf(const std::shared_ptr<Move>& move,
                       unsigned long long count)
    : move_(move), count_(count) {}
void DivideLeaf::doFormat(Position& position, std::ostream& output, int moveNo,
                          bool inlyne) const {
  move_->make(position, std::nullopt, output);
  output << " " << count_;
  move_->unmake(position);
}

PerftNode::PerftNode(unsigned long long count) : count_(count) {}
void PerftNode::doFormat(Position& position, std::ostream& output, int moveNo,
                         bool inlyne) const {
  output << count_;
}

MateRoot::MateRoot(const std::vector<std::shared_ptr<Node>>& children)
    : children_(children) {}
void MateRoot::doFormat(Position& position, std::ostream& output, int moveNo,
                        bool inlyne) const {
  bool first = true;
  for (const std::shared_ptr<Node>& child : children_) {
    if (!first) {
      output << "\n";
    }
    child->doFormat(position, output, moveNo, false);
    first = false;
  }
}

MateBranch::MateBranch(const std::shared_ptr<Move>& move, int distance,
                       const std::vector<std::shared_ptr<Node>>& children)
    : move_(move), distance_(distance), children_(children) {}
int MateBranch::getDistance() const { return distance_; }
void MateBranch::doFormat(Position& position, std::ostream& output, int moveNo,
                          bool inlyne) const {
  if (position.blackToMove) {
    if (!inlyne) {
      output << moveNo << "...";
    }
  } else {
    output << moveNo << ".";
  }
  move_->make(position, std::nullopt, output);
  bool first = true;
  for (const std::shared_ptr<Node>& child : children_) {
    if (first) {
      output << " ";
    } else {
      output << "\n";
      for (int i = 0; i < (position.blackToMove ? moveNo - 1 : moveNo); ++i) {
        output << "\t";
      }
    }
    child->doFormat(position, output,
                    position.blackToMove ? moveNo : moveNo + 1, first);
    first = false;
  }
  move_->unmake(position);
}

MateLeaf::MateLeaf(const std::shared_ptr<Move>& move, int distance)
    : move_(move), distance_(distance) {}
int MateLeaf::getDistance() const { return distance_; }
void MateLeaf::doFormat(Position& position, std::ostream& output, int moveNo,
                        bool inlyne) const {
  move_->make(position, std::nullopt, output);
  output << " [#" << distance_ << "]";
  move_->unmake(position);
}

}  // namespace movement
