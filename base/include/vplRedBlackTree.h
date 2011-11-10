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

#ifndef VPL_RED_BLACK_TREE_H_INCLUDED_
#define VPL_RED_BLACK_TREE_H_INCLUDED_

#include "vplConfig.h"

// "Inspired" by :
// http://eternallyconfuzzled.com/tuts/datastructures/jswut_rbtree.aspx

static const vplUint cMaxTreeHeight = 128;

namespace vpl
{
	template <class T> class VPL_API RedBlackTree;
	template <class T> class VPL_API RedBlackTreeIterator;

	template <class T> class VPL_API RedBlackTreeNode
	{
	
	public:
		
		RedBlackTreeNode():red_(1)
		{
			link_[0] = link_[1] = 0;
		}
		~RedBlackTreeNode(){}

	private:

		friend class RedBlackTree<T>;
		friend class RedBlackTreeIterator<T>;

        RedBlackTreeNode<T>* link_[2];
        signed char red_;
        T value_;
	};

	template <class T> class VPL_API RedBlackTree
	{
	public:
		RedBlackTree():root_(0){}
		~RedBlackTree(){destroyAll();}
	
		T* insert(T data)
		{
			RedBlackTreeNode<T>* newNode; 

			if(root_ == 0) 
			{
				// Empty tree case
				root_ = newNode = makeNode(data);
			}
			else 
			{
				RedBlackTreeNode<T> head; // False tree root  
				RedBlackTreeNode<T>* g;   // grandparent
				RedBlackTreeNode<T>* t;   // parent 
				RedBlackTreeNode<T>* p;	  // iterator
				RedBlackTreeNode<T>* q;   // parent 
				int dir = 0;
				int last = 0;
 
				// Set up helpers 
				t = &head;
				g = p = 0;
				q = t->link_[1] = root_;
				
				// Search down the tree 
				for( ; ; ) 
				{
					if(q == 0) 
					{
						// Insert new node at the bottom 
						p->link_[dir] = q = newNode = makeNode(data);
					}
					else if(isRed(q->link_[0]) && isRed(q->link_[1]))
					{
						// Color flip 
						q->red_ = 1;
						q->link_[0]->red_ = 0;
						q->link_[1]->red_ = 0;
					}
 
					// Fix red violation 
					if(isRed(q) && isRed(p)) 
					{
						int dir2 = (t->link_[1] == g);
				
						if(q == p->link_[last])
							t->link_[dir2] = rotateSingle(g,!last);
						else
							t->link_[dir2] = rotateDouble(g,!last);
					}
		
					// Stop if found 
					if(q->value_ == data)
						break;
		
					last = dir;
					dir = q->value_ < data;
		
					// Update helpers 
					if(g != 0)
						t = g;
					
					g = p;
					p = q;
					q = q->link_[dir];
				}
				// Update root 
				root_ = head.link_[1];
			}
 
			// Make root black 
			root_->red_ = 0;
 
			return &newNode->value_;
		}
		
		void remove(T data)
		{ 
			if(root_ != 0) 
			{ 
				RedBlackTreeNode<T> head;   // False tree root  
				RedBlackTreeNode<T>* q;     // Helper
				RedBlackTreeNode<T>* p;     // Helper
				RedBlackTreeNode<T> *g;     // Helper  
				RedBlackTreeNode<T>* f = 0; // Found item  
				int dir = 1; 
				
				// Set up helpers  
				q = &head; 
				g = p = 0;
				q->link_[1] = root_;
				
				// Search and push a red down  
				while(q->link_[dir] != 0) 
				{ 
					int last = dir; 
					
					// Update helpers  
					g = p;
					p = q; 
					q = q->link_[dir]; 
					dir = q->value_ < data;
					
					// Save found node  
					if(q->value_ == data)
						f = q; 
					
					// Push the red node down  
					if(!isRed(q) && !isRed(q->link_[dir])) 
					{ 
						if(isRed(q->link_[!dir])) 
							p = p->link_[last] = rotateSingle(q,dir);
						else if(!isRed(q->link_[!dir])) 
						{
							RedBlackTreeNode<T>* s = p->link_[!last]; 
							
							if(s != 0) 
							{
								if(!isRed(s->link_[!last]) && 
								   !isRed(s->link_[last])) 
								{ 
									// Color flip  
									p->red_ = 0;
									s->red_ = 1;
									q->red_ = 1;	
								}
								else
								{ 
									int dir2 = g->link_[1] == p; 
									
									if(isRed(s->link_[last]))
										g->link_[dir2] = rotateDouble(p,last);
									else if(isRed(s->link_[!last]))
										g->link_[dir2] = rotateSingle(p,last); 
									
									// Ensure correct coloring 
									q->red_ = g->link_[dir2]->red_ = 1;
									g->link_[dir2]->link_[0]->red_ = 0;
									g->link_[dir2]->link_[1]->red_ = 0; 
								} 
							} 	
						}
					}
				}
				// Replace and remove if found
				if(f != 0)
				{
					f->value_ = q->value_;
                    p->link_[p->link_[1] == q] = q->link_[q->link_[0] == 0];
					
					delete q;
				} 
				// Update root and make it black
				root_ = head.link_[1]; 
				
				if(root_ != 0)
					root_->red_ = 0;
			} 
		}

		T* find(const T& data)
		{
            RedBlackTreeNode<T>* it = root_;

            while(it != 0)
            {
				int cmp = (it->value_ == data);
                
				if(cmp == 1)
                    break;
				
				if(it->value_ < data)
					cmp = -1;
				else
					cmp = 1;

				it = it->link_[cmp < 0];
            }

            return it == 0 ? 0 : &it->value_;
        }

		inline void clear()
		{
			destroyAll();
		}

	private:
		
		friend class RedBlackTreeIterator<T>;

		inline bool isRed(RedBlackTreeNode<T>* node)
		{
			if(node == 0)
				return false;
			
			return node->red_ != 0;
		}
 
		inline RedBlackTreeNode<T>* makeNode(T& data)
		{
			RedBlackTreeNode<T>* newNode = new RedBlackTreeNode<T>();
			
			newNode->value_ = data;

            return newNode;
		}
		
		RedBlackTreeNode<T>* rotateSingle(RedBlackTreeNode<T>* root,int dir)
		{
			 RedBlackTreeNode<T>* save = root->link_[!dir];
			 
			 root->link_[!dir] = save->link_[dir];
			 save->link_[dir] = root;
			 
			 root->red_ = 1;
			 save->red_ = 0;
			 
			 return save;
		}
		
		RedBlackTreeNode<T>* rotateDouble(RedBlackTreeNode<T>* root,int dir)
		{
			root->link_[!dir] = rotateSingle(root->link_[!dir],!dir);
			
			return rotateSingle(root,dir);
		}
		
		bool isTreeValid(RedBlackTreeNode<T>* root)
		{
			bool lh, rh;
 
			if(root == 0)
				return true;
			else 
			{
				RedBlackTreeNode<T>* ln = root->link_[0];
				RedBlackTreeNode<T>* rn = root->link_[1];
 
				// Consecutive red links
				if(isRed(root))
				{
					if(isRed(ln) || isRed(rn)) 
					{
						// "Red violation"
						return false;
					}
				}
 
				lh = isTreeValid(ln);
				rh = isTreeValid(rn);
	
				// Invalid binary search tree 
				if((ln != 0 && ln->value_ >= root->value_) ||
				   (rn != 0 && rn->value_ <= root->value_))
				{
					// "Binary tree violation"
					return false;
				}
	
				// Black height mismatch
				if(lh && rh && lh != rh ) 
				{
					// "Black violation"
					return false;
				}
			}
		}

		void destroyAll()
        {
			RedBlackTreeNode<T>* it = root_;
			RedBlackTreeNode<T>* save;
			
			while(it != 0) 
			{
				if(it->link_[0] == 0) 
				{
					// No left links, just kill the node and move on
					save = it->link_[1];
					delete it;

				}

				else 
				{
					//Rotate away the left link and check again */
					save = it->link_[0];
					it->link_[0] = save->link_[1];
					save->link_[1] = it;
				}

				it = save;
			}

			root_ = 0;
		}

		RedBlackTreeNode<T>* root_;
	};

