#pragma once
#ifndef ITHREAD_DB_H
#define ITHREAD_DB_H
#include <thread>
#include <atomic>

class IThread_db
{
public:
	virtual ~IThread_db();

	void start();
	std::thread::id getId();
	void interrupt();
	bool isInterrupted();
	void join();

	virtual void run()=0;

private:
	std::atomic<bool> isInterrupt = false;
	std::thread th;
};

#endif // !ITHREAD_DB_H