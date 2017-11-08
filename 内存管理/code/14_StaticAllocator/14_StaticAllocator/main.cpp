#include <iostream>
#include <string>
#include<complex>

class allocator
{
private:
	// 结构体obj是一个自身的指针，制作
	struct obj
	{
		struct obj* next;		// embedded pointer
	};
public:
	void* allocate(size_t);
	void deallocate(void*, size_t);
private:
	obj* freeStore = nullptr;
	const int CHUNK = 5;
};

void* allocator::allocate(size_t size)
{
	obj* p;

	if (!freeStore)
	{
		// linked list为空,于是申请一大块
		size_t chunk = CHUNK * size;
		freeStore = p = (obj*)malloc(chunk);

		// 将分配得到的大块切分成小块
		for (int i = 0; i < (CHUNK - 1); ++i)
		{
			p->next = (obj*)((char*)p + size);
			p = p->next;
		}
		p->next = nullptr;		// last
	}
	p = freeStore;
	freeStore = freeStore->next;
	return p;
}

void allocator::deallocate(void* p, size_t)
{
	// 将*p收回插入free list前端
	((obj*)p)->next = freeStore;
	freeStore = (obj*)p;
}

class Foo {
public:
	long L;
	std::string str;
	static allocator myAlloc;
public:
	Foo(long l):L(l){}
	static void* operator new(size_t size)
	{
		return myAlloc.allocate(size);
	}
	static void operator delete(void* pdead, size_t size)
	{
		return myAlloc.deallocate(pdead, size);
	}
};
allocator Foo::myAlloc;

class Foo2 {
public:
	long L;
	std::string str;
public:
	Foo2(long l) :L(l) {}
};

class Goo {
public:
	std::complex<double> c;
	std::string str;
	static allocator myAlloc;
public:
	Goo(const std::complex<double> &x):c(x){}
	static void* operator new(size_t size)
	{
		return myAlloc.allocate(size);
	}
	static void operator delete(void* pdead, size_t size)
	{
		return myAlloc.deallocate(pdead, size);
	}
};
allocator Goo::myAlloc;

int main(int argc, char* argv[])
{
	Foo* p[100];
	
	std::cout << "sizeof(Foo)=" << sizeof(Foo) << std::endl;
	for (int i = 0; i < 23; ++i)
	{
		p[i] = new Foo(i);
		std::cout << p[i] << ' ' << p[i]->L << std::endl;
	}

	for (int i = 0; i < 23; ++i)
	{
		delete p[i];
	}

	Foo2* pFoo2[100];

	std::cout << "sizeof(Foo2)=" << sizeof(Foo2) << std::endl;
	for (int i = 0; i < 23; ++i)
	{
		pFoo2[i] = new Foo2(i);
		std::cout << pFoo2[i] << ' ' << pFoo2[i]->L << std::endl;
	}

	Goo* pG[100];

	std::cout << "sizeof(Goo)=" << sizeof(Goo) << std::endl;
	for (int i = 0; i < 17; ++i)
	{
		pG[i] = new Goo(std::complex<double>(i, i));
		std::cout << pG[i] << ' ' << pG[i]->c << std::endl;
	}

	for (int i = 0; i < 17; ++i)
	{
		delete pG[i];
	}
	return 0;
}