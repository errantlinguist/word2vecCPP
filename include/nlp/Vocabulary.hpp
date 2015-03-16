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

#ifndef VOCABULARY_HPP
#define VOCABULARY_HPP

#include <map>
#include <set>

namespace nlp
{

class Vocabulary
{
	public:
		/** Default constructor */
		Vocabulary();
		Vocabulary(const Vocabulary& copyee) = default;
		/** Default destructor */
		virtual ~Vocabulary() = default;

		virtual size_t fetchId(const std::string& word);

		std::set<std::string>& getVocabTypes() { return vocabTypes; }
		const std::set<std::string>& getVocabTypes() const { return vocabTypes; }

	protected:
		Vocabulary(std::map<std::string, size_t> vocabIds);
	private:
		std::map<std::string, size_t> vocabIds;
		std::set<std::string> vocabTypes;
		size_t nextId;
};

}

#endif // VOCABULARY_HPP
