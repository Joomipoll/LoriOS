#pragma once

#include <Lori/Sys/Handle.h>
#include <Lori/Types.h>

#include <list>
#include <vector>

using namespace std;

namespace Lori 
{
	class Waitable
	{
    	friend class Waiter;

	protected:
    	list<class Waiter*> waiters;

	public:
    	virtual inline const Handle& GetHandle() = 0;
    	virtual inline void GetAllHandles(vector<handle_t>& v) { v.push_back(GetHandle().get()); };
    	void Wait(long timeout = -1);

    	virtual ~Waitable();
	};

	class Waiter
	{
    	list<Waitable*> waitingOn;
    	list<Waitable*> waitingOnAll;
    	vector<handle_t> handles;

	public:
    	void RepopulateHandles();

        void WaitOn(Waitable* waitable);
		void WaitOnAll(Waitable* waitable);

    	void StopWaitingOn(Waitable* waitable);
    	void StopWaitingOnAll(Waitable* waitable);

        void Wait(long timeout = -1);

    	virtual ~Waiter();
	};
}
