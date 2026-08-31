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

#include <sstream>

#include "Move.h"
#include "Position.h"

namespace movement {

DivideRoot::DivideRoot(unsigned long long count,
                       const std::vector<std::shared_ptr<Node>>& children)
    : count_(count), children_(children) {}

std::string DivideRoot::toFormattedString(Position& position, int moveNo,
                                          bool inlyne) const {
  std::ostringstream output;
  for (const std::shared_ptr<Node>& child : children_) {
    output << child->toFormattedString(position, moveNo, false);
    output << "\n";
  }
  output << count_;
  return output.str();
}

DivideLeaf::DivideLeaf(const std::shared_ptr<Move>& move,
                       unsigned long long count)
    : move_(move), count_(count) {}

std::string DivideLeaf::toFormattedString(Position& position, int moveNo,
                                          bool inlyne) const {
  std::ostringstream output;
  move_->make(position, std::nullopt, output);
  output << " " << count_;
  move_->unmake(position);
  return output.str();
}

PerftNode::PerftNode(unsigned long long count) : count_(count) {}

std::string PerftNode::toFormattedString(Position& position, int moveNo,
                                         bool inlyne) const {
  return std::to_string(count_);
}

MateRoot::MateRoot(const std::vector<std::shared_ptr<Node>>& children)
    : children_(children) {}

std::string MateRoot::toFormattedString(Position& position, int moveNo,
                                        bool inlyne) const {
  std::ostringstream output;
  bool first = true;
  for (const std::shared_ptr<Node>& child : children_) {
    if (!first) {
      output << "\n";
    }
    output << child->toFormattedString(position, moveNo, false);
    first = false;
  }
  return output.str();
}

MateBranch::MateBranch(const std::shared_ptr<Move>& move, int distance,
                       const std::vector<std::shared_ptr<Node>>& children)
    : move_(move), distance_(distance), children_(children) {}

int MateBranch::getDistance() const { return distance_; }

std::string MateBranch::toFormattedString(Position& position, int moveNo,
                                          bool inlyne) const {
  std::ostringstream output;
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
      output << "\n"
             << std::string(position.blackToMove ? moveNo - 1 : moveNo, '\t');
    }
    output << child->toFormattedString(
        position, position.blackToMove ? moveNo : moveNo + 1, first);
    first = false;
  }
  move_->unmake(position);
  return output.str();
}

MateLeaf::MateLeaf(const std::shared_ptr<Move>& move, int distance)
    : move_(move), distance_(distance) {}

int MateLeaf::getDistance() const { return distance_; }

std::string MateLeaf::toFormattedString(Position& position, int moveNo,
                                        bool inlyne) const {
  std::ostringstream output;
  move_->make(position, std::nullopt, output);
  output << " [#" << distance_ << "]";
  move_->unmake(position);
  return output.str();
}

}  // namespace movement
