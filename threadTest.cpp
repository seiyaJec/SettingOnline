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
	cout << "��������́F";
	cin >> num;
	SetNumber(num);
	endFlag = true;
	cout << "�X���b�h1�I��" << endl;
}

void thread2()
{
	while (!endFlag)
	{
	}
	cout << "����" << GetNumber() << "�������Ă��܂���";


	cout << "�X���b�h2�I��";
}


int main()
{
	thread th1(thread2);
	thread th2(thread1);

	th1.join();
	th2.join();


}