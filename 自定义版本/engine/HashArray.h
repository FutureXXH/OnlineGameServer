
#ifndef __HashArray_H
#define __HashArray_H
#include <iostream>

//哈希数组
template <class T>
class HashArray
{
public:
	int length;
	int size;
	//void* pointer;
	T* pointer;

	HashArray()
	{
		size = sizeof(T);
		length = 0;
		pointer = nullptr;
	}

	HashArray(int counter)
	{
		if (counter < 1)return;
		size = sizeof(T);
		if (size == 0) return;
		length = counter;
		pointer = new T[length];
		//pointer = malloc(size * length);


	}


	virtual ~HashArray()
	{
		if (pointer != nullptr)
		{
			//free(pointer);
			delete[] pointer;
			pointer = nullptr;
		}
	}

	T* at(const int index)
	{
		//T* temp = (T*)pointer;
		return &pointer[index];
	}
};


#endif // !__HashArray_H

