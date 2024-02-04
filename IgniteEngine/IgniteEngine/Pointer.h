#include "Pointer.hpp"

template<class T>
std::unordered_map<T*, uint32_t> Pointer<T>::_nb_p{};

template<class T>
int Pointer<T>::debug{ 0 };

template<class T>
Pointer<T>::Pointer() :
	_p{ nullptr },
	_offset{ 0 }
{
	;
}

template<class T>
Pointer<T>::Pointer(T* p) :
	_p{ nullptr },
	_offset{ 0 }
{
	add(p, 0);
}

template<class T>
Pointer<T>::Pointer(const Pointer<T>& p) {
	Pointer<T>& p_bis = const_cast<Pointer<T>&>(p);
	add(p_bis.data(), p_bis.offset());
}

template<class T>
Pointer<T>::Pointer(const Pointer<T>&& ptr) {
	Pointer<T>&& ptr_bis = const_cast<Pointer<T>&&>(ptr);
	_p = ptr_bis.data();
	_offset = ptr_bis.offset();
}

template<class T>
Pointer<T>::~Pointer() {
	remove();
}

template<class T>
Pointer<T>& Pointer<T>::operator=(T* p) {
	add(p, 0);
	return *this;
}

template<class T>
Pointer<T>& Pointer<T>::operator=(Pointer<T> ptr) {
	add(ptr.data(), ptr.offset());

	return *this;
}

template<class T>
Pointer<T>& Pointer<T>::operator=(Pointer<T>&& ptr) {
	_p = ptr.data();
	_offset = ptr.offset();

	return *this;
}

template<class T>
Pointer<T> operator+(const Pointer<T>& ptr, int32_t offset) {
	Pointer<T> ret(ptr);
	ret.offset() += offset;
	return ret;
}

template<class T>
Pointer<T> operator+(int32_t offset, const Pointer<T>& ptr) {
	Pointer<T> ret(ptr);
	ret.offset() += offset;
	return ret;
}

template<class T>
Pointer<T> operator-(const Pointer<T>& ptr, int32_t offset) {
	Pointer<T> ret(ptr);
	ret.offset() -= offset;
	return ret;
}

template<class T>
Pointer<T> operator-(int32_t offset, const Pointer<T>& ptr) {
	Pointer<T> ret(ptr);
	ret.offset() -= offset;
	return ret;
}

template<class T>
Pointer<T>& Pointer<T>::operator+=(int32_t offset) {
	_offset += offset;
	return *this;
}

template<class T>
Pointer<T>& Pointer<T>::operator-=(int32_t offset) {
	_offset -= offset;
	return *this;
}

template<class T>
Pointer<T>& Pointer<T>::operator++() {
	++_offset;
	return *this;
}

template<class T>
Pointer<T> Pointer<T>::operator++(int) {
	Pointer<T> ret = *this;
	++_offset;
	return ret;
}

template<class T>
Pointer<T>& Pointer<T>::operator--() {
	--_offset;
	return *this;
}

template<class T>
Pointer<T> Pointer<T>::operator--(int) {
	Pointer<T> ret = *this;
	--_offset;
	return ret;
}

template<class T>
T& Pointer<T>::operator*() {
	return *(_p + _offset);
}

template<class T>
T& Pointer<T>::operator[](int32_t i) {
	return *(_p + i + offset());
}

template<class T>
T* Pointer<T>::data() const {
	return  _p + _offset;
}

template<class T>
int32_t& Pointer<T>::offset() {
	return _offset;
}

template<class T>
void Pointer<T>::add(T* p, uint32_t offset) {
	remove();

	_p = p - offset;
	_offset = offset;

	if (!_nb_p.count(_p)) {
		_nb_p[_p] = 0;
	}
	_nb_p[_p]++;
}

template<class T>
void Pointer<T>::remove() {
	if (!_p) {
		return;
	}
	if (!_nb_p[_p]) {
		return;
	}

	--_nb_p[_p];
	if (!_nb_p[_p]) {
		_nb_p.erase(_p);
		delete[] _p;
	}
}