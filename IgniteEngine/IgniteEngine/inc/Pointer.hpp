#pragma once
#include <iostream>
#include <unordered_map>

// Forward declaration
template<class T>
class Pointer;
template<class T>
Pointer<T> operator+(const Pointer<T>& ptr, int32_t offset);
template<class T>
Pointer<T> operator+(int32_t offset, const Pointer<T>& ptr);
template<class T>
Pointer<T> operator-(const Pointer<T>& ptr, int32_t offset);
template<class T>
Pointer<T> operator-(int32_t offset, const Pointer<T>& ptr);

template<class T>
class Pointer
{

private:
	static std::unordered_map<T*, uint32_t> _nb_p;
	static int debug;

	int deb;

	T* _p;
	int32_t _offset;


public:
	Pointer();
	Pointer(T* p);
	Pointer(const Pointer<T>& p);
	Pointer(const Pointer<T>&& p);
	~Pointer();


	Pointer<T>& operator=(T* p);
	Pointer<T>& operator=(Pointer<T> ptr);
	Pointer<T>& operator=(Pointer<T>&& ptr);

	friend Pointer<T> operator+<T>(const Pointer<T>& ptr, int32_t offset);
	friend Pointer<T> operator+<T>(int32_t offset, const Pointer<T>& ptr);
	friend Pointer<T> operator+<T>(const Pointer<T>& ptr, int32_t offset);
	friend Pointer<T> operator+<T>(int32_t offset, const Pointer<T>& ptr);

	Pointer<T>& operator+=(int32_t offset);
	Pointer<T>& operator-=(int32_t offset);

	Pointer<T>& operator++(); // prefix
	Pointer<T> operator++(int); // postfix
	Pointer<T>& operator--();
	Pointer<T> operator--(int);

	T& operator*();

	T& operator[](int32_t i);

	T* data() const;

	//int32_t offset();

private:
	void add(T* p, uint32_t offset);
	void remove();

	int32_t& offset();
	//T* og_p();

	friend class Pointer;
};
