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

#ifndef FUNCTIONAL_MAPS_HPP
#define FUNCTIONAL_MAPS_HPP

namespace common
{

template<typename M, typename K, typename V>
std::pair<typename M::iterator, bool> emplace(M& map, const K& key, std::function<V(const K&)>& supplier)
{
auto iter = map.find(key);
	if (iter == map.end())
	{
		V supplied = supplier(key);
		return map.insert(std::make_pair(key, supplied));
	}
	else
	{
		return std::make_pair(iter, false);
	}
}

}

#endif // FUNCTIONAL_MAPS_HPP
