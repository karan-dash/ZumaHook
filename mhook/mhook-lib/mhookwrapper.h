#ifndef MHOOKWRAPPER_H
#define MHOOKWRAPPER_H

#include "windows.h"
#include "mhook.h"
#include <iostream>


class Register {
public:
	unsigned long value;
	Register &operator [](int index) {
		return *(Register *)((char*)value+index);
	}
	Register &operator [](Register &index) {
		return *(Register*)((char*)((long)index+value));
	}
	operator long&()  {
		return (long&)value;
	}
	operator unsigned long&()  {
		return value;
	}
	operator short&()  {
		return (short&)value;
	}
	operator char&()  {
		return (char&)value;
	}
	operator char*()  {
		return (char *)value;
	}
	DWORD &operator = (const DWORD val) {
		value = val;
	}

};

class Context{
public:
	Register EDI;
	Register ESI;
	Register EBP;
	Register ESP;
	Register EBX;
	Register EDX;
	Register ECX;
	Register EAX;
public:
	Register &operator [](int index) {
		return *(Register*)((char*)((long)index));
	}
	Register &operator [](Register &index) {
		return *(Register*)((char*)((long)index.value));
	}
};

template <class T>
DWORD operator+(Register t, T i) {
	return t.value + i;
}
template <class T>
DWORD operator-(Register t, T i) {
	return t.value + i;
}
template <class T>
DWORD operator*(Register t, T i) {
	return t.value + i;
}
template <class T>
DWORD operator/(Register t, T i) {
	return t.value + i;
}

template <class T>
DWORD operator+(T i, Register t) {
	return t.value + i;
}

template <class T>
DWORD operator-(T i, Register t) {
	return i - t.value;
}

template <class T>
DWORD operator*(T i, Register t) {
	return t.value * i;
}

std::ostream &operator<<(std::ostream & stream,  Register t);

void RegisterHook(long hookedFunction, long hook);

#endif
