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

#include "w2v/TokenSequence.hpp"

#include <cstring>

namespace w2v
{

static size_t fillTokenArray(std::vector<std::string>& source, token_sequence dest)
{
	size_t result = 0;

	for (std::string& token : source)
	{
		if (result < WORD2VEC_MAX_W)
		{
			if (token.size() > WORD2VEC_MAX_SIZE)
			{
				// Reduce size of backing string to fit in token array
				token.resize(WORD2VEC_MAX_SIZE);
			}
			strcpy(dest[result], token.c_str());
			++result;
		}
		else
		{
			break;
		}
	}

	return result;
}


TokenSequence::TokenSequence(std::vector<std::string> tokens) : tokens(tokens)
{
	//ctor
	tokenCount = fillTokenArray(this->tokens, tokenArray);
}

const std::vector<std::string>& TokenSequence::getTokens() const
{
	return tokens;
}

//const std::string& TokenSequence::getJoined() const
//{
//	return joined;
//}

const token_sequence& TokenSequence::getTokenArray() const
{
	return tokenArray;
}

size_t TokenSequence::getTokenCount() const
{
	return tokenCount;
}

}
