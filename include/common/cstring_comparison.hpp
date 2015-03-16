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

#ifndef CSTRING_COMPARISON_HPP
#define CSTRING_COMPARISON_HPP

#include <cstring>

namespace common
{

typedef const char * cstr_ptr;

// Adapted from <http://stackoverflow.com/a/4157729/1391325>
struct CStringEquals
{
	typedef cstr_ptr first_argument_type;
	typedef cstr_ptr second_argument_type;
    typedef bool result_type;

    result_type operator()(const first_argument_type& lhs, const second_argument_type& rhs) const noexcept
    {
		return std::strcmp(lhs, rhs) < 0;
    }
};

// Adapted from <http://stackoverflow.com/a/25960893/1391325>
struct CStringHash
{
    typedef cstr_ptr argument_type;
    typedef std::size_t result_type;

    result_type operator()(const argument_type& key) const noexcept
    {
        std::size_t h = 0;
        for (std::size_t i = 0 , l = std::strlen(key) ; i < l ; ++i)
        {
            h += h * 65599 + key[i];
        }
        return h ^ (h >> 16);
    }
};

}

#endif // CSTRING_COMPARISON_HPP
