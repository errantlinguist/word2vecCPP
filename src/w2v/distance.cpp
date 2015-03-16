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

#include "w2v/distance.hpp"

//#include <algorithm>
#include <cmath>
#include <list>
#include <unordered_map>
#include <vector>

#include "w2v/CStringTokenizer.hpp"
#include "w2v/WordProjections.hpp"

#define MAX_TOKEN_SEQUENCE_LENGTH 100

namespace w2v
{

// TODO: Replace this C array with a vector
typedef size_t* const TokenVocabIdxMap;
//typedef std::vector<size_t> TokenVocabIdxMap;

template <typename E>
static bool containsElement(const E* const & seq, const size_t seqLength, const E& element)
{
    bool result = false;
    for (size_t i = 0; i < seqLength; ++i)
    {
        if (seq[i] == element)
        {
            result = true;
            break;
        }
    }
    return result;
}

static real calculateDistance(const WordProjections& projections, const std::vector<real>& features, const size_t inputTokenVocabIdx)
{
    real result = 0;

    for (size_t featureIdx = 0; featureIdx < projections.getDimensionality(); ++featureIdx)
    {
        result += features[featureIdx] * projections.getProjection(inputTokenVocabIdx, featureIdx);
    }

    return result;
}

static std::vector<real> createFeatureValueVector(const WordProjections& projections, const TokenVocabIdxMap& inputTokenVocabIdxs, const size_t inputTokenCount)
{
	const size_t dimensionality = projections.getDimensionality();
	std::vector<real> result(dimensionality, 0);

    for (size_t inputTokenIdx = 0; inputTokenIdx < inputTokenCount; ++inputTokenIdx)
    {
		const size_t inputTokenVocabIdx = inputTokenVocabIdxs[inputTokenIdx];
        if (inputTokenVocabIdx != WordProjections::UNKNOWN_VOCAB_IDX)
        {
            for (size_t featureIdx = 0; featureIdx < dimensionality; ++featureIdx)
            {
                real val = projections.getProjection(inputTokenVocabIdx, featureIdx);
                result[featureIdx] += val;
            }
        }
    }
    real len = 0;
    for (size_t i = 0; i < dimensionality; ++i) len += result[i] * result[i];
    len = (real)sqrt(len);
    for (size_t i = 0; i < dimensionality; ++i) result[i] /= len;

    return result;
}

template <typename TokenVocabIdxMap>
static size_t fillTokenVocabIdxs(TokenVocabIdxMap& inputTokenVocabIdxs, WordProjections& projections, const token_sequence inputTokens, const size_t inputTokenCount, bool printPosition=false)
{
	size_t result = 0;
    for (; result < inputTokenCount; ++result)
    {
        const char* inputToken = inputTokens[result];
		const size_t vocabIdx = projections.getVocabIdx(inputToken);
        inputTokenVocabIdxs[result] = vocabIdx;
//		inputTokenVocabIdxs.push_back(vocabIdx);
        if (vocabIdx == WordProjections::UNKNOWN_VOCAB_IDX)
        {
            if (printPosition)
            {
                fprintf(stderr, "Out of dictionary word!: %s\n", inputToken);
            }
            break;
        } else
        {
            if (printPosition)
            {
                fprintf(stderr, "\nWord: %s  Position in vocabulary: %lu\n", inputToken, vocabIdx);
            }
        }
    }

    return result;
}

static std::list<std::pair<const char*, real> > findNearestWords(const WordProjections& projections, const TokenVocabIdxMap& inputTokenVocabIdxs, const size_t inputTokenCount, const size_t maxResultSize=40)
{
	std::list<std::pair<const char*, real> > result;
	const std::vector<real> features = createFeatureValueVector(projections, inputTokenVocabIdxs, inputTokenCount);

    // For each word in the vocabulary,
    for (size_t vocabIdx = 0; vocabIdx < projections.getVocabSize(); ++vocabIdx)
    {
        // If the exact word was not found as a token in the input, calculate the most similar words
//        if (std::find(inputTokenVocabIdxs.cbegin(), inputTokenVocabIdxs.cend(), vocabIdx) != inputTokenVocabIdxs.cend())
        if (!containsElement(inputTokenVocabIdxs, inputTokenCount, vocabIdx))
        {
            const real dist = calculateDistance(projections, features, vocabIdx);
            for (auto wordRankIter = result.begin(); wordRankIter != result.end(); ++wordRankIter)
            {
                // If the distance for the given feature is less than the current best distance, add it to the list
                const std::pair<const char*, real>& wordDistance = *wordRankIter;
                if (dist > wordDistance.second)
                {
					result.emplace(wordRankIter, projections.getVocabSymbol(vocabIdx), dist);
					if (result.size() > maxResultSize)
					{
						result.pop_back();
					}
                    break;
                }
            }
            if (result.size() < maxResultSize)
            {
				result.emplace_back(projections.getVocabSymbol(vocabIdx), dist);
            }
        }
    }
    return result;
}

static void getInput(FILE* pInfile, token inputStr)
{
    size_t i = 0;
    while (true)
    {
        int ch = fgetc(pInfile);
        if ((ch == '\n') || (i >= WORD2VEC_MAX_SIZE - 1))
        {
            inputStr[i] = '\0';
            break;
        }
        else
        {
            inputStr[i] = (char)ch;
        }
        ++i;
    }
}

void consoleDistance(WordProjections& projections)
{
	CStringTokenizer tokenizer;
	while (true)
	{
		printf("Enter word or sentence (EXIT to break): ");

		token inputStr;
		// Read input into buffer
		getInput(stdin, inputStr);
		if (strcmp(inputStr, "EXIT") == 0)
		{
			break;
		}
		else
		{
			const std::pair<token_sequence, size_t> inputTokens = tokenizer(inputStr);

			size_t inputTokenVocabIdxs[MAX_TOKEN_SEQUENCE_LENGTH];
//			TokenVocabIdxMap inputTokenVocabIdxs;
			const size_t lastTokenIdx = fillTokenVocabIdxs(inputTokenVocabIdxs, projections, inputTokens.first, inputTokens.second, true);
			const size_t lastVocabIdx = inputTokenVocabIdxs[lastTokenIdx];
			if (lastVocabIdx != WordProjections::UNKNOWN_VOCAB_IDX)
			{
				printf("\n                                              Word       Cosine distance\n------------------------------------------------------------------------\n");
				const std::list<std::pair<const char*, real> > nearestWords = findNearestWords(projections, inputTokenVocabIdxs, inputTokens.second);
				for (const std::pair<const char*, real>& wordDist : nearestWords)
				{
					printf("%50s\t\t%f\n", wordDist.first, wordDist.second);
				}
			}
		}
	}
}

std::pair<real, word_pair_error_code> distance(WordProjections& projections, const token_sequence inputTokens, size_t inputTokenCount, const std::string& vocabLookupKey)
{
    std::pair<real, word_pair_error_code> result = std::make_pair(-1, BOTH);

	size_t inputTokenVocabIdxs[MAX_TOKEN_SEQUENCE_LENGTH];
//    TokenVocabIdxMap inputTokenVocabIdxs;
    const size_t lastInputTokenIdx = fillTokenVocabIdxs(inputTokenVocabIdxs, projections, inputTokens, inputTokenCount);
    const size_t lastInputTokenVocabIdx = inputTokenVocabIdxs[lastInputTokenIdx];
    if (lastInputTokenVocabIdx == WordProjections::UNKNOWN_VOCAB_IDX)
    {
//		fprintf(stderr, "\"%s\" not found in dictionary!\n", (char*)inputTokens);
        result.second = FIRST;
    }
    else
    {
		const std::vector<real> features = createFeatureValueVector(projections, inputTokenVocabIdxs, inputTokenCount);

        const size_t tokens2_vocabIdx = projections.getVocabIdx(vocabLookupKey);
        if (tokens2_vocabIdx == WordProjections::UNKNOWN_VOCAB_IDX)
        {
//			fprintf(stderr, "\"%s\" not found in dictionary!\n", (char*)tokens2);
            result.second = SECOND;
        }
        else
        {
            const real dist = calculateDistance(projections, features, tokens2_vocabIdx);
//				fprintf(stderr, "Distance for \"%s\": %f\n", token, dist);
			result.first = dist;
			result.second = OK;
        }
    }

    return result;
}

}

#undef MAX_TOKEN_SEQUENCE_LENGTH
