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

#include <algorithm>
#include <fstream>
#include <queue>
#include <sysexits.h>
#include <unordered_map>
#include <vector>

#include <limits.h>

#include <boost/tokenizer.hpp>

#include "common/functional_maps.hpp"
#include "common/CachingJoiner.hpp"
#include "w2v/distance.hpp"
#include "w2v/TokenSequence.hpp"
#include "w2v/WordProjections.hpp"


namespace w2v
{

static std::function<std::string(const std::string&)> lowercaser = [] (const std::string& s) -> std::string
{
	std::string result(s);
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);
	return result;
};

static std::pair<real, word_pair_error_code> distance(WordProjections& projections, const TokenSequence& tokenSequence_i, const std::string& vocabLookupKey)
{
    return distance(projections, tokenSequence_i.getTokenArray(), tokenSequence_i.getTokenCount(), vocabLookupKey.c_str());
}

static std::pair<real, word_pair_error_code> lowercaseDistance(WordProjections& projections, const std::vector<std::string>& tokenSequence_i, const std::string& joined_i_lower, const std::string& vocabLookupKey, std::unordered_map<std::string, TokenSequence>& tokenSequenceCache, std::unordered_map<std::string, std::string>& lowercaseCache)
{
    const TokenSequence& ts_i = tokenSequenceCache.emplace(joined_i_lower, tokenSequence_i).first->second;
    std::string vocabLookupKey_lower = common::emplace(lowercaseCache, vocabLookupKey, lowercaser).first->second;
    return distance(projections, ts_i, vocabLookupKey_lower);
}

static void printDistanceTable(WordProjections& projections, const std::vector<std::vector<std::string> >& tokenSequences, std::ostream& out=std::cout, std::ostream& err=std::cerr, const std::string& rowSep="\n", const std::string& colSep="\t")
{
    common::CachingJoiner tableKeyJoiner(" ");
    common::CachingJoiner vocabKeyJoiner("_");
    std::unordered_map<std::string, TokenSequence> tokenSequenceCache;
    std::unordered_map<std::string, std::string> lowercaseCache;
    std::unordered_map<std::string, std::unordered_map<std::string, real> > table;
    size_t comparisons = 0;
    out << "FIRST" << colSep << "SECOND" << colSep << "DIST" << rowSep;
    for (const std::vector<std::string>& tokenSequence_i : tokenSequences)
    {
        const std::string& joined_i = tableKeyJoiner(tokenSequence_i);
        std::unordered_map<std::string, real>& row = table[joined_i];
        for (const std::vector<std::string>& tokenSequence_j : tokenSequences)
        {
            const std::string& joined_j = tableKeyJoiner(tokenSequence_j);
            auto colIter = row.find(joined_j);
            if (colIter == row.end())
            {
                if (joined_i == joined_j)
                {
                    const real dist = 0;
                    row[joined_j] = dist;
                    out << joined_i << colSep << joined_j << colSep << dist << rowSep;
                    ++comparisons;
                }
                else
                {
                    const TokenSequence& ts_i = tokenSequenceCache.emplace(joined_i, tokenSequence_i).first->second;
                    const std::string& vocabLookupKey = vocabKeyJoiner(tokenSequence_j);

                    const std::pair<real, word_pair_error_code> distResults = distance(projections, ts_i, vocabLookupKey);
                    if (distResults.second == OK)
                    {
                        const real dist = distResults.first;
                        row[joined_j] = dist;
                        out << joined_i << colSep << joined_j << colSep << dist << rowSep;
                        ++comparisons;
                    }
                    else if (distResults.second == SECOND)
                    {
                        //                        err << "\"" << joined_j << "\" not found in dictionary; Trying lowercase version.\n";
						const std::string& joined_i_lower = common::emplace(lowercaseCache, joined_i, lowercaser).first->second;
                        const std::pair<real, word_pair_error_code> distResults = lowercaseDistance(projections, tokenSequence_i, joined_i_lower, vocabLookupKey, tokenSequenceCache, lowercaseCache);
                        if (distResults.second == OK)
                        {
							std::string joined_j_lower = common::emplace(lowercaseCache, joined_j, lowercaser).first->second;
                            const real dist = distResults.second;
                            row[joined_j] = dist;
                            out << joined_i_lower << colSep << joined_j_lower << colSep << dist << rowSep;
                            ++comparisons;
                        }
                        else
                        {
                            //                            err << "Lowercased \"" << joined_j_lower << "\" lookup failed.\n";
                        }
                    }
                    else if (distResults.second == FIRST)
                    {
                        //                        err << "\"" << joined_i << "\" not found in dictionary; Trying lowercase version.\n";
                        const std::string& joined_i_lower = common::emplace(lowercaseCache, joined_i, lowercaser).first->second;
                        const std::pair<real, word_pair_error_code> distResults = lowercaseDistance(projections, tokenSequence_i, joined_i_lower, vocabLookupKey, tokenSequenceCache, lowercaseCache);
                        if (distResults.second == OK)
                        {
                            std::string joined_j_lower = common::emplace(lowercaseCache, joined_j, lowercaser).first->second;
                            const real dist = distResults.second;
                            row[joined_j] = dist;
                            out << joined_i_lower << colSep << joined_j_lower << colSep << dist << rowSep;
                            ++comparisons;
                        }
                        else
                        {
                            //                            err << "Lowercased \"" << joined_i << "\" lookup failed.\n";
                            // Break out of loop for second word in word pair
                            break;
                        }
                    }
                    else
                    {
                        err << "An undentified error occured for word pair {\"" << joined_i << "\", \"" << joined_j << "\"}.\n";
                    }
                }
            }
            else
            {
                err << "Already computed distance between \"" << joined_i << "\" and \"" << joined_j << "\"; Skipping.\n";
            }
        }
    }
    std::cerr << "Checked " << comparisons << " word pairs.\n";
}

