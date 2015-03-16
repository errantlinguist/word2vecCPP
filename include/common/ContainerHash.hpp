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

#ifndef CONTAINERHASH_HPP
#define CONTAINERHASH_HPP

#include <boost/functional/hash.hpp>

namespace common
{

// Taken from <http://stackoverflow.com/a/10405129/1391325>
template <typename Container> // we can make this generic for any container [1]
struct ContainerHash
{
    typedef Container argument_type;
    typedef std::size_t result_type;

    result_type operator()(const argument_type& c) const
    {
        return boost::hash_range(c.begin(), c.end());
    }
};

}

#endif // CONTAINERHASH_HPP