	template <class T> class VPL_API RedBlackTreeIterator
	{
	public:
		
		RedBlackTreeIterator():current_(0),top_(0){}
		~RedBlackTreeIterator(){}

		
		inline void begin(RedBlackTree<T>& tree)
		{
			start(tree,true);
		}
		inline void beginReversed(RedBlackTree<T>& tree)
		{
			start(tree,false);
		}
		inline void operator ++() 
		{
			move(1);
		}
		inline void operator --() 
		{
			move(0);
		}
		inline T& operator *() 
		{
			return current_->value_;
		}
		inline bool atLast() const
		{
			return current_ == 0;
		}

	private:
		
		void start(RedBlackTree<T>& tree,bool ascending)
		{
			current_ = tree.root_;
			top_ = 0;
			int dir;

			if(ascending)
				dir = 0;
			else
				dir = 1;

			if(current_ != 0)
			{
				while(current_->link_[dir] != 0 ) 
				{
					path_[top_++] = current_;
					current_ = current_->link_[dir];
				}
			}
		}

		void move(int dir)
		{
			if(current_->link_[dir] != NULL ) 
			{
				// Continue down this branch 
				path_[top_++] = current_;
			   current_ =current_->link_[dir];

				while(current_->link_[!dir] != NULL ) 
				{
					path_[top_++] = current_;
					current_ = current_->link_[!dir];
				}
			}
			else 
			{
				// Move to the next branch 
				RedBlackTreeNode<T>* last;

				do
				{
					if(top_ == 0 ) 
					{
						current_ = 0;
						break;
					}

					last = current_;
					current_ = path_[--top_];
				} while(last == current_->link_[dir]);
			}
		}

		RedBlackTreeNode<T>* current_;              // Current node
		RedBlackTreeNode<T>* path_[cMaxTreeHeight]; // Traversal path 
		vplUint top_;                               // Top of stack 
	};
}
#endif
