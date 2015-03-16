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

#ifndef WORDPROJECTIONS_HPP
#define WORDPROJECTIONS_HPP

#include <memory>
#include <unordered_map>

#include "common/ArrayIterator.hpp"
//#include "common/cstring_comparison.hpp"
#include "w2v/distance.hpp"

namespace w2v
{

class WordProjections
{
public:

    static const size_t UNKNOWN_VOCAB_IDX;

    WordProjections(size_t vocabSize, size_t dimensionality);

    size_t getDimensionality() const;
    real getProjection(size_t inputTokenVocabIdx, size_t featureIdx) const;
//		const std::unique_ptr<char[]>& getVocab() const;
	common::ArrayIterator<real> getProjectionVectorBegin(size_t inputTokenVocabIdx) const;
//    size_t getVocabIdx(const char* inputToken) const;
    size_t getVocabIdx(const std::string& inputToken) const;
    size_t getVocabSize() const;
    const char* const getVocabSymbol(size_t vocabIdx) const;

    void load(FILE *pInfile);
protected:
private:

    size_t vocabSize;
    size_t dimensionality;
    std::unique_ptr<real[]> M;
    std::unique_ptr<char[]> vocab;
//    std::unordered_map<cstr_ptr, size_t, CStringHash, CStringEquals> vocabIdxCache;
	// NOTE: Converting C strings to std::string and then hashing them is faster than trying to use the custom hash above
     std::unordered_map<std::string, size_t> vocabIdxCache;

//    size_t findVocabIdx(const char* inputToken) const;
    real& getProjection(size_t inputTokenVocabIdx, size_t featureIdx);
    size_t getProjectionVectorBeginIdx(size_t inputTokenVocabIdx) const;

};

}

#endif // WORDPROJECTIONS_HPP
