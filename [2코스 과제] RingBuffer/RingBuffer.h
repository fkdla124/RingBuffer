#pragma once

// #define min(a, b) ((a) < (b) ? (a) : (b))

class RingBuffer
{
private:
	char* buffer;
	size_t _capacity;
	size_t _size;
	size_t maxCapacity;

	int front;
	int rear;

public:
	char* GetBuffer() const
	{
		return buffer;
	}

	// front ptr
	char* GetFront() const
	{
		return &buffer[front];
	}

	// rear ptr
	char* GetRear() const
	{
		return &buffer[rear];
	}

	// 데이터를 빼는 것과 동일.
	int MoveFront(size_t len)
	{
		if (len >= _size)
		{
			front = (front + _size) % _capacity;

			int ret = _size;
			_size = 0;
			return ret;
		}
		else
		{
			front = (front + len) % _capacity;
			_size -= len;
			return len;
		}
	}

	// 데이터를 넣는 것과 동일.
	int MoveRear(size_t len)
	{
		int freeSpace = _capacity - _size;
		if (len >= freeSpace)
		{
			rear = (rear + freeSpace) % _capacity;
			_size += freeSpace;
			return freeSpace;
		}
		else
		{
			rear = (rear + len) % _capacity;
			_size += len;
			return len;
		}
	}

	// 순환하지않고 push할 수 있는 사이즈
	int DirectPushSize() const
	{
		if (full())
			return 0;

		if (rear < front)
		{
			return front - rear;
		}
		else
		{
			return _capacity - rear;
		}
	}

	// 순환하지않고 pop할 수 있는 사이즈
	int DirectPopSize() const
	{
		if (empty())
			return 0;

		if (rear <= front)
		{
			return _capacity - front;
		}
		else
		{
			return rear - front;
		}
	}

public:
	RingBuffer(const RingBuffer& src) = delete;
	RingBuffer(RingBuffer&& src) = delete;
	RingBuffer& operator=(const RingBuffer& src) = delete;

	RingBuffer() : _capacity(1024), maxCapacity(1024 * 2), _size(0), front(0), rear(0)
	{
		buffer = new char[_capacity];
	}

	RingBuffer(int capacity) : _capacity(capacity > 0 ? capacity : 1024), maxCapacity(_capacity * 2), _size(0), front(0), rear(0)
	{
		buffer = new char[_capacity];
	}

	~RingBuffer()
	{
		delete[] buffer;
	}

	bool empty() const
	{
		return _size == 0;
	}

	bool full() const
	{
		return _capacity == _size;
	}

	size_t size() const
	{
		return _size;
	}

	size_t capacity() const
	{
		return _capacity;
	}

	size_t space() const
	{
		return _capacity - _size;
	}

	void clear()
	{
		front = 0;
		rear = 0;
		_size = 0;
	}

	// 원래 사이즈의 2배까지 가능. 줄이는 것은 불가능.
	bool resize(size_t capacity)
	{
		if (capacity == 0 || capacity < _capacity || capacity > maxCapacity)
			return false;

		if (capacity == _capacity)
			return true;

		char* newBuffer = new char[capacity];

		int i = 0;
		for (; i < _size; i++)
			newBuffer[i] = buffer[(front + i) % _capacity];

		_capacity = capacity;
		front = 0;
		rear = i;

		delete[] buffer;
		buffer = newBuffer;

		return true;
	}

	int push(const char* data, size_t len)
	{
		// data nullptr 체크는 안함.
		// 그자리에서 뻗게 만들어야 디버깅 할 수 있을 것으로 판단.

		if (full() && _capacity == maxCapacity)
			return 0;

		// 공간이 부족할시
		int freeSpace = _capacity - _size;
		if (freeSpace < len)
		{
			// 더 필요한 사이즈
			int needSize = len - freeSpace;

			// 새로 할당할 사이즈
			int newSize = _capacity + ((needSize + 63) / 64) * 64;

			// 기존보다 두배 초과하면 두배로 제한
			if (newSize > maxCapacity)
				newSize = maxCapacity;

			resize(newSize);
		}

		freeSpace = _capacity - _size;

		int i;
		int count = min(freeSpace, len);
		for (i = 0; i < count; i++)
		{
			buffer[rear] = *data++;
			rear = (rear + 1) % _capacity;
			_size++;
		}
		return i;
	}

	int pop(char* dest, size_t len)
	{
		// dest nullptr 체크는 안함.
		// 그자리에서 뻗게 만들어야 디버깅 할 수 있을 것으로 판단.

		if (empty())
			return 0;

		int i;
		int count = min(len, _size);
		for (i = 0; i < count; i++)
		{
			*dest++ = buffer[front];
			front = (front + 1) % _capacity;
			_size--;
		}
		return i;
	}

	// front에서 dest로 _size바이트 복사.
	int peek(char* dest, size_t len) const
	{
		// dest nullptr 체크는 안함.
		// 그자리에서 뻗게 만들어야 디버깅 할 수 있을 것으로 판단.

		if (empty())
			return 0;

		int i;
		int count = min(len, _size);
		for (i = 0; i < count; i++)
			*dest++ = buffer[(front + i) % _capacity];

		return i;
	}

	//// 디버깅 print
	//void print()
	//{
	//	printf("\n");
	//	printf("=================================================================================================== RingBuffer ==========================================================================================\n");
	//	printf("Capacity: %02d | Size: %02d | Free: %02d\n\n",
	//		_capacity, _size, _capacity - _size);

	//	// Index
	//	printf("Index  : ");
	//	for (int i = 0; i < _capacity; i++)
	//		printf("%02d ", i);
	//	printf("\n");

	//	// Front / Rear 위치
	//	printf("Pointer: ");
	//	for (int i = 0; i < _capacity; i++)
	//	{
	//		if (i == front && i == rear)
	//			printf("FR ");
	//		else if (i == front)
	//			printf(" F ");
	//		else if (i == rear)
	//			printf(" R ");
	//		else
	//			printf("   ");
	//	}
	//	printf("\n");

	//	// 데이터
	//	printf("Data   : ");
	//	for (int i = 0; i < _capacity; i++)
	//	{
	//		// 현재 데이터가 존재하는 영역인지 확인
	//		int distance = (i - front + _capacity) % _capacity;

	//		if (distance < _size)
	//		{
	//			char c = buffer[i];

	//			// 출력하기 어려운 문자 처리
	//			if (c >= 32 && c <= 126)
	//				printf(" %c ", c);
	//			else
	//				printf("%02X ", (unsigned char)c);
	//		}
	//		else
	//		{
	//			printf(" . ");
	//		}
	//	}
	//	printf("\n\n");

	//	printf("=========================================================================================================================================================================================================\n");
	//}
};