#include<iostream>
#include<string>
#include <stdlib.h>
#include<mutex>
#include<stdarg.h> 
#include"List.hpp"
#pragma warning(disable:4996)
using namespace std;

//选择保存到控制台还是文件
enum SaveOptions
{
	SAVE_TO_CONSOLE = 1,		 // 保存到控制台
	SAVE_TO_FILE = 2,			 // 保存到文件
};

class SaveAdapter
{
public:
	virtual void Save(char* format, ...) = 0; //纯虚函数，抽象类不能实例化出对象
};

//控制台保存适配器
class ConsoleSaveAdapter :public SaveAdapter
{
public:
	virtual void Save(char* format, ...);
};

//文件保存适配器
class FileSaveAdapter :public SaveAdapter
{
public:
	FileSaveAdapter(char *filename = "");
	~FileSaveAdapter();
	virtual void Save(char* format, ...);
private:
	FileSaveAdapter(const FileSaveAdapter& fsa);
	FileSaveAdapter& operator= (const FileSaveAdapter& fsa);
	FILE* _fOut;
};

//单例模式（懒汉模式，在第一次调用GetInstance时创建实例对象）
template <class T>
class Singleton
{
public:
	static T* GetInstance()
	{
		if (_sInstance == NULL) //提高效率，避免重复加锁
		{
			//加锁保证线程安全，出了作用域自动解锁，防止new抛异常时没有解锁
			unique_lock<mutex> lock(_mutex);
			if (_sInstance == NULL)
			{
				_sInstance = new T();
			}
		}
		return _sInstance;
	}
protected:
	//构造，拷贝构造，赋值操作符重载都定义为私有的
	Singleton()
	{}
	Singleton(const Singleton<T>&);
	Singleton& operator= (const Singleton<T>&);

	static T* _sInstance; //单个实例对象
	static mutex _mutex;	// 互斥锁对象

};
//类外对静态成员变量初始化
template <class T>
T*  Singleton<T>::_sInstance = NULL;
template <class T>
mutex Singleton<T>::_mutex;

class ConfigManager : public Singleton<ConfigManager>
{
	friend class Singleton<ConfigManager>;
public:
	int GetOption()
	{
		return _flag;
	}
	void SetOption(int opt)
	{
		_flag |= opt;
	}
	void ResetOption(int opt)
	{
		_flag &= ~opt;
	}
private:
	ConfigManager()
		:_flag(0)
	{}
	ConfigManager(const ConfigManager& con);
	ConfigManager& operator=(const ConfigManager& con);
	int _flag;
};

//内存块信息
struct MemoryBlockInfo
{
	friend ostream& operator<<(ostream& os, const MemoryBlockInfo& m);
	friend class MemoryManager;
public:
	MemoryBlockInfo(void* ptr = "", string filename = "", int line = 0);
	bool operator==(const MemoryBlockInfo& m);

private:
	void* _ptr; //开辟内存地址
	string _filename; //开辟内存函数所在的文件名
	int _line;  //行号
};

class MemoryManager : public Singleton<MemoryManager>
{
	friend class Singleton<MemoryManager>;
public:
	//开辟空间保存内存块信息到链表中
	void* Alloc(size_t size, string filename, int line);
	//释放空间，将内存块信息从节点删除
	void Dealloc(void* ptr);
	static void OutPut();
protected:
	//将构造函数和拷贝构造定义为私有，保证全局只有唯一实例
	MemoryManager();
	MemoryManager(const MemoryManager& m);
	MemoryManager& operator=(const MemoryManager& m);
	void _OutPut(SaveAdapter& sa);

	List<MemoryBlockInfo> list;
};

template<class T>
inline T* _NEW(size_t size, string filename, int line)
{
	return new(MemoryManager::GetInstance()->Alloc(sizeof(T), filename, line))T;
	//new的定位表达式，在已分配的原始内存空间中初始化一个对象。
	//new(p1)int(3),p1是int*，用3初始化p1指向的空间
	//new(s1)String，调用默认的或者自定义的构造函数对s1对象进行初始化
}

//模板不支持分离编译
template<class T>
inline void _DELETE(T* ptr)
{
	if (ptr)
	{
		ptr->~T(); //内置类型会自动跳过这句
		MemoryManager::GetInstance()->Dealloc(ptr); //找到节点删除并释放空间
	}
}

template<class T>
inline T* _NEW_ARRAY(size_t num, string filename, int line)
{
	T* ptr = (T*)MemoryManager::GetInstance()->Alloc(sizeof(T)* num + 4, filename, line); //多开辟4个字节存储开辟T类型的个数
	*(int*)ptr = num; //将前四个字节强转为int*，解引用赋值为开辟T类型的个数
	ptr = (T*)((int*)ptr + 1); //真正的空间是给ptr+4字节之后的空间。指针+1，加的是类型的大小
	for (size_t i = 0; i < num; i++)
	{
		new(ptr + i)T;
	}
	return ptr;
}

template<class T>
inline void _DELETE_ARRAY(T* ptr)
{
	T* selfptr = (T*)((int*)ptr - 1); //释放时要将多开辟的4字节空间一起释放
	int num = *((int*)ptr - 1); //取出开辟T类型的个数

	for (int i = 0; i < num; i++)
	{
		ptr[i].~T(); //内置类型会自动跳过这句
	}
	MemoryManager::GetInstance()->Dealloc(selfptr);

}

class String
{
public:
	String(char* ptr = "");
	~String();
private:
	char* _ptr;
};

#define NEW(type)  _NEW<type>(sizeof(type),__FILE__,__LINE__)
#define DELETE(ptr)   _DELETE(ptr)

#define NEW_ARRAY(type,num) _NEW_ARRAY<type>(num,__FILE__,__LINE__)
#define DELETE_ARRAY(ptr)  _DELETE_ARRAY(ptr)

#define SET_CONFIG_OPTION(flag)  ConfigManager::GetInstance()->SetOption(flag);