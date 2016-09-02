#include"MemoryManager.hpp"

/*************���浽����̨****************/
void ConsoleSaveAdapter::Save(char* format, ...)
{
	va_list args;
	va_start(args, format);
	//int vfprintf ( FILE * stream, const char * format, va_list arg );
	vfprintf(stdout, format, args); //���������̨
	va_end(args);
}

/*************���浽�ļ�****************/
FileSaveAdapter::FileSaveAdapter(char *filename)
{
	_fOut = fopen(filename, "w");
	if (_fOut == NULL)
	{
		perror("fopen");
	}
}

FileSaveAdapter::~FileSaveAdapter()
{
	if (_fOut)
		fclose(_fOut);
}
void FileSaveAdapter::Save(char* format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(_fOut, format, args);
	va_end(args);
}

/*************�ڴ����Ϣ��****************/
MemoryBlockInfo::MemoryBlockInfo(void* ptr, string filename, int line)
:_ptr(ptr)
, _filename(filename)
, _line(line)
{}
bool MemoryBlockInfo::operator == (const MemoryBlockInfo& m)
{
	return _ptr == m._ptr; //ɾ���ڵ�ʱ��ֻ��Ҫ�жϵ�ַ�Ƿ���ȼ���
}

ostream& operator<<(ostream& os, const MemoryBlockInfo& m)
{
	os << "��ַ��" << m._ptr << endl;
	os << "Filename:" << m._filename << endl;
	os << "line:" << m._line << endl;
	return os;
}

/*************�ڴ������***************/
//���ٿռ䱣���ڴ����Ϣ��������
inline void* MemoryManager::Alloc(size_t size, string filename, int line)
{
	void* ptr = malloc(size);
	if (ptr)
	{
		list.PushBack(MemoryBlockInfo(ptr, filename, line));
	}
	return ptr;
}

inline void MemoryManager::Dealloc(void* ptr)
{
	if (ptr)
	{
		LinkNode<MemoryBlockInfo>* ret = list.Find(ptr); //���ҵ��ڴ���������е�λ��
		list.Erase(ret); //��ɾ����ǰ�ڵ� 
	}
	free(ptr); //�ͷſռ�
}

void MemoryManager::OutPut()
{
	int flag = ConfigManager::GetInstance()->GetOption();

	if (flag & SAVE_TO_CONSOLE)
	{
		ConsoleSaveAdapter ssa;
		MemoryManager::GetInstance()->_OutPut(ssa);
	}
	if (flag & SAVE_TO_FILE)
	{
		FileSaveAdapter fsa("text.txt");
		MemoryManager::GetInstance()->_OutPut(fsa);
	}
}

void MemoryManager::_OutPut(SaveAdapter& sa)
{
	LinkNode<MemoryBlockInfo> *begin = list._Head;
	while (begin)
	{
		sa.Save("Ptr->%p\nFilename : %s\nLine : %d\n", \
			begin->_data._ptr, begin->_data._filename.c_str(), begin->_data._line);
		begin = begin->_next;
	}
}

MemoryManager::MemoryManager()
{
	//ע��Print�������ڳ������н�������ӡ����ڵ���Ϣ������֪���ĸ��ط�û���ͷ�
	atexit(OutPut);
}
MemoryManager::MemoryManager(const MemoryManager& m)
{}
MemoryManager& MemoryManager::operator=(const MemoryManager& m)
{
	return *this;
}

/*************string��****************/
String::String(char* ptr)
:_ptr(new char[strlen(ptr) + 1])
{
	strcpy(_ptr, ptr);
}
String::~String()
{
	if (_ptr)
	{
		delete[] _ptr;
	}
}

/*************��������****************/
void Test()
{
	int* p1 = NEW(int); //_NEW<int>(sizeof(int),__FILE__,__LINE__)
	char* p2 = NEW(char);
	String* p3 = NEW(String);

	int* p4 = NEW_ARRAY(int, 3);// _NEW_ARRAY<int>(3,__FILE__,__LINE__)
	char* p5 = NEW_ARRAY(char, 5);
	string* p6 = NEW_ARRAY(string, 2);

	DELETE(p1);
	DELETE(p2);
	DELETE(p3); //_DELETE(p3)
	DELETE_ARRAY(p4);
	DELETE_ARRAY(p5);
	//DELETE_ARRAY(p6);
}

int main()
{
	SET_CONFIG_OPTION(SAVE_TO_CONSOLE | SAVE_TO_FILE);
	Test();
	return 0;
}