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

#ifndef VPL_LIST_H_INCLUDED_
#define VPL_LIST_H_INCLUDED_

namespace vpl
{
	template <class T> struct ListNode
	{
		ListNode(const T& value):value_(value),next_(0),prev_(0){}
		~ListNode(){}
		
		T value_;
		ListNode<T>* next_;
		ListNode<T>* prev_;
	};

	template <class T> class VPL_API ListIterator;

	template <class T> class VPL_API List
	{
	public:
		List():head_(0),tail_(0){}
		~List(){clear();}

		void insert(const T& item)
		{
			ListNode<T>* node = new ListNode<T>(item);

			if(!head_)
				head_ = tail_ = node;
			else
			{
				tail_->next_ = node;
				node->prev_ = tail_;

				if(tail_ == head_)
					head_->next_ = node;

				tail_ = node;
			}
		}
		
		void remove(const T& item)
		{
			ListNode<T>* node = head_;

			while(node)
			{
				if(item == node->value_)
				{
					node->prev_->next_ = node->next_;
					node->next_->prev_ = node->prev_;

					delete node;

					return;
				}

				node = node->next_;
			}
		}

		void clear()
		{
			ListNode<T>* node = head_;
			ListNode<T>* temp;

			while(node)
			{
				temp = node;
				node = node->next_;

				delete temp;
			}

			head_ = tail_ = 0;
		}

	private:
		
		friend class ListIterator<T>;

		ListNode<T>* head_;
		ListNode<T>* tail_;
	};

	template <class T> class VPL_API ListIterator
	{
	public:
		ListIterator():current_(0){}
		~ListIterator(){}

		inline void begin(List<T>& list)
		{
			current_ = list.head_;
		}
		inline void beginReversed(List<T>& list)
		{
			current_ = list.tail_;
		}
		inline void operator ++() 
		{
			current_ = current_->next_;
		}
		inline void operator --() 
		{
			current_ = current_->prev_;
		}
		inline bool atLast() const
		{
			return current_ == 0;
		}
		inline T& operator *() 
		{
			return current_->value_;
		}

	private:

		ListNode<T>* current_;

	};
}

#endif
