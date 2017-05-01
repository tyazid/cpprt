/*
 *Licensed to the Apache Software Foundation (ASF) under one
 or more contributor license agreements.  See the NOTICE file
 distributed with this work for additional information
 regarding copyright ownership.  The ASF licenses this file
 to you under the Apache License, Version 2.0 (the
 "License"); you may not use this file except in compliance
 with the License.  You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing,
 software distributed under the License is distributed on an
 "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND, either express or implied.  See the License for the
 specific language governing permissions and limitations
 under the License.
 */


/*
 * utils.cpp
 *
 *  Created on: Oct 25, 2013
 *      Author: tyazid
 */


#include "rt_internal.h"


using namespace std;
using namespace util;

namespace util {
class JobQueue;
static JobQueue* defaultJobQueue(bool );
/** POOL JOB **/
 class Job{
 public:
	 Job(util::Runnable *  job,void*  jobArg);
	 virtual ~Job();
	util::Runnable *  runnable;
 	void*  arg;

} ;
 Job::Job(util::Runnable *job,void*  jobArg):runnable(job),arg(jobArg){}
 Job::~Job(){
	 if(runnable) delete runnable;
  }

 /** POOL QUEUE **/
 class JobQueue{
 public :
	 JobQueue(){ }
	 virtual ~JobQueue(){
		 Job*j=NULL;
		 while((j=this->popJob()))
			 delete j;
	 }
	void addJob(util::Runnable* rjob, void*arg) {
		synchronized(this->mutex)
		{
			this->jobs.push(new util::Job(rjob, arg));
		}
	}
	Job* popJob() {
		Job*j = NULL;
	 	synchronized(this->mutex)
		{
			if (this->jobs.empty())
				j= NULL;
			else {
			 j = this->jobs.front();
			 this->jobs.pop();
			}

		}
		return j;
	}
	std::queue<Job*> jobs;
	util::Mutex mutex;
 } ;

 class RunnableThreadPool: public Runnable
 {
 private:
 Mutex& notifyer;

