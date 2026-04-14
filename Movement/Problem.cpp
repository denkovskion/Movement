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

#include "Problem.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

#include "Node.h"

namespace movement {

Problem::Problem(Position position) : position_(std::move(position)) {}

Problem::~Problem() {}

void Problem::solve(bool detailed) {
  std::cout << std::string(42, '_') << std::endl;
  std::cout << toFormatted(position_, getOperation()) << std::endl;
  std::cout << std::endl;
  logger(std::clog) << (detailed ? "Solving with analysis...\n"
                                 : "Solving...\n");
  std::chrono::steady_clock::time_point begin =
      std::chrono::steady_clock::now();
  std::shared_ptr<Node> solution;
  if (std::vector<std::shared_ptr<Move>> pseudoLegalMoves;
      isPositionLegal(position_, pseudoLegalMoves)) {
    solution = doSolve(position_, pseudoLegalMoves, detailed);
  } else {
    solution = std::make_shared<IllegalNode>();
  }
  std::cout << toFormatted(solution, position_) << std::endl;
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  logger(std::clog)
      << "Finished solving in " +
             std::to_string(
                 std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       begin)
                     .count()) +
             "ms.\n";
}

std::ostream& logger(std::ostream& output) {
  std::time_t calendarTime = std::time(nullptr);
#if _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
  std::tm* localTime = std::localtime(&calendarTime);
#if _MSC_VER
#pragma warning(pop)
#endif
  if (localTime) {
    output << std::put_time(localTime, "%c Movement: ");
  } else {
    output << "Movement: ";
  }
  return output;
}

}  // namespace movement
