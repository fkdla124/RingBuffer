#include <iostream>
#include <windows.h>
#include <queue>
#include "Console.h"
#include "RingBuffer.h"

// 내부를 출력해야해서 작은 사이즈로 실험.
#define RINGBUFFERSIZE 64

int main()
{
	// 콘솔 설정
	InitConsole();
	system("mode con: cols=205 lines=35");

	// 테스트 실패 시 재현을 위해 시드 상수 고정 (내 생일)
	srand(0x20020315);

	// 나의 링버퍼
	RingBuffer myRing(RINGBUFFERSIZE);

	// stl 링버퍼 -> 체크용.
	std::queue<char> stlRing;

	// 랜덤길이 랜덤문자열 저장버퍼
	char pushStr[RINGBUFFERSIZE + 1];
	char popStr[RINGBUFFERSIZE + 1];
	char stlPopStr[RINGBUFFERSIZE + 1];

	// 테스트 횟수
	int count = 0;

	// 시간체크
	clock_t startTick = clock();

	while (1)
	{
		// 널문자 고려하여 0으로 밀기.
		memset(pushStr, 0, RINGBUFFERSIZE + 1);
		memset(popStr, 0, RINGBUFFERSIZE + 1);
		memset(stlPopStr, 0, RINGBUFFERSIZE + 1);

		// a ~ z 랜덤길이 랜덤문자열 생성
		int pushLen = rand() % RINGBUFFERSIZE + 1;
		for (int i = 0; i < pushLen; i++)
		{
			pushStr[i] = rand() % 26 + 97; // a ~ z
		}

		// 링버퍼 출력
		MoveCursor(0, 0);
		myRing.print();

		// push
		int returnValuePush = myRing.push(pushStr, pushLen);
		for (int i = 0; i < returnValuePush; i++)
		{
			stlRing.push(pushStr[i]);
		}

		// 데이터 출력
		printf("\n  Push String... [Len = %02d]\n\n  [%.*s]%s\n", returnValuePush, returnValuePush, pushStr, std::string(64, ' ').c_str());

		// 링버퍼 출력
		myRing.print();

		// 랜덤길이 pop
		int popLen = rand() % RINGBUFFERSIZE + 1;
		int returnValuePop = myRing.pop(popStr, popLen);
		for (int i = 0; i < returnValuePop; i++)
		{
			stlPopStr[i] = stlRing.front();
			stlRing.pop();
		}

		//비교
		printf("\n  Pop String... [Len = %02d]\n\n", returnValuePop);
		printf("  expected\t[%s]%s\n", stlPopStr, std::string(64, ' ').c_str());
		printf("  result\t[%s]%s\n", popStr, std::string(64, ' ').c_str());
		printf("  RingBuffer Size : %d\n", myRing.size());
		printf("  STL queue Size : %d\n", stlRing.size());
		if (memcmp(popStr, stlPopStr, RINGBUFFERSIZE) != 0 || myRing.size() != stlRing.size())
		{
			printf("\n  Test [%d] ERROR !!!!!!!!!!!!!!!!!!!!!!!!!\n\n", count);
			Sleep(INFINITE);
		}
		else
		{
			printf("\n  Test [%d] OK..\n\n", count);
			clock_t diff = clock() - startTick;
			int second = diff / 1000;
			int min = second / 60;
			int hour = min / 60;
			printf("\n  Time: %02dh, %02dm, %02ds\n", hour, min % 60, second % 60);
		}
		count++;

		// 오래 돌릴 거라서 너무 빠르게 돌지 않게.
		// 재현할때는 없애고 재현해서 빨리 도달하게 하면 좋을듯.
		Sleep(10);
	}


	return 0;
}