 public:
	RunnableThreadPool(Mutex& owner) :
			notifyer(owner) {
	}
	virtual ~RunnableThreadPool() {

	}
	void Run(void*arg) {
		Job*job = NULL;
		JobQueue* q = defaultJobQueue(false);
		// std::cout<<std::endl<<"--------- RunnableThreadPool Q -------- ?::"<< (q->jobs.size())<<std::endl;

		while ((job = q->popJob())){
			// std::cout<<"===============> POPED JOB (in:"<< (this) << ") J:"<< job->runnable<<std::endl;

			job->runnable->Run(job->arg);
		}

 		//synchronized(this->notifyer)
		//{
	//		this->notifyer.NotifyAll();
//		}
		printf("***>>>ALL JOB DONE FOR %p\n",this);

	}

};

static unsigned coreNumber(void) {
	static int num_threads = -1;
	if (num_threads == -1)
		num_threads= (int)sysconf( _SC_NPROCESSORS_ONLN );
	if(num_threads == -1)
		num_threads=MIN_NB_TH_POOL;
	return num_threads;
}



static JobQueue* defaultJobQueue(bool release) {
	static JobQueue* Q = NULL;
	if (release) {
		delete Q;
		Q = NULL;
	} else {
		Q = Q == NULL ? new JobQueue : Q;
	}
	return Q;
}

static bool existTh(vector<pthread_t> & locked) // Mutex* This)
{
	pthread_t thread = pthread_self();
	for (unsigned i = 0; i < locked.size(); ++i)
		if (EQ_TH(locked[i], thread))
			return true;
	return false;
}
bool addTh(vector<pthread_t>& locked)
{
	DLOG_INF();
	pthread_t thread = pthread_self();
	DLOG_LOGL( "addTh thread = " << TH_LOG( thread ));

	for (unsigned i = 0; i < locked.size(); i++)
	{
		if (EQ_TH(locked[i], thread))
		{
			DLOG_LOGL( " RET = FALSE");DLOG_OUTF();
			return false;
		}
	}
	locked.push_back(thread);
	DLOG_OUTF();
	return true;

}
bool rmTh(vector<pthread_t> &locked)
{
	DLOG_INF();
	pthread_t thread = pthread_self();
	for (unsigned i = 0; i < locked.size(); i++)
		if (EQ_TH(locked[i], thread))
		{
			locked.erase(locked.begin() + i);
			DLOG_LOGL( " RET = TRUE");DLOG_OUTF();
			return true;
		}DLOG_OUTF();
	return false;
}

static bool setTime(long int time, struct timespec& ts)
{
	DLOG_INF();
	struct timeval tp;

	assert(gettimeofday(&tp, NULL) != -1); //cannot get time!;
	memset(&ts, 0, sizeof(struct timespec));
	ts.tv_sec = tp.tv_sec;
	long int usec = (time % (long int) 1E3l); //
	usec *= 1000;
	usec += tp.tv_usec;
	usec *= 1000;
	ts.tv_nsec += usec;	//nano sec
	ts.tv_sec += (time_t) (ts.tv_nsec / 1E9L);
	ts.tv_nsec %= (time_t) 1E9L;
	ts.tv_sec += time / 1000;
	DLOG_OUTF();
	return true;

}

Mutex::Mutex() :
		ctx(NULL), isCopy(false)/*, released(true)*/
{
	DLOG_INF();
	INIT(ctx, m_context);

	INIT(ctx->cond_waiter, pthread_cond_t);
	INIT(ctx->mutex, pthread_mutex_t);
	if (pthread_mutex_init(ctx->mutex, NULL))
	{
		ctx->mutex = NULL;
		DLOG_LOGL( " Raised runtime_error");DLOG_OUTF();
		throw std::runtime_error("cannot create mutex !");
	}DLOG_OUTF();
}

Mutex::Mutex(Mutex& other, bool lock) :
		ctx(other.ctx), isCopy(true)
{
	DLOG_INF();
	assert(ctx->mutex);
	if (lock)
	{
		DLOG_LOGL(" WARNIN IN MTX COPY/bool == > WILL LOCK ");
		Acquire();
	}DLOG_OUTF();
}
Mutex::Mutex(Mutex& other) :
		ctx(other.ctx), isCopy(true)/*, released(false)*/
{
	DLOG_INF();DLOG_LOGL(" WARNIN IN MTX COPY == > WILL NOT LOCK " );
	assert(ctx->mutex);DLOG_OUTF();

}
Mutex::~Mutex()
{
	DLOG_INF();
	DLOG_LOGL(" isCopy:"<<isCopy );
	if (!isCopy)
	{
		Release();
		pthread_mutex_destroy(ctx->mutex);
		pthread_cond_destroy(ctx->cond_waiter);
		DESTROY(ctx->mutex);
		DESTROY(ctx->cond_waiter);
		DESTROY(ctx);
	}
	DLOG_OUTF();
}
bool Mutex::Wait(long int time)
{
	DLOG_INF();
	int rc = -1;

	assert(*this);/*will interpret the overridden bool op.*/
	ctx->nbWaitingTh++;
	if (time > 0)
	{
		struct timespec ts;
		setTime(time, ts);
		rc = pthread_cond_timedwait(ctx->cond_waiter, ctx->mutex, &ts);
	} else
	{
		rc = pthread_cond_wait(ctx->cond_waiter, ctx->mutex);
	}
	assert(rc==0 || rc == ETIMEDOUT);	//	("cannot pthread wait !"); */

	ctx->nbWaitingTh--;
	DLOG_OUTF();
	return true;
}

bool Mutex::Wait()
{
	DLOG_INF();
	bool b= Wait(0);
	DLOG_OUTF();
	return b;
}
bool Mutex::Notify()
{
	DLOG_INF();
	if (!ctx->locked.empty())
	{
		if (ctx->nbWaitingTh && !pthread_cond_signal(ctx->cond_waiter))
		{
			DLOG_LOGL("OK" );
			DLOG_OUTF();
			return true;
		}

	} else
	{
		DLOG_LOGL("/!\\not locked mutex! in Notify!!!" );
		///throw std::runtime_error("not locked mutex! in Notify");
	}
	DLOG_OUTF();
	return false;
}

bool Mutex::NotifyAll()
{
	DLOG_INF();
	bool b = (!ctx->locked.empty() && ctx->nbWaitingTh && !pthread_cond_broadcast(ctx->cond_waiter));
	DLOG_OUTF();
	return b;
}

void Mutex::Release()
{
	DLOG_INF();
	if (ctx->mutex == NULL || !ctx->locked.size() || !rmTh(ctx->locked))
		return;
	DLOG_LOGL( "<MTX release mtx    " << ctx->mutex << ", locked.size=" << ctx->locked.size());
	pthread_mutex_unlock(ctx->mutex);
	DLOG_OUTF();
}

void Mutex::Acquire()
{
	DLOG_INF();
	assert(ctx->mutex);	//no mutex !
	if (!addTh(ctx->locked))
		return;
	/*released = false;*/
	DLOG_LOGL(">MTX acquire mtx    " << ctx->mutex << ", locked.size=" << ctx->locked.size());
	pthread_mutex_lock(ctx->mutex);
	DLOG_OUTF();
}

Mutex::operator bool() const
{
	DLOG_INF();
	bool b=ctx->locked.size() > 0 && existTh(this->ctx->locked);
	DLOG_OUTF();
	return b;
}
/***************************************************************/
/**************************Threading****************************/
/***************************************************************/
typedef struct thread_arg
{
	void* app_data;
	Runnable* job;
} Thread_Ctx;
static void *doIt(void *arg)
{
	DLOG_INF();
	Thread_Ctx* parm = (Thread_Ctx*) arg;
	// std::cout<<std::endl<<"--------- IN DOIT -------- ?::"<< ((parm && parm->job)?1:0)<<std::endl;

	if (parm && parm->job)
		try
		{
			// std::cout<<std::endl<<"--------- STARTING JOB  -------- ?::"<< (parm->job)<<std::endl;

			parm->job->Run(parm->app_data);
		} catch (...)
		{
		}

	if (parm && parm->job)
	{
		parm->app_data = NULL;
		parm->job = NULL;
		delete parm;
	}
	// std::cout<<std::endl<<"--------- OUT DOIT -------- ?::"<< ((parm && parm->job)?1:0)<<std::endl;

	DLOG_OUTF();
	return NULL;  // ((Thread *)context)->doIt(((Thread *)context)->appData);
}
#ifdef _WIN32
Thread::Thread() :

_run(this), appData(NULL)
{
	_pthread.x = ULONG_MAX;
}
Thread::Thread(Runnable* task) :
_run(task ? task : this), appData(NULL)
{
	_pthread.x = ULONG_MAX;
}
#else
Thread::Thread(bool deamon) :_run(this), _pthread(ULONG_MAX), appData(NULL),_deamon(deamon),_hyperthread_core_id(0),hyper(false)
{
	DLOG_INF();
	DLOG_OUTF();
}
Thread::Thread(Runnable* task ,bool deamon,int hyperthreadCoreId) :
		_run(task ? task : this),
#ifdef __APPLE__
	 thread_helper(false),

#endif /* __APPLE__*/
	_pthread(ULONG_MAX),
	appData(NULL),
	_deamon(deamon),
	_hyperthread_core_id(hyperthreadCoreId),
	hyper(hyperthreadCoreId>=0?true:false)
{
	DLOG_INF();
	DLOG_OUTF();
}



#endif /* _WIN32*/

Thread::Thread(const Thread&other) :
		_run(other._run), 
		_pthread(other._pthread),
		appData(other.appData),
		_deamon(other._deamon),
		_hyperthread_core_id(other._hyperthread_core_id),
		hyper(other.hyper)
{
}
Thread::~Thread()
{
	DLOG_INF(); DLOG_LOGL( " Thread DESTROY  ON TH =" << this);
	if (!this->_deamon) {
		if (_pthread != ULONG_MAX) {
			try {
				pthread_detach(_pthread);
			} catch (...) {
				DLOG_LOGL( " pthread_detach  failed  on TH" <<this );
			}
		}
	}
	Join();
	DLOG_OUTF();
}
void Thread::Run(void* arg)
{
	DLOG_LOGL( " ** START PARENT THREAD" );
}
void Thread::Start(void* arg)
{
	DLOG_INF();
	Thread_Ctx* parm = new Thread_Ctx;
	DLOG_LOGL( " Thread Start  ON TH =" << this );
	DLOG_LOGL( " Thread START  ON P =" << parm );
	//pthread_create(struct thread_arg*) malloc(
	//	sizeof(struct thread_arg));
	assert(parm);		//no mutex !
	parm->app_data = arg;
	parm->job = this->_run;
	int th_start = pthread_create(&_pthread, NULL, &doIt, parm);
	DLOG_LOGL( "Thread START th_start =" << th_start);
	if (th_start != 0)
	{
		parm->app_data = NULL;
		parm->job = NULL;
		delete parm;
	} else
	{
		/*hyper threading stuff.*/
		  int num_threads =coreNumber();

		if (num_threads != -1 && this->_hyperthread_core_id >= 0
				&& this->_hyperthread_core_id < num_threads) {
			cpu_set_t cpuset;
			CPU_ZERO(&cpuset);
			CPU_SET(this->_hyperthread_core_id, &cpuset);
			int rc = pthread_setaffinity_np(this->_pthread,
					sizeof(cpu_set_t), &cpuset);
			if (rc != 0) {
				std::cerr << "!!!!Error calling pthread_setaffinity_np: " << rc
						<< "\n";
			}

			rc = pthread_getaffinity_np(this->_pthread,
					sizeof(cpu_set_t), &cpuset);
			if (rc != 0)
				std::cerr << "!!!!Error calling pthread_getaffinity_np: " << rc
						<< "\n";
 			if (!CPU_ISSET(this->_hyperthread_core_id, &cpuset))
				std::cerr << "!!!!Error : could not set cpu affinity cpu: "
						<< this->_hyperthread_core_id << "\n";


		}
	} 
	DLOG_OUTF();
}

void Thread::Join()
{
	DLOG_INF();
#if defined _WIN32
	if (_pthread.x != ULONG_MAX)
	{
		pthread_join(_pthread, NULL);
		_pthread.x = ULONG_MAX;
	}
#else
	if (_pthread != ULONG_MAX)
	{
		pthread_join(_pthread, NULL);
		_pthread = ULONG_MAX;
	}
#endif
	DLOG_OUTF();

}
//IsAlive
bool Thread::IsAlive() {
	return (_pthread != ULONG_MAX) && (pthread_kill(_pthread, 0) == ESRCH) ?
			true : false;
}

#define ALIGN_TH_POOL(N) N<MIN_NB_TH_POOL?\
						 MIN_NB_TH_POOL:(N>coreNumber()? coreNumber()  : N)

ThreadPool::ThreadPool(unsigned num_threads):tnumber( ALIGN_TH_POOL(num_threads)),
											 hyperThreaded(false),working(0),threads(new std::vector<Thread*>())
											 {

}
ThreadPool::ThreadPool(bool hyperthread):tnumber(MIN_NB_TH_POOL),
										hyperThreaded(hyperthread),
										working(0),
										threads(new std::vector<Thread*>()){

	this->tnumber= (hyperthread?ALIGN_TH_POOL(coreNumber()):MIN_NB_TH_POOL);

}

ThreadPool::ThreadPool(unsigned num_threads, bool hyperthread):
										tnumber( ALIGN_TH_POOL(num_threads)),
										hyperThreaded(hyperthread),
										working(0),
										threads(new std::vector<Thread*>())
										 {}

ThreadPool::ThreadPool(const ThreadPool& other):tnumber( ALIGN_TH_POOL(other.tnumber)),
												hyperThreaded(other.hyperThreaded),
												threads(new std::vector<Thread*>()),
												working(0) {}




void ThreadPool::AddTask(Runnable* task, void* arg) {
	 JobQueue* q = defaultJobQueue(false);
	 q->addJob(task,arg);
	 std::cout<<"IN AddTask PendingTasks=="<< this->PendingTasks()<<std::endl;

}


unsigned ThreadPool::WorkingThreads() {
	unsigned w=0;


	for ( size_t i = 0; i <  this->threads->size(); i++ )
	 	  if(this->threads->at(i) && this->threads->at(i)->IsAlive())
	 		  w++;
	return w;
}

unsigned ThreadPool::PendingTasks() {
	 JobQueue* q = defaultJobQueue(false);
	return q->jobs.size();
}

//PendingTasks
void ThreadPool::Join() {
	 std::cout<<"IN Join "<<std::endl;

	 for ( size_t i = 0; i <  this->threads->size(); i++ ){

	 	 	  if(this->threads->at(i) )
	 	 		this->threads->at(i)->Join();
	 }

 /*  synchronized(this->lock){
		while(this->WorkingThreads() || this->PendingTasks()){
			 std::cout<<"IN Join WorkingThreads=="<< this->WorkingThreads() <<" PendingTasks=="<< this->PendingTasks()<<std::endl;
			this->lock.Wait();
		}
	}*/
	 std::cout<<"OUT Join."<<std::endl;

 }

/*!
 *\brief Causes this thread pool to begin executions.
 */
void ThreadPool::Start() {
	 std::cout<<"IN Start WorkingThreads=="<< this->WorkingThreads() <<" PendingTasks=="<< this->PendingTasks()<<std::endl;
	if (this->WorkingThreads() || !this->PendingTasks())
		return;

	 std::cout<<"IN Start in progress ..."<<std::endl;

	unsigned l = std::min(this->PendingTasks(),this->tnumber);
	for(unsigned instance = 0;instance < l;instance++) {
		Thread *thread =NULL;
 		 std::cout<<"     > Starting  "<<instance<< "/"<<l<<std::endl;
  		thread=new Thread(new RunnableThreadPool(this->lock ) ,false,instance);
  		thread->Start(NULL);
  		this->threads->push_back(thread);
	}
	 std::cout<<"IN Start in Done ..."<<std::endl;
}
ThreadPool::~ThreadPool() {
	for ( size_t i = 0; i <  this->tnumber; i++ )
 	  delete this->threads->at(i) ;
	delete this->threads ;
}
}



