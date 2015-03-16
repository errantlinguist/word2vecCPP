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

#ifndef CSTRINGTOKENIZER_HPP
#define CSTRINGTOKENIZER_HPP

#include <unordered_map>

#include "common/cstring_comparison.hpp"
#include "w2v/distance.hpp"

namespace w2v
{

class CStringTokenizer
{
	public:
		typedef const char * argument_type;
		typedef std::pair<token_sequence, size_t> result_type;

		/** Default constructor */
		CStringTokenizer();

		 const result_type& operator() (const argument_type& input) noexcept;
	protected:
	private:
		std::unordered_map<argument_type, result_type, common::CStringHash, common::CStringEquals> cache;
//		std::unordered_map<std::string, result_type> cache;
};

}

#endif // CSTRINGTOKENIZER_HPP
