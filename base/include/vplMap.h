/* VPL - Vector path library
 * Copyright (C) 2009 - 2011 Mattias Sahlén <mattias.sahlen@gmail.com>
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General General Public License for more details.
 *
 * You should have received a copy of the GNU General General Public
 * License along with this library;
 * if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VPL_MAP_H_INCLUDED_
#define VPL_MAP_H_INCLUDED_

#include "vplRedBlackTree.h"

namespace vpl
{
	// Simple map class based red black trees

	// Not a general pair, use std::pair for that
	template <class T1, class T2> struct VPL_API Pair
	{
		T1 first_;
		T2 second_;
		
		Pair():first_(T1()),second_(T2()) {}
		Pair(const T1& x):first_(x),second_(T2()) {}
		Pair(const T1& x,const T2& y):first_(x),second_(y) {}
		
		// Needed for red black tree to work since it uses comparisons
		// to determine order.
		// Only key is used in comparisons(duh!)
		inline bool operator >(const Pair<T1,T2>& rhs) const 
		{
			return first_ > rhs.first_;
		}
		inline bool operator <(const Pair<T1,T2>& rhs) const 
		{
			return first_ < rhs.first_;
		}
		inline bool operator ==(const Pair<T1,T2>& rhs) const
		{
			return first_ == rhs.first_;
		}
	};

	// Forward declaration
	template <class KeyType,class ValueType> class VPL_API MapIterator;

	template <class KeyType,class ValueType> class VPL_API Map
	{
	public:
		Map(){}
		~Map(){}
		
		// Only one value allowed per key, if key exists we don't insert anything
		void insert(const KeyType& key,const ValueType& value)
		{
			Pair<KeyType,ValueType> pair(key,value);
			
			if(tree_.find(pair) == 0)
				tree_.insert(pair);
		}

		void remove(const KeyType& key)
		{
			Pair<KeyType,ValueType> pair(key,ValueType());
			
			tree_.remove(pair);
		}

		inline void clear()
		{
			tree_.clear();
		}

		inline ValueType& operator[](const KeyType& key)
		{
			Pair<KeyType,ValueType> pair(key);
			Pair<KeyType,ValueType>* result = tree_.find(pair);

			if(result)
				return result->second_; 
			else 
				return tree_.insert(pair)->second_;
		}

	private:

		friend class MapIterator<KeyType,ValueType>;

		RedBlackTree<Pair<KeyType,ValueType> > tree_;
	};

	template <class KeyType,class ValueType> class VPL_API MapIterator
	{
	public:
		MapIterator(){}
		~MapIterator(){}

		inline void begin(Map<KeyType,ValueType>& map)
		{
			iterator_.begin(map.tree_);
		}
		inline void beginReversed(Map<KeyType,ValueType>& map)
		{
			iterator_.beginReversed(map.tree_);
		}
		inline void operator ++() 
		{
			++iterator_;
		}
		inline void operator --() 
		{
			--iterator_;
		}
		inline Pair<KeyType,ValueType>& operator *() 
		{
			return *iterator_;
		}
		inline bool atLast() const
		{
			return iterator_.atLast();
		}

	private:

		RedBlackTreeIterator<Pair<KeyType,ValueType> > iterator_;
	};
}

#endif
