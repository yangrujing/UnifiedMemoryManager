#include<iostream>
#include<string>
#include <stdlib.h>
#include<mutex>
#include<stdarg.h> 
#include"List.hpp"
#pragma warning(disable:4996)
using namespace std;

//ѡ�񱣴浽����̨�����ļ�
enum SaveOptions
{
	SAVE_TO_CONSOLE = 1,		 // ���浽����̨
	SAVE_TO_FILE = 2,			 // ���浽�ļ�
};

class SaveAdapter
{
public:
	virtual void Save(char* format, ...) = 0; //���麯���������಻��ʵ����������
};

//����̨����������
class ConsoleSaveAdapter :public SaveAdapter
{
public:
	virtual void Save(char* format, ...);
};

//�ļ�����������
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

//����ģʽ������ģʽ���ڵ�һ�ε���GetInstanceʱ����ʵ������
template <class T>
class Singleton
{
public:
	static T* GetInstance()
	{
		if (_sInstance == NULL) //���Ч�ʣ������ظ�����
		{
			//������֤�̰߳�ȫ�������������Զ���������ֹnew���쳣ʱû�н���
			unique_lock<mutex> lock(_mutex);
			if (_sInstance == NULL)
			{
				_sInstance = new T();
			}
		}
		return _sInstance;
	}
protected:
	//���죬�������죬��ֵ���������ض�����Ϊ˽�е�
	Singleton()
	{}
	Singleton(const Singleton<T>&);
	Singleton& operator= (const Singleton<T>&);

	static T* _sInstance; //����ʵ������
	static mutex _mutex;	// ����������

};
//����Ծ�̬��Ա������ʼ��
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

//�ڴ����Ϣ
struct MemoryBlockInfo
{
	friend ostream& operator<<(ostream& os, const MemoryBlockInfo& m);
	friend class MemoryManager;
public:
	MemoryBlockInfo(void* ptr = "", string filename = "", int line = 0);
	bool operator==(const MemoryBlockInfo& m);

private:
	void* _ptr; //�����ڴ��ַ
	string _filename; //�����ڴ溯�����ڵ��ļ���
	int _line;  //�к�
};

class MemoryManager : public Singleton<MemoryManager>
{
	friend class Singleton<MemoryManager>;
public:
	//���ٿռ䱣���ڴ����Ϣ��������
	void* Alloc(size_t size, string filename, int line);
	//�ͷſռ䣬���ڴ����Ϣ�ӽڵ�ɾ��
	void Dealloc(void* ptr);
	static void OutPut();
protected:
	//�����캯���Ϳ������춨��Ϊ˽�У���֤ȫ��ֻ��Ψһʵ��
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
	//new�Ķ�λ���ʽ�����ѷ����ԭʼ�ڴ�ռ��г�ʼ��һ������
	//new(p1)int(3),p1��int*����3��ʼ��p1ָ��Ŀռ�
	//new(s1)String������Ĭ�ϵĻ����Զ���Ĺ��캯����s1������г�ʼ��
}

//ģ�岻֧�ַ������
template<class T>
inline void _DELETE(T* ptr)
{
	if (ptr)
	{
		ptr->~T(); //�������ͻ��Զ��������
		MemoryManager::GetInstance()->Dealloc(ptr); //�ҵ��ڵ�ɾ�����ͷſռ�
	}
}

template<class T>
inline T* _NEW_ARRAY(size_t num, string filename, int line)
{
	T* ptr = (T*)MemoryManager::GetInstance()->Alloc(sizeof(T)* num + 4, filename, line); //�࿪��4���ֽڴ洢����T���͵ĸ���
	*(int*)ptr = num; //��ǰ�ĸ��ֽ�ǿתΪint*�������ø�ֵΪ����T���͵ĸ���
	ptr = (T*)((int*)ptr + 1); //�����Ŀռ��Ǹ�ptr+4�ֽ�֮��Ŀռ䡣ָ��+1���ӵ������͵Ĵ�С
	for (size_t i = 0; i < num; i++)
	{
		new(ptr + i)T;
	}
	return ptr;
}

template<class T>
inline void _DELETE_ARRAY(T* ptr)
{
	T* selfptr = (T*)((int*)ptr - 1); //�ͷ�ʱҪ���࿪�ٵ�4�ֽڿռ�һ���ͷ�
	int num = *((int*)ptr - 1); //ȡ������T���͵ĸ���

	for (int i = 0; i < num; i++)
	{
		ptr[i].~T(); //�������ͻ��Զ��������
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