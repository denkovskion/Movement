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

#include "ProblemTypes.h"

#include <algorithm>

#include "Move.h"
#include "NodeTypes.h"

namespace movement {

long count(
    int nPlies, Position& position,
    const std::vector<std::shared_ptr<Move>>& pseudoLegalMoves,
    std::optional<std::reference_wrapper<std::vector<std::shared_ptr<Node>>>>
        nodes);

Perft::Perft(Position position, int analysisDepth)
    : Problem(std::move(position)), nPlies_(analysisDepth) {}
std::string Perft::getOperation() const {
  return "Perft at depth " + std::to_string(nPlies_);
}
std::shared_ptr<Node> Perft::doSolve(
    const std::vector<std::shared_ptr<Move>>& pseudoLegalMoves, bool detailed) {
  if (detailed) {
    std::vector<std::shared_ptr<Node>> nodes;
    long nNodes = count(nPlies_, position_, pseudoLegalMoves, nodes);
    return std::make_shared<DivideRoot>(nNodes, nodes);
  } else {
    long nNodes = count(nPlies_, position_, pseudoLegalMoves, std::nullopt);
    return std::make_shared<PerftNode>(nNodes);
  }
}

long count(
    int nPlies, Position& position,
    const std::vector<std::shared_ptr<Move>>& pseudoLegalMoves,
    std::optional<std::reference_wrapper<std::vector<std::shared_ptr<Node>>>>
        nodes) {
  if (nPlies == 0) {
    return 1;
  }
  long nNodes = 0;
  for (const std::shared_ptr<Move>& move : pseudoLegalMoves) {
    if (std::vector<std::shared_ptr<Move>> pseudoLegalMovesNext;
        move->make(position, pseudoLegalMovesNext, std::nullopt)) {
      long nChildNodes =
          count(nPlies - 1, position, pseudoLegalMovesNext, std::nullopt);
      if (nodes) {
        nodes->get().push_back(std::make_shared<DivideLeaf>(move, nChildNodes));
      }
      nNodes += nChildNodes;
    }
    move->unmake(position);
  }
  return nNodes;
}

std::vector<std::shared_ptr<Node>> analyse(
    int nMoves, Position& position,
    std::vector<std::shared_ptr<Move>> pseudoLegalMoves);
int searchMax(int nMoves, Position& position,
              const std::vector<std::shared_ptr<Move>>& pseudoLegalMovesMax);
int searchMin(int nMoves, Position& position,
              const std::vector<std::shared_ptr<Move>>& pseudoLegalMovesMin);

MateSearch::MateSearch(Position position, int searchDepth)
    : Problem(std::move(position)), nMoves_(searchDepth) {}
std::string MateSearch::getOperation() const {
  return "Mate in " + std::to_string(nMoves_);
}
std::shared_ptr<Node> MateSearch::doSolve(
    const std::vector<std::shared_ptr<Move>>& pseudoLegalMoves, bool detailed) {
  if (detailed) {
    std::vector<std::shared_ptr<Node>> nodes =
        analyse(nMoves_, position_, pseudoLegalMoves);
    return std::make_shared<MateRoot>(nodes);
  } else {
    std::vector<std::shared_ptr<Node>> nodes;
    for (std::shared_ptr<Move> moveMax : pseudoLegalMoves) {
      if (std::vector<std::shared_ptr<Move>> pseudoLegalMovesMin;
          moveMax->make(position_, pseudoLegalMovesMin, std::nullopt)) {
        int min = searchMin(nMoves_, position_, pseudoLegalMovesMin);
        if (min > 0) {
          int distanceMax = nMoves_ - min + 1;
          nodes.push_back(std::make_shared<MateLeaf>(moveMax, distanceMax));
        }
      }
      moveMax->unmake(position_);
    }
    std::stable_sort(
        nodes.begin(), nodes.end(),
        [](const std::shared_ptr<Node>& node1,
           const std::shared_ptr<Node>& node2) {
          return std::static_pointer_cast<MateLeaf>(node1)->getDistance() <
                 std::static_pointer_cast<MateLeaf>(node2)->getDistance();
        });
    return std::make_shared<MateRoot>(nodes);
  }
}

std::vector<std::shared_ptr<Node>> analyse(
    int nMoves, Position& position,
    std::vector<std::shared_ptr<Move>> pseudoLegalMoves) {
  std::vector<std::shared_ptr<Node>> nodes;
  for (std::shared_ptr<Move> moveMax : pseudoLegalMoves) {
    if (std::vector<std::shared_ptr<Move>> pseudoLegalMovesMin;
        moveMax->make(position, pseudoLegalMovesMin, std::nullopt)) {
      int min = searchMin(nMoves, position, pseudoLegalMovesMin);
      if (min > 0) {
        int distanceMax = nMoves - min + 1;
        std::vector<std::shared_ptr<Node>> nodesMin;
        for (std::shared_ptr<Move> moveMin : pseudoLegalMovesMin) {
          if (std::vector<std::shared_ptr<Move>> pseudoLegalMovesMax;
              moveMin->make(position, pseudoLegalMovesMax, std::nullopt)) {
            int max = searchMax(distanceMax - 1, position, pseudoLegalMovesMax);
            int distanceMin = distanceMax - max;
            std::vector<std::shared_ptr<Node>> nodesMax =
                analyse(distanceMin, position, pseudoLegalMovesMax);
            nodesMin.push_back(
                std::make_shared<MateBranch>(moveMin, distanceMin, nodesMax));
          }
          moveMin->unmake(position);
        }
        std::stable_sort(
            nodesMin.begin(), nodesMin.end(),
            [](const std::shared_ptr<Node>& node1,
               const std::shared_ptr<Node>& node2) {
              return std::static_pointer_cast<MateBranch>(node1)
                         ->getDistance() >
                     std::static_pointer_cast<MateBranch>(node2)->getDistance();
            });
        nodes.push_back(
            std::make_shared<MateBranch>(moveMax, distanceMax, nodesMin));
      }
    }
    moveMax->unmake(position);
  }
  std::stable_sort(
      nodes.begin(), nodes.end(),
      [](const std::shared_ptr<Node>& node1,
         const std::shared_ptr<Node>& node2) {
        return std::static_pointer_cast<MateBranch>(node1)->getDistance() <
               std::static_pointer_cast<MateBranch>(node2)->getDistance();
      });
  return nodes;
}
int searchMax(int nMoves, Position& position,
              const std::vector<std::shared_ptr<Move>>& pseudoLegalMovesMax) {
  int max = -1;
  for (const std::shared_ptr<Move>& move : pseudoLegalMovesMax) {
    if (std::vector<std::shared_ptr<Move>> pseudoLegalMovesMin;
        move->make(position, pseudoLegalMovesMin, std::nullopt)) {
      int min = searchMin(nMoves, position, pseudoLegalMovesMin);
      if (min > max) {
        max = min;
      }
    }
    move->unmake(position);
    if (max == nMoves) {
      break;
    }
  }
  return max;
}
int searchMin(int nMoves, Position& position,
              const std::vector<std::shared_ptr<Move>>& pseudoLegalMovesMin) {
  int min = 0;
  if (nMoves == 1) {
    for (const std::shared_ptr<Move>& move : pseudoLegalMovesMin) {
      if (move->make(position, std::nullopt, std::nullopt)) {
        min = -1;
      }
      move->unmake(position);
      if (min == -1) {
        break;
      }
    }
  } else {
    for (const std::shared_ptr<Move>& move : pseudoLegalMovesMin) {
      if (std::vector<std::shared_ptr<Move>> pseudoLegalMovesMax;
          move->make(position, pseudoLegalMovesMax, std::nullopt)) {
        int max = searchMax(nMoves - 1, position, pseudoLegalMovesMax);
        if (min == 0 || max < min) {
          min = max;
        }
      }
      move->unmake(position);
      if (min == -1) {
        break;
      }
    }
  }
  if (min == 0) {
    std::shared_ptr<Move> nullMove = std::make_shared<NullMove>();
    min = nullMove->make(position, std::nullopt, std::nullopt) ? -1 : nMoves;
    nullMove->unmake(position);
  }
  return min;
}

}  // namespace movement
