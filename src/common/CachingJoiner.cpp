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

#include "common/CachingJoiner.hpp"

#include <boost/algorithm/string/join.hpp>

namespace common
{

CachingJoiner::CachingJoiner(std::string sep) : sep(sep)
{
	//ctor
}

const CachingJoiner::result_type& CachingJoiner::operator() (const argument_type& input) noexcept
{
	auto cacheIter = cache.find(input);
	if (cacheIter == cache.end())
	{
		CachingJoiner::result_type joined = boost::algorithm::join(input, sep);
		return cache.insert(std::make_pair(input, joined)).first->second;
	}
	else
	{
		return cacheIter->second;
	}

}

}
