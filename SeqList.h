//
// SeqList.h
// SeqList
//
// Created by 潘学海.
// Copyright © 2016 潘学海. All rights reserved.
//

#pragma once

#ifndef _SEQLIST_H_

#define _SEQLIST_H_

#ifdef MAXSIZE
#undef MAXSIZE
#endif // MAXSIZE

#define MAXSIZE 100

using namespace std;

template <class T>
class SeqList //顺序表类
{
protected:
	T *key;
	unsigned int head, tail, maxsize;

public:
	class iterator //迭代器类
	{
		friend class SeqList<T>;
	protected:
		T *ptr;

		iterator(T * const p)
			:ptr(p) {}

	public:
		iterator()
			:ptr(nullptr) {}

		iterator(const iterator &p)
			:ptr(p.ptr) {}

		virtual ~iterator()
		{
			ptr = nullptr;
		}

		const iterator & operator=(const iterator &p)
		{
			ptr = p.ptr;
			return *this;
		}

		T & operator*() const
		{
			return *ptr;
		}

		T & operator[](const int n) const
		{
			return ptr[n];
		}

		const iterator & operator++()
		{
			++ptr;
			return *this;
		}

		const iterator operator++(int)
		{
			iterator temp(ptr);
			++ptr;
			return temp;
		}

		const iterator operator+(const int n) const
		{
			return iterator(ptr + n);
		}

		const iterator & operator+=(int n)
		{
			ptr += n;
			return *this;
		}

		const iterator & operator--()
		{
			--ptr;
			return *this;
		}

		const iterator operator--(int)
		{
			iterator temp(ptr);
			--ptr;
			return temp;
		}

		const iterator operator-(const int n) const
		{
			return iterator(ptr - n);
		}

		const iterator & operator-=(const int n)
		{
			ptr -= n;
			return *this;
		}

		const int operator-(const iterator &p) const
		{
			return (ptr - p.ptr);
		}

		const bool operator==(const iterator &p) const
		{
			return (ptr == p.ptr);
		}

		const bool operator!=(const iterator &p) const
		{
			return (ptr != p.ptr);
		}

		const bool operator<(const iterator &p) const
		{
			return (p.ptr - ptr > 0);
		}

		const bool operator>(const iterator &p) const
		{
			return (ptr - p.ptr > 0);
		}

		const bool operator<=(const iterator &p) const
		{
			return (p.ptr - ptr >= 0);
		}

		const bool operator>=(const iterator &p) const
		{
			return (ptr - p.ptr >= 0);
		}
	};

	SeqList(const unsigned int tsize = MAXSIZE) //构造函数
		:head(0), tail(0), maxsize(tsize)
	{
		key = new T[maxsize];
	}

	SeqList(const SeqList &list) //拷贝构造函数
		:key(nullptr)
	{
		*this = list;
	}

	SeqList(const T array[], const unsigned int tsize) //构造函数
		:head(0), tail(0), maxsize(tsize + MAXSIZE)
	{
		key = new T[maxsize];
		for (unsigned int i = head; i < tail; ++i)
			key[i] = array[i];
	}

	virtual ~SeqList() //析构函数
	{
		delete[] key;
		key = nullptr;
	}

	const SeqList & operator=(const SeqList &list) //拷贝
	{
		resize(list.size() + MAXSIZE);
		tail = list.size();
		for (unsigned int i = head; i < tail; ++i)
			key[i] = list[i];
		return *this;
	}

	void clear() //清空顺序表内容
	{
		delete[] key;
		key = new T[maxsize];
		head = 0, tail = 0;
	}

	void resize(const unsigned int tsize = MAXSIZE) //重设顺序表大小
	{
		delete[] key;
		key = new T[tsize];
		head = 0, tail = 0, maxsize = tsize;
	}

	const unsigned int size() const //获取顺序表大小
	{
		return (tail - head);
	}

	const iterator begin() const //获取头迭代器
	{
		return iterator(key + head);
	}

	const iterator end() const //获取超尾迭代器
	{
		return iterator(key + tail);
	}

	const bool insert(unsigned int loc, const T &tkey) //插入元素
	{
		loc += head;
		if (loc > tail)
			loc = tail;
		if (isFull())
		{
			maxsize = size() + MAXSIZE;
			T *temp = new T[maxsize];
			for (unsigned int i = head; i < tail; ++i)
				temp[i - head] = key[i];
			delete[] key;
			tail -= head;
			loc -= head;
			head = 0;
			key = temp, temp = nullptr;
		}
		for (unsigned int i = tail; i > loc; --i)
			key[i] = key[i - 1];
		key[loc] = tkey;
		++tail;
		return true;
	}

	const bool insert(const iterator &i, const T &tkey) //插入元素
	{
		return insert(i - begin(), tkey);
	}

	const bool del(unsigned int loc) //删除元素
	{
		loc += head;
		if (loc < tail)
		{
			for (unsigned int i = loc; i < tail - 1; ++i)
				key[i] = key[i + 1];
			--tail;
			return true;
		}
		else
			return false;
	}

	const bool del(const iterator &i) //删除元素
	{
		return del(i - begin());
	}

	const bool pushFront(const T &tkey) //在顺序表头插入元素
	{
		return insert(0, tkey);
	}

	const bool popFront() //在顺序表头删除元素
	{
		if (!isEmpty())
		{
			++head;
			return true;
		}
		else
			return false;
	}

	const bool pushBack(const T &tkey) //在顺序表尾插入元素
	{
		return insert(tail - head, tkey);
	}

	const bool popBack() //在顺序表尾删除元素
	{
		if (!isEmpty())
		{
			--tail;
			return true;
		}
		else
			return false;
	}

	const bool search(iterator &p, const T &tkey) const //查找元素
	{
		for (iterator i = begin(); i != end(); ++i)
		{
			if (*i == tkey)
			{
				p = i;
				return true;
			}
		}
		return false;
	}

	const bool isEmpty() const //判断是否为空表
	{
		return (head == tail);
	}

	const bool isFull() const //判断顺序表是否已满
	{
		return (tail == maxsize);
	}

	T & operator[](unsigned int loc) const //随机访问
	{
		loc += head;
		if (loc < tail)
			return key[loc];
		else
			return key[head];
	}

	void forEach(void(*func)(T &)) const //遍历
	{
		for (unsigned int i = head; i < tail; ++i)
			func(key[i]);
	}

	void forEach(const iterator &iBegin, const iterator &iEnd, void(*func)(T &)) const //遍历
	{
		for (iterator i = iBegin; i != iEnd; ++i)
			func(*i);
	}

	void forEach(void(*func)(T * const)) const //遍历
	{
		for (unsigned int i = head; i < tail; ++i)
			func(&key[i]);
	}

	void forEach(const iterator &iBegin, const iterator &iEnd, void(*func)(T * const)) const //遍历
	{
		for (iterator i = iBegin; i != iEnd; ++i)
			func(&(*i));
	}
};

#endif // !_SEQLIST_H_