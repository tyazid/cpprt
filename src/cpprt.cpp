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
#include "../inc/dbg.h"
#include "../inc/cpprt.h"

using namespace std;
using namespace util;

namespace util {
class JobQueue;
//static JobQueue* defaultJobQueue(bool );
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
//#define synchronized(M)  for( M.Acquire(); M; M.Release())

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
	   JobQueue* _jq;

 public:
	RunnableThreadPool(  JobQueue* jq) :
		_jq(jq) {
		//this->lock
	}
	virtual ~RunnableThreadPool() {

	}
	void Run(void*arg) {
		Job*job = NULL;

 		// std::cout<<std::endl<<"--------- RunnableThreadPool Q -------- ?::"<< (q->jobs.size())<<std::endl;
		while ((job = this->_jq->popJob())){
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

/*
 static unsigned ncpus(void) {
 	unsigned nprocs = 0;
 	FILE *cmd = popen("grep '^processor' /proc/cpuinfo | wc -l", "r");
 	if (cmd) {
 		size_t n;
 		char buff[8];
 		if ((n = fread(buff, 1, sizeof(buff) - 1, cmd)) > 0) {
 			buff[n] = '\0';
 			if (sscanf(buff, "%u", &nprocs) != 1)
 				nprocs = 0;
 		}
 		pclose(cmd);
 	}
 	return nprocs;
 }

 // nb core
  static unsigned coreNumber(void) {
  	static unsigned num_threads = 0;
 	if (!num_threads) {
 		if (!(num_threads = (unsigned) sysconf( _SC_NPROCESSORS_ONLN))
 				&& !(num_threads = ncpus()))
 			num_threads = MIN_NB_TH_POOL;
 	}
  	return num_threads;
  }
*/

/*
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
*/




#define USER_NEW_MTX
Mutex::Mutex() :
		   isCopy(false),
		   mutex(new std::mutex),
		/*global_lock(new std::unique_lock<std::mutex> (*mutex, std::defer_lock)),*/
		locked(false),
		cond_var(new std::condition_variable),lock_tmp(NULL)
{
	DLOG_INF();
	cout<<"### MUTEX CREATE IN ("<<this->mutex<<")"<<endl;

	DLOG_OUTF();
}

Mutex::Mutex(Mutex& other, bool lock) :
		  isCopy(true),
		  mutex(new std::mutex),
		/*global_lock(new std::unique_lock<std::mutex> (*mutex, std::defer_lock)),*/
		locked(false),
		cond_var(new std::condition_variable),lock_tmp(NULL)
{
	DLOG_INF();

	if (lock)
	{
		DLOG_LOGL(" WARNIN IN MTX COPY/bool == > WILL LOCK ");
		//Acquire();
	}DLOG_OUTF();
}
Mutex::Mutex(Mutex& other) :
		  isCopy(true),
		  mutex(new std::mutex),
		/*global_lock(new std::unique_lock<std::mutex> (*mutex, std::defer_lock)),*/
		locked(false),
		cond_var(new std::condition_variable),lock_tmp(NULL)

{
	DLOG_INF();DLOG_LOGL(" WARNIN IN MTX COPY == > WILL NOT LOCK " );
#ifndef USER_NEW_MTX
	assert(ctx->mutex);
#endif
 	DLOG_OUTF();

}
Mutex::~Mutex()
{
	DLOG_INF()
	;
	DLOG_LOGL(" isCopy:"<<isCopy);
	cout<<"### MUTEX DESTROY IN ("<<this->mutex<<")"<<endl;

	//delete global_lock;
	delete cond_var;
	delete mutex;
	DLOG_OUTF()
	;
}


bool Mutex::setLockTmp(  std::unique_lock<std::mutex>& lock_tmp) {
	this->lock_tmp = &lock_tmp;
	return true;
}

bool Mutex::Wait(long int time)
{
	DLOG_INF();
	/*if (!this->global_lock)
		return false;*/
	if(!this->lock_tmp)
		return false;

	std::unique_lock< std::mutex>& lock=*this->lock_tmp;
	this->lock_tmp=NULL;
	locked = false;
	const std::chrono::milliseconds timeout(time > 0 ? time : 0);

	if (time <= 0)
		this->cond_var->wait(lock);
	else
		this->cond_var->wait_for(lock, timeout);
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
	//if(!this->global_lock)
		//return false;
	this->cond_var->notify_one();

	DLOG_OUTF();
	return true;//TODO
}

bool Mutex::NotifyAll()
{
	DLOG_INF();
	//if(!this->global_lock)
		//return false;
	this->cond_var->notify_all();
	DLOG_OUTF();
	return true;//TODO
}
/*
void Mutex::Release()
{
	DLOG_INF();

	cout<<"## MUTEX RELEASE IN ("<<this->mutex<<")"<<endl;

	mutex->unlock();
	cout<<"## MUTEX RELEASE OUT ("<<this->mutex<<")"<<endl;

	locked=false;

	DLOG_OUTF();
}

void Mutex::Acquire()
{
	DLOG_INF();
	//pthread_mutex_lock
///	global_lock = new  std::unique_lock<std::mutex>(mutex);
	locked=true;
	cout<<"## MUTEX ACQUIRE IN ("<<this->mutex<<")"<<endl;
	mutex->lock();
	cout<<"## MUTEX ACQUIRE OUT ("<<this->mutex<<")"<<endl;

	//std::gar
    DLOG_OUTF();
}*/
std::mutex& Mutex::native_handle() const
  {
	 return *(this->mutex);
	}
Mutex::operator bool() const
{
	DLOG_INF();

	bool b=	locked;//this->global_lock && this->global_lock->owns_lock();
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
	bool alive;
} Thread_Ctx;
#define FREE_THREAD_CTX(C) do{if (C){\
							C->app_data = NULL;\
							C->job = NULL;\
							free(C);\
							}}while(0)


#define ALLOC_THREAD_CTX(C) do{\
							C = (Thread_Ctx*)malloc(sizeof(Thread_Ctx));\
							assert(C); \
							memset(C,0,sizeof(Thread_Ctx));\
							}while(0)

