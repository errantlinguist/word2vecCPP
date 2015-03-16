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

#include "nlp/Vocabulary.hpp"

namespace nlp
{

Vocabulary::Vocabulary() : Vocabulary(std::map<std::string, size_t>())
{
	//ctor
}

Vocabulary::Vocabulary(std::map<std::string, size_t> vocabIds) : vocabIds(vocabIds), vocabTypes(), nextId(0)
{
	//ctor
	// TODO: Reconstruct dependent members based on data in (possibly) non-empty vocabIds
}

size_t Vocabulary::fetchId(const std::string& word)
{
	size_t result = 0;
	auto iter = vocabIds.find(word);
	if (iter == vocabIds.end())
	{
		result = nextId++;
		vocabIds[word] = result;
		vocabTypes.insert(word);
	}
	else
	{
		result = iter->second;
	}

	return result;
}

//Vocabulary::~Vocabulary()
//{
//	//dtor
//}

}
