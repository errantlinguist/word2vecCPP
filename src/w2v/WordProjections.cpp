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

#include "w2v/WordProjections.hpp"

#include <cmath>
#include <limits>

namespace w2v
{

const size_t WordProjections::UNKNOWN_VOCAB_IDX = std::numeric_limits<size_t>::max();

WordProjections::WordProjections(size_t vocabSize, size_t dimensionality) : vocabSize(vocabSize), dimensionality(dimensionality), M(new real[vocabSize * dimensionality]), vocab(new char[vocabSize * WORD2VEC_MAX_W]), vocabIdxCache(vocabSize + 1)
{
    //ctor
    if (vocabSize >= UNKNOWN_VOCAB_IDX)
    {
        throw std::invalid_argument("Vocabulary size greater than allowed maximum.");
    }
}

//size_t WordProjections::findVocabIdx(const char* inputToken) const
//{
//    size_t result = 0;
//    // Iterate over the words in the matrix until a matching word is found
//    for (; result < getVocabSize(); ++result)
//    {
//        if (strcmp(getVocabSymbol(result), inputToken) == 0)
//        {
//            break;
//        }
//    }
//
//    if (result == getVocabSize()) result = WordProjections::UNKNOWN_VOCAB_IDX;
//    return result;
//}

real& WordProjections::getProjection(size_t inputTokenVocabIdx, size_t featureIdx)
{
    return M[featureIdx + getProjectionVectorBeginIdx(inputTokenVocabIdx)];
}

real WordProjections::getProjection(size_t inputTokenVocabIdx, size_t featureIdx) const
{
    return M[featureIdx + getProjectionVectorBeginIdx(inputTokenVocabIdx)];
}

common::ArrayIterator<real> WordProjections::getProjectionVectorBegin(size_t inputTokenVocabIdx) const
{
	return common::ArrayIterator<real>(&M[getProjectionVectorBeginIdx(inputTokenVocabIdx)]);
}

size_t WordProjections::getProjectionVectorBeginIdx(size_t inputTokenVocabIdx) const
{
	return inputTokenVocabIdx * getDimensionality();
}

size_t WordProjections::getDimensionality() const
{
    return dimensionality;
}

//const WordProjections::std::unique_ptr<char[]>& getVocab() const
//{
//	return vocab;
//}

//size_t WordProjections::getVocabIdx(const char* inputToken) const
//{
//    return getVocabIdx(std::string(inputToken));
//}

size_t WordProjections::getVocabIdx(const std::string& inputToken) const
{
    size_t result;

    auto inputTokenVocabIdxIter = vocabIdxCache.find(inputToken);
    if (inputTokenVocabIdxIter == vocabIdxCache.end())
    {
//        result = findVocabIdx(inputToken);
		result = UNKNOWN_VOCAB_IDX;
//        vocabIdxCache[inputToken] = result;
    }
    else
    {
        result = inputTokenVocabIdxIter->second;
//        fprintf(stderr, "Found token in cache: %lu\n", result);
    }

    return result;
}

size_t WordProjections::getVocabSize() const
{
    return vocabSize;
}

const char* const WordProjections::getVocabSymbol(size_t vocabIdx) const
{
    return &vocab[vocabIdx * WORD2VEC_MAX_W];
}

void WordProjections::load(FILE *pInfile)
{
    for (size_t vocabIdx = 0; vocabIdx < getVocabSize(); ++vocabIdx)
    {
		const size_t wordStartIdx = vocabIdx * WORD2VEC_MAX_W;
        size_t charIdx = 0;
        while (true)
        {
            vocab[wordStartIdx + charIdx] = (char)fgetc(pInfile);
            if (feof(pInfile) || (vocab[wordStartIdx + charIdx] == ' ')) break;
            if ((charIdx < WORD2VEC_MAX_W) && (vocab[wordStartIdx + charIdx] != '\n')) ++charIdx;
        }
        vocab[wordStartIdx + charIdx] = '\0';
        vocabIdxCache[getVocabSymbol(vocabIdx)] = vocabIdx;

        for (size_t featureIdx = 0; featureIdx < getDimensionality(); ++featureIdx) fread(&getProjection(vocabIdx, featureIdx), sizeof(real), 1, pInfile);
        real len = 0;
        for (size_t featureIdx = 0; featureIdx < getDimensionality(); ++featureIdx) len += getProjection(vocabIdx, featureIdx) * getProjection(vocabIdx, featureIdx);
        len = (real)sqrt(len);
        for (size_t featureIdx = 0; featureIdx < getDimensionality(); ++featureIdx) getProjection(vocabIdx, featureIdx) /= len;
    }
}

}