static void *doIt(void *arg)
{
	DLOG_INF();
	Thread_Ctx* parm = (Thread_Ctx*) arg;
	parm->alive = true;
	 std::cout<<std::endl<<"##(CPP RT) Starting TH:"<<  std::this_thread::get_id()<<"  ON CPU:"<< sched_getcpu() <<std::endl;
	if (parm && parm->job)
		try
		{
			// std::cout<<std::endl<<"--------- STARTING JOB  -------- ?::"<< (parm->job)<<std::endl;
			parm->job->Run(parm->app_data);
		} catch (...){}
	 FREE_THREAD_CTX(parm);
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
Thread::Thread(bool deamon) :_run(this),  appData(NULL),
		_deamon(deamon),_hyperthread_core_id(0),hyper(false),thread(NULL)
{
	DLOG_INF();
	DLOG_OUTF();
}
Thread::Thread(Runnable* task ,bool deamon,int hyperthreadCoreId) :
		_run(task ? task : this),
#ifdef __APPLE__
	 thread_helper(false),

#endif /* __APPLE__*/
	appData(NULL),
	_deamon(deamon),
	_hyperthread_core_id(hyperthreadCoreId),
	hyper(hyperthreadCoreId>=0?true:false),thread(NULL)
{
	DLOG_INF();
	DLOG_OUTF();
}



#endif /* _WIN32*/

Thread::Thread(const Thread&other) :
		_run(other._run),
		appData(other.appData),
		_deamon(other._deamon),
		_hyperthread_core_id(other._hyperthread_core_id),
		hyper(other.hyper),thread(NULL)
{
}
Thread::~Thread()
{
	DLOG_INF();
	DLOG_LOGL( " Thread DESTROY  ON TH =" << this);
	if (!this->_deamon) {
#ifdef __APPLE__
		 if(thread_helper)
#else
		if ( this->thread )
#endif /* __APPLE__*/
		 {
			try {
				this->thread->detach();
				delete this->thread;
				this->thread=NULL;
			///	pthread_detach(_pthread);
			} catch (...) {
				DLOG_LOGL( " pthread_detach  failed  on TH" <<this );
			}
		}
	}

	DLOG_OUTF();
}
void Thread::Run(void* arg)
{
	DLOG_LOGL( " ** START PARENT THREAD" );
}
void Thread::Start(void* arg)
{
	DLOG_INF();
	DLOG_LOGL( " Thread Start  ON TH =" << this );

	Thread_Ctx* parm;
	DLOG_LOGL( " Thread Start  ON TH =" << this );
	DLOG_LOGL( " Thread START  ON P =" << parm );
	ALLOC_THREAD_CTX(parm);
	parm->app_data = arg;
	parm->job = this->_run;


	 std::cout<<"      > IN Start Thread :: Started in Hyper Th mode "<<std::endl;


	this->thread = new std::thread(doIt, parm);


	if (!this->thread)
	{
		 FREE_THREAD_CTX(parm);
 	} else
	{
		 std::cout<<"     > IN Start Thread :: Created ok "<<std::endl;

		/*hyper threading stuff.*/
		static int num_threads = -1;
		if (num_threads == -1)
			num_threads = std::thread::hardware_concurrency();
		if (num_threads != -1 && this->_hyperthread_core_id >= 0
				&& this->_hyperthread_core_id < num_threads) {
			cpu_set_t cpuset;
			CPU_ZERO(&cpuset);
			CPU_SET(this->_hyperthread_core_id, &cpuset);
			int rc = pthread_setaffinity_np(this->thread->native_handle(),
					sizeof(cpu_set_t), &cpuset);
			if (rc != 0) {
				std::cerr << "!!!!Error calling pthread_setaffinity_np: " << rc
						<< "\n";
			}

			rc = pthread_getaffinity_np(this->thread->native_handle(),
					sizeof(cpu_set_t), &cpuset);
			if (rc != 0)
				std::cerr << "!!!!Error calling pthread_getaffinity_np: " << rc
						<< "\n";
 			if (!CPU_ISSET(this->_hyperthread_core_id, &cpuset))
				std::cerr << "!!!!Error : could not set cpu affinity cpu: "
						<< this->_hyperthread_core_id << "\n";
 			 std::cout<<"     		> IN Start Thread :: Started in Hyper Th mode "<<std::endl;


		}
	} 
	 std::cout<<"     		< OUT Start Thread   "<<std::endl;

	DLOG_OUTF();
}

void Thread::Join()
{
	DLOG_INF();
	 std::cout<<"     		> IN Start Thread::Join()   "<<std::endl;

#if defined _WIN32
	if (_pthread.x != ULONG_MAX)
	{
		pthread_join(_pthread, NULL);
		_pthread.x = ULONG_MAX;
	}
#else

#ifdef __APPLE__
	 if(thread_helper)
#else
	 if (this->thread )
#endif
	{
		// std::cout<< std::endl<<"################ J O I N "<<this->thread->native_handle()<< std::endl;
		 this->thread ->join();
		//pthread_join(_pthread, NULL);


	 delete this->thread;
	 this->thread=NULL;

	}
#endif
	 std::cout<<"     		< OUT Start Thread::Join()   "<<std::endl;

	DLOG_OUTF();

}
//IsAlive
bool Thread::IsAlive() {
	return (this->thread && this->thread->joinable() ) ?true : false;
 }



bool Thread::SetPriority(Thread::Priority prio, bool fifo){
int __sched_priority; //Thread::
int __policy;
switch (prio) {
	case MAX_PRIORITY:
		__policy = fifo ? SCHED_FIFO : SCHED_RR;
		__sched_priority = sched_get_priority_max(__policy);
 		break;
	case MIN_PRIORITY:
		__policy= SCHED_IDLE;
		__sched_priority = 0;

			break;
	case NORM_PRIORITY:
		__policy= SCHED_OTHER;
		 __sched_priority = 0;
			break;
	default:
	return false;
}

sched_param sch_params;
sch_params.sched_priority = __sched_priority;
cout<<"### THREAD SET PRIOR (PRIORITY="<<__sched_priority<<" , POLICY=" <<__policy<<  ")"<<endl;

if (this->thread && pthread_setschedparam(this->thread->native_handle(), __policy, &sch_params)) {
		std::cerr << "Failed to set Thread scheduling : "
				<< std::strerror(errno) << std::endl;
		return false;
	}

	return true;
}


#define ALIGN_TH_POOL(N) N<1?\
						 1:(N> std::thread::hardware_concurrency()?  std::thread::hardware_concurrency()  : N)

ThreadPool::ThreadPool(unsigned num_threads):tnumber( ALIGN_TH_POOL(num_threads)),
											 hyperThreaded(false),
											 working(0),priority(Thread::NORM_PRIORITY),priorityFifo(false),jobQ(NULL)
											 {

}
ThreadPool::ThreadPool(bool hyperthread):tnumber(1),
										hyperThreaded(hyperthread),
										working(0) ,priority(Thread::NORM_PRIORITY),priorityFifo(false),jobQ(new JobQueue){

 this->tnumber= (hyperthread?ALIGN_TH_POOL( std::thread::hardware_concurrency()):1);

}

ThreadPool::ThreadPool(unsigned num_threads, bool hyperthread):
										tnumber( ALIGN_TH_POOL(num_threads)),
										hyperThreaded(hyperthread),
										working(0) ,priority(Thread::NORM_PRIORITY),priorityFifo(false),jobQ(new JobQueue) {}

ThreadPool::ThreadPool(const ThreadPool& other):tnumber( ALIGN_TH_POOL(other.tnumber)),
												hyperThreaded(other.hyperThreaded),
												working(0),priority(Thread::NORM_PRIORITY),priorityFifo(false),jobQ(new JobQueue) {}




void ThreadPool::AddTask(Runnable* task, void* arg) {

	JobQueue* q = (JobQueue*) this->jobQ;
//	 JobQueue* q = defaultJobQueue(false);
	q->addJob(task, arg);
	std::cout << "IN AddTask PendingTasks==" << this->PendingTasks()
			<< std::endl;

}


unsigned ThreadPool::WorkingThreads() const {
	unsigned w=0;


	for ( size_t i = 0; i <  this->threads.size(); i++ )
	 	  if(this->threads.at(i) && this->threads.at(i)->IsAlive())
	 		  w++;
	return w;
}

unsigned ThreadPool::PendingTasks() const{
	 JobQueue* q =( JobQueue* )this->jobQ;
	return q->jobs.size();
}

  unsigned ThreadPool::GetPlatformHyperThreadNumber() const{
	  return this->tnumber;
  }


//PendingTasks
void ThreadPool::Join() {
	 std::cout<<"IN Join "<<std::endl;
	 for ( size_t i = 0; i <  this->threads.size(); i++ ){
		 std::cout<<"---> Join:"<<i<<std::endl;
		 std::cout<<"--->    Joint:"<<this->threads[i]<<std::endl;
		 std::cout<<"--->    Joina:"<<this->threads[i]->IsAlive()<<std::endl;
		 try
		 {
			 if( this->threads[i] && this->threads[i]->IsAlive())
			 	 this->threads[i]->Join();
		 }
		 catch (std::exception& e)
		 {
		     std::cerr << "!!!!!!!!!!! in cpp-rt Exception catched : " << e.what() << std::endl;
		 }
	  std::cout<<"---< Join:"<<i<<std::endl;
	 }
	 if(this->jobQ )
	 std::cout<<"OUT Join."<<std::endl;
 }


unsigned ThreadPool::ThreadCount() const{
	return this->threads.size();
}


void ThreadPool::SetThreadsPriority(Thread::Priority prio, bool fifo){
this->priority = prio;
this->priorityFifo=fifo;
}

/*!
 *\brief Causes this thread pool to begin executions.
 */
void ThreadPool::Start() {
	std::cout << ">ThreadPool::Start WorkingThreads==" << this->WorkingThreads()
			<< " PendingTasks==" << this->PendingTasks() << std::endl;
	if (this->WorkingThreads() || !this->PendingTasks())
		return;
	this->threads.clear();
	unsigned l = std::min(this->PendingTasks(), this->tnumber);
	std::cout << "ThreadPool::Start in progress ... L=" << l << std::endl;
	for (unsigned instance = 0; instance < l; instance++) {
		Thread *thread = new Thread(
				new RunnableThreadPool((JobQueue*) this->jobQ), false,
				instance);
		std::cout << "     > Starting  " << instance << "/" << l << std::endl;
		this->threads.push_back(thread);
		thread->Start(NULL);
		if (this->priority != Thread::NORM_PRIORITY)
			thread->SetPriority(this->priority, this->priorityFifo);
	}
	std::cout << "<<ThreadPool::Start  Done ..." << std::endl;
}

ThreadPool::~ThreadPool() {
	 std::cout<<">>~ThreadPool..."<<std::endl;
	 if(!this->threads.empty())
	 for ( size_t i = 0; i <  this->threads.size(); i++ )
	 	 	   delete this->threads[i];
	if(this->jobQ)
		delete ( JobQueue* )this->jobQ;

	 std::cout<<"<<~ThreadPool..."<<std::endl;

}

}

