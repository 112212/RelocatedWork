#include "RelocatedWork.hpp"
#include <iostream>
#include <thread>
#include <type_traits>
#include <utility>
std::string test(int s, int a, int b) {
	// s = 87897;
	std::cout << "mysql thread\n";
	std::cout << "state: " << s << std::endl;
	if(a > b)
		return "a > b";
	else
		return "a < b";
}

void cont(std::string a) {
	std::cout << "calling thread\n";
	std::cout << "result: " << a << std::endl;
}

int some_work(void* s, int a, int b) {
	
}

void ok(int r) {
	std::cout << r << std::endl;
}

RelocatedWork w;

void mysql_thread() {
	int c = 0;
	while(1) {
		w.Work(c);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

int main() {
	
	std::thread mysql_thrd(mysql_thread);
	mysql_thrd.detach();
	int b = 57;
	std::cout << "pos b: " << &b << "\n";
	
	
	
	// thread 1
	w.MakeWork(
		test, 5, 4
	).then(cont);
	
	
	// another work (perhaps for another packet requesting mysql, or same packet but other query)
	w.MakeWork(
		 // called when using .Continue()
		test, 4, 5 // test - some function which takes params 4 and 5, this is called on mysql thread ( .Work() )
		
	).then(cont);
	
	
	w.MakeWork(
		[](void* s, int a, int b) { return a+b; },
		10,15
	).then([](int a) { std::cout << "got: " << a << std::endl; });
		
	w.MakeWork(
		[](void* s, int a, int b) { return a+b; },
		10,15
	).then(ok);
	
	w.MakeWork(some_work, 5,7);

	while(w.HasWork()) {
		// thread 1
		w.Continue();
		std::cout << "doing other stuff while waiting for work to be done" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	
	// thread 1
	w.MakeWork(test, 5, 4).then(
		[](std::string res) { 
			std::cout << "result is: " << res << "\n";
		}
	);
	
	w.Work(&b);
	
	w.Continue();
	
	std::cout << "b value: " << b << std::endl;
	
	return 0;
}
