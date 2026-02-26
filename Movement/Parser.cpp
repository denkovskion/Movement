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

#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>

#include "PieceTypes.h"
#include "ProblemTypes.h"

namespace movement {

std::vector<std::unique_ptr<Problem>> readAllProblems() {
  std::vector<std::unique_ptr<Problem>> problems;
  for (std::string line; std::getline(std::cin, line);) {
    Position position;
    std::istringstream tokenInput(line);
    if (std::string token; tokenInput >> token) {
      std::istringstream symbolInput(token);
      int rank = 7;
      for (; rank >= 0; --rank) {
        int file = 0;
        for (; file <= 7; ++file) {
          char symbol = symbolInput.get();
          if (symbol >= '1' && symbol <= '8' - file) {
            file += symbol - '0';
            if (file > 7) {
              break;
            }
            symbol = symbolInput.get();
          }
          int square = file * 8 + rank;
          if (symbol == 'K') {
            position.board[square] = std::make_unique<King>(false);
          } else if (symbol == 'Q') {
            position.board[square] = std::make_unique<Queen>(false);
          } else if (symbol == 'R') {
            position.board[square] = std::make_unique<Rook>(false);
          } else if (symbol == 'B') {
            position.board[square] = std::make_unique<Bishop>(false);
          } else if (symbol == 'N') {
            position.board[square] = std::make_unique<Knight>(false);
          } else if (symbol == 'P') {
            position.board[square] = std::make_unique<Pawn>(false);
          } else if (symbol == 'k') {
            position.board[square] = std::make_unique<King>(true);
          } else if (symbol == 'q') {
            position.board[square] = std::make_unique<Queen>(true);
          } else if (symbol == 'r') {
            position.board[square] = std::make_unique<Rook>(true);
          } else if (symbol == 'b') {
            position.board[square] = std::make_unique<Bishop>(true);
          } else if (symbol == 'n') {
            position.board[square] = std::make_unique<Knight>(true);
          } else if (symbol == 'p') {
            position.board[square] = std::make_unique<Pawn>(true);
          } else {
            break;
          }
        }
        if (file <= 7) {
          break;
        }
        if (rank > 0) {
          if (symbolInput.get() != '/') {
            break;
          }
        } else {
          if (symbolInput.get(); !symbolInput.eof()) {
            break;
          }
        }
      }
      if (rank < 0) {
        if (std::string token; tokenInput >> token) {
          if (std::regex_match(token, std::regex("[wb]"))) {
            if (token == "w") {
              position.blackToMove = false;
            } else if (token == "b") {
              position.blackToMove = true;
            }
            if (std::string token; tokenInput >> token) {
              if (std::regex_match(token, std::regex("\\bK?Q?k?q?|-"))) {
                if (!(token == "-")) {
                  for (char symbol : token) {
                    if (symbol == 'K' || symbol == 'Q') {
                      position.castlingOrigins.insert(32);
                    } else if (symbol == 'k' || symbol == 'q') {
                      position.castlingOrigins.insert(39);
                    }
                    if (symbol == 'K') {
                      position.castlingOrigins.insert(56);
                    } else if (symbol == 'Q') {
                      position.castlingOrigins.insert(0);
                    } else if (symbol == 'k') {
                      position.castlingOrigins.insert(63);
                    } else if (symbol == 'q') {
                      position.castlingOrigins.insert(7);
                    }
                  }
                }
                if (std::string token; tokenInput >> token) {
                  if (std::regex_match(token, std::regex("[a-h][36]|-"))) {
                    if (!(token == "-")) {
                      int file = token.at(0) - 'a';
                      int rank = token.at(1) - '1';
                      position.enPassantTarget = file * 8 + rank;
                    }
                    if (std::string token; tokenInput >> token) {
                      if (std::regex_match(token, std::regex("acd|dm"))) {
                        try {
                          if (token == "acd") {
                            if (std::string token; tokenInput >> token) {
                              if (std::regex_match(
                                      token, std::regex("(0|[1-9]\\d*);"))) {
                                int nPlies = std::stoi(token);
                                if (std::string token; !(tokenInput >> token)) {
                                  Piece::validate(position.board,
                                                  position.blackToMove,
                                                  position.castlingOrigins,
                                                  position.enPassantTarget);
                                  problems.push_back(std::make_unique<Perft>(
                                      std::move(position), nPlies));
                                  continue;
                                }
                              }
                            }
                          } else if (token == "dm") {
                            if (std::string token; tokenInput >> token) {
                              if (std::regex_match(token,
                                                   std::regex("[1-9]\\d*;"))) {
                                int nMoves = std::stoi(token);
                                if (std::string token; !(tokenInput >> token)) {
                                  Piece::validate(position.board,
                                                  position.blackToMove,
                                                  position.castlingOrigins,
                                                  position.enPassantTarget);
                                  problems.push_back(
                                      std::make_unique<MateSearch>(
                                          std::move(position), nMoves));
                                  continue;
                                }
                              }
                            }
                          }
                        } catch (const std::invalid_argument& error) {
                          logger(std::cerr) << "Not accepted line: '" + line +
                                                   "'. " + error.what() + ".\n";
                          return {};
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      logger(std::cerr) << "Invalid line: '" + line + "'.\n";
      return {};
    }
  }
  return problems;
}

}  // namespace movement
