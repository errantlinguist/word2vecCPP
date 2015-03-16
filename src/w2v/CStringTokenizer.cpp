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

#include "w2v/CStringTokenizer.hpp"

namespace w2v
{

static size_t tokenize(const CStringTokenizer::argument_type& input_str, token_sequence& tokens)
{
    size_t b = 0;
    size_t c = 0;
    size_t result = 0;
    while (true)
    {
        tokens[result][b] = input_str[c];
        ++b;
        ++c;
        tokens[result][b] = '\0';
        if (input_str[c] == '\0') break;
        // If a whitespace character is found, jump to the next slot in the token array for the input
        if (input_str[c] == ' ')
        {
            ++result;
            b = 0;
            ++c;
        }
    }
    // Add 1 to correct count since arrays are 0-indexed
    ++result;
    return result;
}

CStringTokenizer::CStringTokenizer()
{
	//ctor
}

const CStringTokenizer::result_type& CStringTokenizer::operator() (const argument_type& input) noexcept
{
	auto cacheIter = cache.find(input);
	if (cacheIter == cache.end())
	{
		CStringTokenizer::result_type& result = cache[input];
		result.second = tokenize(input, result.first);
		return result;
	}
	else
	{
		return cacheIter->second;
	}

}

}
