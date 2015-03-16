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

#ifndef ARRAYITERATOR_HPP
#define ARRAYITERATOR_HPP

namespace common
{

template <typename T>
//class ArrayIterator : public std::iterator<std::random_access_iterator_tag, T>
class ArrayIterator : public std::iterator<std::input_iterator_tag, T>
{
public:
    ArrayIterator(T* p) : p(p)
    {
    }
    ArrayIterator& operator++()
    {
        ++p;
        return *this;
    }
    ArrayIterator operator++(int)
    {
        ArrayIterator tmp(*this);
        operator++();
        return tmp;
    }
    bool operator==(const ArrayIterator& rhs)
    {
        return p==rhs.p;
    }
    bool operator!=(const ArrayIterator& rhs)
    {
        return p!=rhs.p;
    }
    T& operator*()
    {
        return *p;
    }
protected:
private:
    T* p;
};

}

#endif // ARRAYITERATOR_HPP
