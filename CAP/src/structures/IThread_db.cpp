#include "IThread_db.h"

IThread_db::~IThread_db() {
	if (this->isInterrupted()) {
		this->interrupt();
	}
	if (this->th.joinable()) {
		this->th.join();
	}
}

void IThread_db::start() {
	std::thread thr(std::bind(&IThread_db::run, this));
	this->th = std::move(thr);
}

std::thread::id IThread_db::getId() {
	return this->th.get_id();
}
void IThread_db::interrupt() {
	this->isInterrupt = true;
}
bool IThread_db::isInterrupted() {
	return this->isInterrupt;
}
void IThread_db::join() {
	this->th.join();
}
//void IThread_db::run() {}
