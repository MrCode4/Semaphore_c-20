#include <bits/stdc++.h>

using namespace std;

const int X = 100, Y = 1, Z = 10;

std::counting_semaphore<2> enter_signal(2);  
std::counting_semaphore<1> exit_signal(1);
std::counting_semaphore<X> stay_store_signal(X);

std::atomic<int> len_of_entering_saaf = 0;
std::atomic<int> num_of_persons_in_store = 0;
std::atomic<int> len_of_cash1_saaf = 0;
std::atomic<int> len_of_cash2_saaf = 0;
std::atomic<int> len_of_cash3_saaf = 0;
std::atomic<int> len_of_exiting_saaf = 0;
std::atomic<bool> it_happened = false;

std::mutex mut;

void person_thread()
{
	len_of_entering_saaf++;

	//cout << "Person now waiting in queue to enter store.\n";

	enter_signal.acquire();

	//cout << "Person now wanna enter to store.\n";

	stay_store_signal.acquire();

	num_of_persons_in_store++;

	enter_signal.release();

	//cout << "Person now buying in srore.\n";

	this_thread::sleep_for(std::chrono::seconds(Y));

	mut.lock();

	int len_time_out = Z;
	int which_queue = 0;

	if(len_of_cash1_saaf <= len_of_cash2_saaf && 
	   len_of_cash1_saaf <= len_of_cash3_saaf)
	{
		which_queue = 1;

		len_time_out += len_of_cash1_saaf;

		len_of_cash1_saaf++;
	}
	else if(len_of_cash2_saaf <= len_of_cash1_saaf &&
		    len_of_cash2_saaf <= len_of_cash3_saaf)
	{
		which_queue = 2;

		len_time_out += len_of_cash2_saaf;

		len_of_cash2_saaf++;
	}
	else
	{
		which_queue = 3;

		len_time_out += len_of_cash3_saaf;

		len_of_cash3_saaf++;
	}

	mut.unlock();

	//cout << "Person waiting in queue for cash.\n";

    this_thread::sleep_for(std::chrono::seconds(len_time_out));

    mut.lock();
	if(which_queue == 1)
		len_of_cash1_saaf--;
	else if(which_queue == 2)
		len_of_cash2_saaf--;
	else 
		len_of_cash3_saaf--;
	mut.unlock();

	//cout << "Cash done.\n now person wants to exiting...\n";

	mut.lock();
	len_of_exiting_saaf++;

	if(len_of_exiting_saaf >= 10)
	{
		it_happened = true;

		cout << "The exting queue is more than 10.\n";

		terminate();
	}
	else
	{
		cout << "The exting queue len is : " << len_of_exiting_saaf << endl;
	}
	mut.unlock();

	exit_signal.acquire();

	this_thread::sleep_for(std::chrono::seconds(1));
	//cout << "Person exited.\n";
	
	exit_signal.release();
	stay_store_signal.release();

	len_of_exiting_saaf--;

}

vector<thread*> threads;

int main()
{
	cout << "Started...\n" << endl;

	long long elapsed_time = 0;
	while(!it_happened)
	{
		for(int i=1;i<=X;i++)
		{
			threads.push_back(new thread(person_thread));
		}

		this_thread::sleep_for(std::chrono::seconds(1));
		elapsed_time++;

		cout << "Waiting time : " << elapsed_time << endl;
	}	

	for(int i=1;i<threads.size();i++)
		threads[i]->join();

	cout << "Finished!\n";

	return 0;
}