//static int printProjections(const WordProjections& projections, std::ostream& out=std::cout, std::ostream& err=std::cerr, const std::string& rowSep="\n", const std::string& colSep="\t")
//{
//    for (size_t vocabIdx = 0; vocabIdx < projections.getVocabSize(); ++vocabIdx)
//    {
//        const char* const vocabSymbol = projections.getVocabSymbol(vocabIdx);
//        out << vocabSymbol;
//
//        ArrayIterator<real> projectionStart = projections.getProjectionVectorBegin(vocabIdx);
//		const ArrayIterator<real> projectionEnd = projections.getProjectionVectorBegin(vocabIdx + 1);
//		while (projectionStart != projectionEnd)
//		{
//			out << colSep << *projectionStart;
//
//			++projectionStart;
//		}
//
//		 out << rowSep;
//    }
//
//    return EXIT_SUCCESS;
//}

static std::vector<std::vector<std::string> > readTokens(std::istream& in)
{
    std::vector<std::vector<std::string> > result;

    std::string line;
    while (std::getline(in,line))
    {
        if(line.find_first_not_of(' ') != std::string::npos)
        {
            boost::char_separator<char> sep(" ");
            typedef boost::token_iterator_generator<boost::char_separator<char> >::type Iter;
            Iter beg = boost::make_token_iterator<std::string>(line.begin(), line.end(), sep);
            Iter end = boost::make_token_iterator<std::string>(line.end(), line.end(), sep);
            std::vector<std::string> tokenSequence(beg, end);
            result.push_back(tokenSequence);
        }
    }

    return result;
}

static int printDistanceTable(const char* tokensPath, WordProjections& projections)
{
    int result = EXIT_FAILURE;

    std::cerr << "Reading tokens from \"" << tokensPath << "\"...\n";
    std::ifstream tokensFile(tokensPath);
    if (tokensFile.is_open())
    {
        std::vector<std::vector<std::string> > tokenSequences = readTokens(tokensFile);
        tokensFile.close();
        std::cerr << "Read in " << tokenSequences.size() << " token sequence(s).\n";
        printDistanceTable(projections, tokenSequences);
        //                            std::map<std::string, std::map<std::string, real> > distances = createDistanceTable(projections, tokenSequences);
        //                            std::cerr << "Created a table with " << distances.size() << " rows.\n";
        //                            print(distances, std::cout);

        result = EXIT_SUCCESS;
    }
    else
    {
        std::cerr << "Tokens file not found.\n";
        result = EX_NOINPUT;
    }

    return result;
}

static int printDistanceTable(std::istream& in, WordProjections& projections)
{
    std::vector<std::vector<std::string> > tokenSequences = readTokens(in);
    std::cerr << "Read in " << tokenSequences.size() << " token sequence(s).\n";
    printDistanceTable(projections, tokenSequences);
    //                        std::map<std::string, std::map<std::string, real> > distances = createDistanceTable(projections, tokenSequences);
    //                        std::cerr << "Created a table with " << distances.size() << " rows.\n";
    //						print(distances, std::cout);

    return EXIT_SUCCESS;
}

}

int main(int argc, char **argv)
{
    int result = EXIT_FAILURE;

    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " PROJS [TOKENS_FILE]\nwhere PROJS contains word projections in the BINARY FORMAT\n";
        result = EX_USAGE;
    }
    else
    {
        const char* projectionsPath = argv[1];
        std::FILE* projectionsFile = std::fopen(projectionsPath, "rb");
        if (projectionsFile == NULL)
        {
            std::cerr << "Projections file not found.\n";
            result = EX_NOINPUT;
        }
        else
        {
//			std::cerr << "sizeof(real)" << sizeof(real) << "\n";
//			std::cerr << std::numeric_limits<float>::is_iec559 << "\n";

            std::cerr << "Reading word projections from path \"" << projectionsPath << "\"...\n";
            long long vocabSize;
            std::fscanf(projectionsFile, "%lld", &vocabSize);
            std::cerr << vocabSize << " words in total.\n";
            long long dimensionality;
            std::fscanf(projectionsFile, "%lld", &dimensionality);
            std::cerr << "Dimensionality is " << dimensionality << ".\n";
            w2v::WordProjections projections(vocabSize, dimensionality);
            projections.load(projectionsFile);
            std::fclose(projectionsFile);

            if (argc > 2)
            {
                const char* tokensPath = argv[2];
                result = w2v::printDistanceTable(tokensPath, projections);
            }
            else if (isatty(fileno(stdin)))
            {
                // Read interactive console input
                consoleDistance(projections);
                result = EXIT_SUCCESS;
            }
            else
            {
                result = w2v::printDistanceTable(std::cin, projections);
            }
//			result = w2v::printProjections(projections);
        }

        return result;
    }
}
