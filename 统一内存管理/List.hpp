#include<iostream>
#include<assert.h>
using namespace std;

template<typename T>
class LinkNode
{
	template<class T>
	friend class List;
	friend class MemoryManager;
public:
	LinkNode(const T& x)
		:_data(x)
		, _prev(NULL)
		, _next(NULL)
	{}
private:
	T _data;
	LinkNode* _prev;
	LinkNode* _next;
};


template<class T>
class List
{
	friend class MemoryManager;
public:
	List() :_Head(NULL), _Tail(NULL)
	{}
	~List()
	{
		LinkNode<T> *begin = _Head;
		while (begin)
		{
			_Head = _Head->_next;
			delete begin;
			begin = _Head;
		}
	}
public:
	LinkNode<T>* GetHead()
	{
		return _Head;
	}

	void print()
	{
		assert(this);
		if (_Head == NULL)
		{
			return;
		}
		LinkNode<T>* begin = _Head;
		while (begin)
		{
			cout << begin->_data;
			begin = begin->_next;
		}
	}
	void PushBack(const T& x)
	{
		assert(this);
		if (_Head == NULL)  //没有节点
		{
			_Head = new LinkNode<T>(x);
			_Tail = _Head;
		}
		else
		{
			LinkNode<T>* tmp = new LinkNode<T>(x);
			_Tail->_next = tmp;
			tmp->_prev = _Tail;
			_Tail = tmp;
		}
	}

	void PopBack()
	{
		assert(this);
		if (_Head == NULL)
		{
			cout << "List is  empty" << endl;
		}
		if (_Head == _Tail)
		{
			delete _Tail;
			_Head = NULL; //释放完都要置空
			_Tail = NULL;
		}
		else
		{
			_Tail = _Tail->_prev;
			delete _Tail->_next;
			_Tail->_next = NULL;
		}
	}

	void PushFront(const T& x)
	{
		assert(this);
		if (_Head == NULL)  //没有节点
		{
			_Head = new LinkNode<T>(x);
			_Tail = _Head;
		}
		else
		{
			LinkNode<T>* tmp = new LinkNode<T>(x);
			tmp->_next = _Head;
			_Head->_prev = tmp;
			_Head = tmp;
		}
	}

	void PopFornt()
	{
		assert(this);
		if (_Head == NULL)
		{
			cout << "List is  empty" << endl;
		}
		if (_Head == _Tail)
		{
			delete _Tail;
			_Head = NULL; //释放完都要置空
			_Tail = NULL;
		}
		else
		{
			_Head = _Head->_next;
			delete _Head->_prev;
			_Head->_prev = NULL;
		}
	}

	LinkNode<T>* Find(const T& x)
	{
		assert(this);
		LinkNode<T>* begin = _Head;
		while (begin)
		{
			if (begin->_data == x)
			{
				//cout << "Find " << x << endl;
				return begin;
			}
			begin = begin->_next;
		}
		//cout << "not find " << x << endl;
		return NULL;
	}

	void Insert(LinkNode<T>* pos, const T& x)
	{
		assert(this);
		assert(pos);
		LinkNode<T>* tmp = new LinkNode<T>(x);
		tmp->_next = pos->_next;
		tmp->_prev = pos;
		pos->_next = tmp;
		if (pos == _Tail)
		{
			_Tail = tmp;
		}
	}

	void Erase(LinkNode<T>* del)
	{
		assert(this);
		if (del)
		{
			if (_Head == _Tail && del == _Head)
			{
				_Head = NULL;
				_Tail = NULL;
			}
			if (del == _Head)
			{
				_Head = _Head->_next;
				_Head->_prev = NULL;
			}

			if (del == _Tail)
			{
				_Tail = _Tail->_prev;
				_Tail->_next = NULL;
			}
		}
		else
		{
			LinkNode<T> *prev = del->_prev;
			prev->_next = del->_next;
			del->_next->_prev = prev;
		}

		delete del;
	}
private:
	LinkNode<T>* _Head;
	LinkNode<T>* _Tail;

};

void Print()
{

}