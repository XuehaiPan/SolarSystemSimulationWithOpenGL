//
// SeqQueue.h
// SeqQueue
//
// Created by 潘学海.
// Copyright © 2016 潘学海. All rights reserved.
//

#pragma once

#ifndef _SEQQUEUE_H_

#define _SEQQUEUE_H_

#ifdef MAXSIZE
#undef MAXSIZE
#endif // MAXSIZE

#define MAXSIZE 100

using namespace std;

template <class T>
class SeqQueue //顺序队列类
{
protected:
	T *key;
	unsigned int head, tail, maxsize;

public:
	SeqQueue(const unsigned int tsize = MAXSIZE) //构造函数
			: head(0), tail(0), maxsize(tsize + 1)
	{
		key = new T[maxsize];
	}

	SeqQueue(const SeqQueue &queue) //拷贝构造函数
			: key(nullptr)
	{
		*this = queue;
	}

	SeqQueue(const T array[], const unsigned int tsize) //构造函数
			: head(0), tail(0), maxsize(tsize + MAXSIZE + 1)
	{
		key = new T[maxsize];
		for (unsigned int i = head; i < tail; ++i)
			key[i] = array[i];
	}

	virtual ~SeqQueue() //析构函数
	{
		delete[] key;
		key = nullptr;
	}

	const SeqQueue &operator=(const SeqQueue &queue) //拷贝
	{
		resize(queue.maxsize - 1);
		tail = queue.size();
		for (unsigned int i = head; i < tail; ++i)
			key[i] = queue.key[(i + queue.head) % queue.maxsize];
		return *this;
	}

	void clear() //清空队列内容
	{
		head = 0, tail = 0;
		delete[] key;
		key = new T[maxsize];
	}

	void resize(const unsigned int tsize = MAXSIZE) //重设队列大小
	{
		head = 0, tail = 0, maxsize = tsize + 1;
		delete[] key;
		key = new T[maxsize];
	}

	const unsigned int size() const //获取队列大小
	{
		return ((tail + maxsize - head) % maxsize);
	}

	const bool popFront() //在队列头删除元素
	{
		if (!isEmpty())
		{
			head = (head + 1) % maxsize;
			return true;
		}
		else
			return false;
	}

	const bool popFront(T &tkey) //在队列头删除元素
	{
		if (!isEmpty())
		{
			tkey = key[head];
			head = (head + 1) % maxsize;
			return true;
		}
		else
			return false;
	}

	const bool pushBack(const T &tkey) //在队列尾插入元素
	{
		if (!isFull())
		{
			key[tail] = tkey;
			tail = (tail + 1) % maxsize;
			return true;
		}
		else
			return false;
	}

	const T &front() const //获取队列头元素
	{
		if (!isEmpty())
			return key[head];
	}

	const bool front(T &tkey) const //获取队列头元素
	{
		if (!isEmpty())
		{
			tkey = key[head];
			return true;
		}
		else
			return false;
	}

	const bool isEmpty() const //判断是否为空队列
	{
		return (head == tail);
	}

	const bool isFull() const //判断队列是否已满
	{
		return (size() == maxsize - 1);
	}

	void forEach(void(*func)(const T &)) const //遍历
	{
		for (unsigned int i = head; i != tail; i = (i + 1) % maxsize)
			func(key[i]);
	}

	void forEach(void(*func)(T const *const)) const //遍历
	{
		for (unsigned int i = head; i != tail; i = (i + 1) % maxsize)
			func(&key[i]);
	}
};

#endif // !_SEQQUEUE_H_