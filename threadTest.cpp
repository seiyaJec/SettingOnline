#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

int number = 0;
mutex mtx;
bool endFlag = false;

void SetNumber(int num_)
{
	lock_guard<mutex> lock(mtx);
	number = num_;
}


int GetNumber()
{
	lock_guard<mutex> lock(mtx);
	return number;
}

void thread1()
{
	int num;
	cout << "数字を入力：";
	cin >> num;
	SetNumber(num);
	endFlag = true;
	cout << "スレッド1終了" << endl;
}

void thread2()
{
	while (!endFlag)
	{
	}
	cout << "数字" << GetNumber() << "が送られてきました";


	cout << "スレッド2終了";
}


int main()
{
	thread th1(thread2);
	thread th2(thread1);

	th1.join();
	th2.join();


}