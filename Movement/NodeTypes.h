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

#include <vector>

#include "Node.h"

namespace movement {

class Move;

class DivideRoot : public Node {
  const unsigned long long count_;
  const std::vector<std::shared_ptr<Node>> children_;

 public:
  DivideRoot(unsigned long long count,
             const std::vector<std::shared_ptr<Node>>& children);

  void doFormat(Position& position, std::ostream& output, int moveNo,
                bool inlyne) const override;
};

class DivideLeaf : public Node {
  const std::shared_ptr<Move> move_;
  const unsigned long long count_;

 public:
  DivideLeaf(const std::shared_ptr<Move>& move, unsigned long long count);

  void doFormat(Position& position, std::ostream& output, int moveNo,
                bool inlyne) const override;
};

class PerftNode : public Node {
  const unsigned long long count_;

 public:
  PerftNode(unsigned long long count);

  void doFormat(Position& position, std::ostream& output, int moveNo,
                bool inlyne) const override;
};

class MateRoot : public Node {
  const std::vector<std::shared_ptr<Node>> children_;

 public:
  MateRoot(const std::vector<std::shared_ptr<Node>>& children);

  void doFormat(Position& position, std::ostream& output, int moveNo,
                bool inlyne) const override;
};

class MateBranch : public Node {
  const std::shared_ptr<Move> move_;
  const int distance_;
  const std::vector<std::shared_ptr<Node>> children_;

 public:
  MateBranch(const std::shared_ptr<Move>& move, int distance,
             const std::vector<std::shared_ptr<Node>>& children);

  int getDistance() const;

  void doFormat(Position& position, std::ostream& output, int moveNo,
                bool inlyne) const override;
};

class MateLeaf : public Node {
  const std::shared_ptr<Move> move_;
  const int distance_;

 public:
  MateLeaf(const std::shared_ptr<Move>& move_, int distance_);

  int getDistance() const;

  void doFormat(Position& position, std::ostream& output, int moveNo,
                bool inlyne) const override;
};

}  // namespace movement
