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

#ifndef TOKENSEQUENCE_HPP
#define TOKENSEQUENCE_HPP

#include <string>
#include <vector>

#include "w2v/distance.hpp"

namespace w2v
{

class TokenSequence
{
	public:
		/** Default constructor */
		TokenSequence(std::vector<std::string> tokens);
//		TokenSequence(const TokenSequence& copyee) = default;
		/** Default destructor */
		virtual ~TokenSequence() = default;

		virtual const std::vector<std::string>& getTokens() const;
//		virtual const std::string& getJoined() const;
		virtual const token_sequence& getTokenArray() const;
		virtual size_t getTokenCount() const;

	protected:
	private:
		std::vector<std::string> tokens;
//		std::string joined;
		token_sequence tokenArray;
		size_t tokenCount;
};

}

#endif // TOKENSEQUENCE_HPP
