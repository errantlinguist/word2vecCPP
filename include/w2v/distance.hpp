/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/

#ifndef DISTANCE_H
#define DISTANCE_H

#include <string>
#include <utility>

#define WORD2VEC_MAX_SIZE 2000         // max length of strings
#define WORD2VEC_MAX_W 50              // max length of vocabulary entries

namespace w2v
{

typedef float real;
typedef char token[WORD2VEC_MAX_SIZE];
typedef token token_sequence[WORD2VEC_MAX_W];

class WordProjections;

typedef enum {OK, FIRST, SECOND, BOTH} word_pair_error_code;

void consoleDistance(WordProjections& projections);
std::pair<real, word_pair_error_code> distance(WordProjections& projections, const token_sequence tokens1, size_t tokens1_count, const std::string& vocabLookupKey);

}

#endif // DISTANCE_H
