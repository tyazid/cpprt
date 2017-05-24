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
/*!
 * \file utils.h
 * \brief Runtime Tools API.
 * \author Yazid.T
 * \version 0.1
 * \brief  This is the header file that expose C++ Runtime Tools.
 */
/*
 * utils.h
 *
 *  Created on: Oct 10, 2013
 *      Author: tyazid
 */

#ifndef UTILS_H_
#define UTILS_H_
#include <set>
#include <map>
#include <stddef.h>
#include <iterator>
#include <vector>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <queue>
#include <thread>
#include <mutex>

#include <pthread.h>
#include <sstream>
#include <algorithm>
#include <time.h>
#include <sys/time.h>
#include <climits>
#include <errno.h>
#include <typeinfo>
#include <sys/types.h>
#include "dbg.h"


#include <condition_variable>

#define MIN_NB_TH_POOL 2
/*!
 *  \brief utils namespace offering a simple way to use synchronizing mechanism and event - queuing.
 */
namespace util {

/*!
 * \def  INSTANCEOF(OBJ, T)
 * \brief  Bool Macro that check if OBJ is an instance of T type.
 */
#define INSTANCEOF(OBJ, T) (dynamic_cast<T*>(&OBJ) != NULL)

/*!
 *  \def synchronized(M)
 *  \brief  Macro that act as a mutual exclusion lock block which is always synchronized on a particular Mutex.
 * @warning : When a block of code is declared synchronized, then at any one time, only one thread can be "inside" any block synchronized on that Mutex.
 *! For example, in the following code:
 *  \code{.cpp}
 *  Mutext m;
 *synchronized (m) {
 * // ... do some stuff ...
 * }
 *\endcode
 */


//std::lock_guard<std::mutex> lock(M.ctx_new.mutex)
//#define synchronized(M)  for(std::lock_guard<std::mutex> lock(M.ctx_new.mutex); lock; lck.Release())
//#define synchronized(M)  for( (&M)->Acquire(); M; (&M)->Release())
//#define synchronized(M) for( std::unique_lock<std::mutex> lock(M.native_handle()) ; lock.owns_lock(); lock.unlock())
#define synchronized(M) for( std::unique_lock<std::mutex> lock{(&M)->native_handle()} ;(&M)->setLockTmp(lock) && lock.owns_lock(); lock.unlock())


#define DESTROY(V) do{if(V) delete V; V =NULL;}while(0)
template<class > class Observable;
template<class > class EventObject;
template<class > class EventListener;
template<class > class EventQueue;
template<typename > class AttributeSet;
template<typename > class Attribute;
template<class, class > class Factory;
class Runnable;
class Thread;

/*!
 *  \brief A simple non-reentrant mutual exclusion mutex .
 * \n A typical use of the Mutex is to create a synchronized block using <b>synchronized(M)</b> macro which acts as a mutual exclusion lock block which is is always synchronized on a particular Mutex.
 * @see synchronized
 */
class Mutex {
public:
	virtual ~Mutex();
	/*!
	 * \brief Default constructor.
	 * Construct and init a new Mutex.

	 */
	Mutex();

	/*!
	 *\brief Causes the caller thread to wait until another thread invokes the Notify().
	 * @return true if waited successfully.
	 */
	bool Wait();
	/*!
	 * \brief Causes current thread to wait until  another thread invokes the Notify function.
	 * @param time the maximum time to wait in milliseconds. MUTEX is assumed to be locked before..
	 * \return true if waited successfully.
	 **/
 	//bool Wait2 = [&]() { return true; };

	bool Wait(long int time);
	/*!
	 * \brief Wakes up the thread that is waiting on this Mutex.
	 * \return true if notified (it was in waiting state & wait able to signal a waiter thread) successfully.
	 **/
	bool Notify();
	/*!
	 * \brief Wakes up the threads that are waiting on this Mutex.
	 * \return  true if notified (at least one) successfully.
	 **/
	bool NotifyAll();

	/*!
	 * \brief override of bool operator.
	 * \return  true if this Mutex is locked.
	 **/
	operator bool() const;
	/*!
	 * \brief Potentially enable other threads to pass.
	 * Releasing an acquire that is already free has no effect..
	 */
//	void Release();
	/*!
	 * \brief Wait until successful passage.
	 */
//	void Acquire();

	  std::mutex& native_handle() const;
 	  bool setLockTmp( std::unique_lock<std::mutex>& lock_tmp);
//	 for( std::unique_lock<std::mutex>* lock = new  std::unique_lock<std::mutex>(this->mutex->native_handle()) ; lock->owns_lock(); lock->unlock())


private:
	/*!
		 * \brief Copy constructor.
		 * This constructor will lock on the same mutex.
		 * @param other to copy from.
		 */
		Mutex(Mutex& other);

		/*!
		 *\brief Copy constructor.
		 * This constructor will lock on the same mutex.
		 * @param other to copy from.
		 * @param lock: this means that the mutex will lock at the cpy time.
		 */
		Mutex(Mutex& other, bool lock);
	bool isCopy;
    std::mutex *mutex;

    //std::unique_lock <std::mutex>* global_lock ;
 	bool locked ;

 	std::condition_variable *cond_var;
	   std::unique_lock<std::mutex>* lock_tmp;

  // std::lock_guard<std::unique_lock<std::mutex>> *  lock_guard;


//	bool released;

};

/***************************************************************/
/**************************Threading****************************/
/***************************************************************/
/*!
 *  \brief This interface is designed to provide a common action implementation class of Thread.
 * The Runnable class could be implemented by any
 * class whose instances are intended to be executed by a Thread.
 */
class Runnable {
public:
	/*!
	 *  The function that will be invoked be the executing thread.
	 * @param arg the argument passed to Thread#Start(void*) function.
	 */
	virtual void Run(void*arg) =0;
	virtual ~Runnable() {
	}
};
/*!
 *  \brief Thread is a thread of asynchronous execution in an application. application is allowed to have multiple threads of
 * execution.
 */
class Thread: virtual public Runnable {
private:
	Runnable* _run;
	//pthread_t _pthread;
	void* appData;
	bool _deamon;
	int _hyperthread_core_id;
	bool hyper;
	std::thread *thread;

	#ifdef __APPLE__
	bool thread_helper;
	#endif


public:
	enum Priority{MIN_PRIORITY,NORM_PRIORITY, MAX_PRIORITY};
	/**
	 * \brief Default constructor.
	 * construct and init a new Thread.
	 * @param deamon if true mean that the thread will keep alive until it finish its run method even id should wait during its
	 *destruction.
	 */
	Thread( bool deamon=false);
	/**
	 * \brief Constructor with Runnable.
	 * Construct and init a new Thread with a Runnable parameter that will executed asynchronously when Start is called.
	 *@param task the Runnable task to execute.
	 *@param deamon if true mean that the thread will keep alive until it finish its run method even id should wait during its
	 *destruction.
	 *@hyperthreadCoreId Limit specified thread  to run only on the processor represented by the coreID.
	 */
	Thread(Runnable* task, bool deamon=false, int hyperthreadCoreId=-1);
	/**
	 * \brief Copy constructor.
	 * @param other Thread to copy from.
	 */
	Thread(const Thread&other);
	virtual ~Thread();
	/*!
	 * \brief Thread Run method.
	 * If this thread was constructed using a separate Runnable run object,
	 * then that Runnable's Runnable#Run() method is called;otherwise,
	 * this method does nothing and returns.*/
	virtual void Run(void* arg);

	/*!
	 * \brief Causes this thread to begin execution; The Run method oh this thread or of the defined (during constructor call) Runnable
	 * will be called in separated execution thread..
	 * @param arg any kind of data pointer passed as the sole argument of Runnable#Run()
	 */
	virtual void Start(void* arg);
	/*!
	 *\brief  Waits for this thread to finish.
	 */
	virtual void Join();

	virtual bool IsAlive();

 	bool SetPriority(Priority , bool);
};

/***************************************************************/
/**************************ThreadPool***************************/
/***************************************************************/
/*!
 * \brief A simple thread Pool.
 */
class ThreadPool {
private :
	unsigned tnumber;
	bool hyperThreaded;
	unsigned working;
	Thread::Priority priority;
	bool priorityFifo;
	Mutex lock;
	std::vector<Thread*> threads;
public:
	//h == treu --> nbth == nb core.
	ThreadPool(bool hyperthread =true);

	ThreadPool(unsigned num_threads  );
	ThreadPool(unsigned num_threads , bool hyperthread =true);
	ThreadPool(const ThreadPool&other);

	/*!
	 *\brief  Add work to the job queue.
	 */
	virtual void AddTask(Runnable* task, void *arg);

	/*!
	 *\brief Gets currently working threads number.
	 */
	virtual unsigned WorkingThreads() const;

	virtual unsigned ThreadCount() const;
	/*!
	 *\brief Gets currently pending task (not yet started) number.
	 */
	virtual unsigned PendingTasks() const;
	/*!
		 *\brief Gets currently max supported H-threads number.
		 */
	virtual unsigned GetPlatformHyperThreadNumber()const;
	/*!
	 *\brief  Waits for all threads to finish( task job became empty & all thread are in idle state)).
	 */
	virtual void Join();
	/*!
	 *\brief Causes this thread pool to begin executions.
	 */
	virtual void Start();

	void SetThreadsPriority(Thread::Priority , bool);
	virtual ~ThreadPool();

	void* jobQ; /*opaque*/

};


/***************************************************************/
/**************************EventQueue***************************/
/***************************************************************/
/*!
 * \brief The root class from which all event state objects shall be derived.
 */
template<class SRC> class EventObject {

public:
	/*!
	 * \brief Constructs an Event.
	 *
	 * @param src The object on which the Event initially occurred.
	 */
	EventObject(const SRC* src) :
			source(src) {
	}
	/*!
	 * \brief clone an Event.
	 *
	 * @param other The other to clone.
	 */
	EventObject(const EventObject& other) :
			source(other.source) {
	}
	/*!
	 * \brief  The object on which the Event initially occurred.
	 * @return   The object on which the Event initially occurred.
	 */
	inline const SRC* GetSource() const {
		return source;
	}

private:

	const SRC* source;
};
/*!
 * \brief The EventListener class allows to  handle  asynchronous events generated from source component. Generally, class who should be monitored a given kind of event
 * shall extends observable class (namespace  util)
 * @see Observable.
 *
 */
template<class T> class EventListener {
public:
	virtual ~EventListener() {
	}
	/*!
	 \brief Notifies the listener of an event generated by its source.
	 * */
	virtual void onEvent(T *eventData)=0;
};

/******************** EvtQ ***************************/
/******************** **************************************/

/*!
 * \brief Class to be implemented by EventQueue users.
 * The dispatch method is called each time an event has to be sent to a
 * registered listener
 */

template<class T> class EventQueue {
private:
	template<class J>
	struct EqJob {
		std::set<EventListener<J>*> *listners;
		J* event;
	};

	template<class E>
	class EqThread: public Thread {
	public:
		EqThread() :
				Thread(), mq(new Mutex), _q(new std::queue<struct EqJob<E>*>), alive(
						false) {
		}

		~EqThread() {
			DESTROY(mq);
			DESTROY(_q);

		}
		void Start(void* arg) {
			alive = true;
			Thread::Start(arg);
		}
		void push(struct EqJob<E>* j) {
			synchronized(*mq)
			{
				_q->push(j);
				DLOG_LOG(" Q.Job == " << (_q->size()));DLOG_LOGL("");DLOG_LOGL(" RUNNER PUSH -- ALIVE= " << alive << " WEAK up mtx = " << mq );
				mq->Notify();
			}
		}
		void kill() {
			if (alive) {
				alive = false;
				synchronized(*mq)
				{
					mq->Notify();
				}
				Join();
			}
		}
		void Run(void *d) {
			DLOG_LOGL( " **EqThread IN RUN alive = " <<alive<< ", mq ="<< mq );
			struct EqJob<E>* j = NULL;
			try {
				while (alive) {
					synchronized(*mq)
					{
						if (_q->empty()) {
							DLOG_LOGL(" **EQ no evt waiting ..." << mq );
							mq->Wait();
							DLOG_LOGL(" **EQ   waiting done!..." << mq << "alive:" << alive);
						} else{
							DLOG_LOGL(" EQ   evt consume ...");
						}
						//			while (alive && (j = pull()) != NULL)
						//						dispatch(j->listners, j->event);
						while (alive && !_q->empty()
								&& (j = _q->front()) != NULL) {
							_q->pop();
							dispatch(j->listners, j->event);
						}

					}
				}
			} catch (...) {
				alive = false;
			}
			DLOG_LOGL(" **EQ run leaving ..alive = " <<alive);
		}
	private:
		Mutex* mq;
		std::queue<struct EqJob<E>*>* _q;
		bool alive;
		void dispatch(std::set<EventListener<E>*>* ls, E* evt) {
			std::vector<EventListener<T>*> cpy(ls->begin(), ls->end());
			int s = cpy.size();
			for (int l = 0; l < s; l++)
				try {
					((EventListener<E>*) cpy[l])->onEvent(evt);
				} catch (...) {
					DLOG_LOGL( "Err when dispatching event : " << typeid(evt).name() << " to listener : " << typeid( cpy[l]).name());
				}
			delete evt;

		}
	};
public:
	/*!
	 *  \brief Default constructor
	 *
	 *  Constructor of EventQueue class. A default dispatcher will be created based on the generic type from EventQueue template.
	 */
	EventQueue(void) :
			_listeners(new std::set<EventListener<T>*>), mx(new Mutex), runner(
					new EqThread<T>()) {
		runner->Start(this);
	}
	/*!
	 *  \brief Destructor.
	 *
	 * Destroy an even queue and release its resources.
	 */
	virtual ~EventQueue() {
		DLOG_LOGL("~EventQueue()  dest mx" );
		DESTROY(mx);DLOG_LOGL("~EventQueue()  kill runner mx @"<< (void*)runner );
		runner->kill();
		DLOG_LOGL("~EventQueue()  dest runner");
		DESTROY(runner);
	}
	/*!
	 * \brief Enqueue an event which will be sent to registered listeners
	 * @param event typed T event to post.
	 */
	void Post(T* event) const {
		struct EqJob<T>* j = new struct EqJob<T>;
		synchronized(*mx)
		{
			j->event = event;
			j->listners = _listeners;
			runner->push(j);
		}
	}
	/*!
	 * \brief Adds the specified %EventListener to receive event from this event-queue object.
	 * @param l EventListener
	 */
	void AddQListener(EventListener<T>* l) const {
		synchronized(*mx)
		{
			DLOG_LOGL("EventQueue#AddQListener" );
			_listeners->insert(l);
		}
	}
	/*!
	 * \brief Removes the specified %EventListener so that it no longer receives event.
	 * @param l EventListener
	 */
	void RemoveQListener(EventListener<T>*l) const {
		synchronized(*mx)
		{
			DLOG_LOGL("EventQueue#RemoveQListener" );
			_listeners->erase(l);
		}
	}

private:

	std::set<EventListener<T>*> *_listeners;
	Mutex* mx;
	EqThread<T>* runner;
};

/*!
 @brief  A class which offers a default implementation of Observable class. So could be inherited by class who will integrate observable feature.
 */
template<class EVT> class Observable {
public:
	/*!
	 *  \brief Constructor
	 *  Constructor of Observable class.
	 */
	Observable() :
			_evtQ(new EventQueue<EVT>()) {
		DLOG_INF();DLOG_OUTF();

	}
	virtual ~Observable() {
		DLOG_INF();
		delete _evtQ;
		DLOG_OUTF();

	}
	/*!
	 * \brief  Adds the specified %EventListener to receive event from this observable object.
	 * @param l EventListener
	 */
	void AddListener(EventListener<EVT>*l) {
		DLOG_INF();
		_evtQ->AddQListener(l);
		DLOG_OUTF();
	}
	/*!
	 *  \brief Removes the specified %EventListener so that it no longer receives event.
	 * @param l EventListener
	 */
	void RemoveListener(EventListener<EVT>*l) {
		DLOG_INF();
		_evtQ->RemoveQListener(l);
		DLOG_OUTF();
	}
protected:
	/*!
	 * \brief Enqueue an event which will be sent to registered listeners.
	 * @param event the event to post.
	 */
	void postEvent(EVT*event) {
		DLOG_INF();
		_evtQ->Post(event);
		DLOG_OUTF();
	}
private:
	EventQueue<EVT> *_evtQ;

};

/******************** FACTORY HELPER ************************/
/*!
 @brief  A class which offers a default and templated Factory pattern.
 Its genericity is based on tow generic types defining the product type (P) and the product key type(K) which can also be considered as raw data used to create a product instance(s)
 @brief  Factory can also act as product (produced) data cache, so produced instance is stored and associated to its key in internal cache, this feature is enabled if the constructor with a boolean parameter cache_product (==true) Factory#Factory(bool) parameter is called.

 */
template<class P, class K>
class Factory {
private:
	class XFact;

public:
	/*!
	 * \brief This default constructor will disable the cache feature (see Factory#Factory(bool)).
	 */
	Factory(void) :
			caching(false) {
	}
	/*!
	 * \brief Construct a Factory with argument product cache enabler.
	 * @param chache_product will enable (true) the internal produced (typed P) products cache (associated with their keys type K).		 *
	 */
	Factory(bool chache_product) :
			caching(chache_product) {
		DLOG_INF();DLOG_OUTF();
	}
	//caching
	virtual ~Factory() {
		DLOG_INF();DLOG_OUTF();
	}

	/*!
	 *\brief Gets the unique instance of the Factory object.
	 * @return Factory (type templated<F, P, K>) instance ptr.
	 */
	static Factory<P, K>* GetInstance(void) {
		DLOG_INF();
		static Mutex m;
		synchronized(m)
		{
			if (_x_instances_ == NULL)
				_x_instances_ = new std::set<Factory<P, K>*>;
			if (NULL == _singleton_) {

				_singleton_ = new XFact(_x_instances_);
			}
		}

		DLOG_OUTF();
		return _singleton_;
	}
	/*!
	 * \brief Kill (delete) the unique instance of (templated <F, P, K>) Factory an release its resources. So it will be re-created next call of Factory#GetInstance().
	 */
	static void kill() {

		if (NULL != _x_instances_) {
			_x_instances_->clear();
			if (_x_instances_) {
				delete _x_instances_;
				_x_instances_ = NULL;
			}

		}
		if (_singleton_) {
			delete _singleton_;
			_singleton_ = NULL;
		}

	}
	/*!
	 * \brief  Register a new Factory typed F, so it will serve as potential creator for a given key (K) through Factory#Creat() call.
	 * @param factory the Factory pointer ref to register.
	 * @return true if the factory has been registered, false otherwise..
	 */
	static bool Register(Factory<P, K>* factory) {
		DLOG_INF();
		if (_x_instances_ == NULL)
			_x_instances_ = new std::set<Factory<P, K>*>;

		bool contains = _x_instances_->count(factory) != 0;
		_x_instances_->insert(factory);
		DLOG_LOG(" new size = " << _x_instances_->size());DLOG_LOGL(" )"); DLOG_OUTF();
		return !contains;
	}
	/*!
	 * \brief Creates a P typed object  from the specified key argument type K.
	 * @param  key The K typed argument the object's key to create.
	 * @return a prt ref on the created object typed P*.
	 */
	virtual P* Create(const K& key) throw (std::invalid_argument)=0;
	/*!
	 * \brief Gets a P typed object from the specified key argument type K. This method will return the cache content if the Factory was created with cache feature activated.
	 * Otherwise it calls Factory#Create for each get.
	 * @param key The K typed argument the object's key to get.
	 * @return a pointer ref on the created object typed P*.
	 */
	virtual P* Get(const K& key) throw (std::invalid_argument) {
		DLOG_INF();
		P* p;
		if (caching) {
			p = _fromCache(key);
			DLOG_LOGL( " P from cache prt = " << (void*) p );
			if ( NULL == p) {
				p = Create(key);
				_toCache(key, p);
			}
		} else {
			p = Create(key);
		} DLOG_OUTF();
		return p;
	}

protected:
	/*!
	 *\brief Should  implement the way to store a product typed P associated to key type K in the cache (for cache enabled feature).
	 * @param key The key arg of the object to be stored.
	 * @param product a pointer ref on the product object typed P* to be stored.
	 */
	virtual void _toCache(const K& key, P* product) = 0;
	/*!
	 *\brief Should  implement the way to read a product typed P associated to key type K from the cache (for cache enabled feature).
	 * @param key The key arg of the object to be read.
	 * @return product a prt ref on the stored product object typed P* (NULL if not present in the cache).
	 */
	virtual P* _fromCache(const K& key) = 0;
private:
	bool caching;
	static Factory<P, K> *_singleton_;
	static std::set<Factory<P, K>*> *_x_instances_;

};

/**make the code portable due to compiler core issue [org msg=warning: invalid use of incomplete type ‘class util::Factory< <template-parameter-1-1>, <template-parameter-1-2> >’ [enabled by default]
 ]**/
template<class P, class K>
class Factory<P, K>::XFact: public Factory<P, K> {

public:
	std::set<Factory<P, K>*>*_instances;

	XFact(std::set<Factory<P, K>*> * instances) :
			_instances(instances) {
	}
	virtual ~XFact() {
		_instances = 0;
	}

	//new OIRoombaTTLFactory()

	P* Get(const K& key) throw (std::invalid_argument) {
		std::vector<Factory<P, K>*> cpy(_instances->begin(), _instances->end());
		for (unsigned int l = 0; l < cpy.size(); l++)
			try {
				Factory<P, K>* f = cpy[l];
				return f->Get(key);
			} catch (std::invalid_argument& e) {
			}
		throw(new std::invalid_argument(" cannot create such object."));
	}

	P* Create(const K &k) throw (std::invalid_argument) {
		(void) k;
		throw(new std::invalid_argument(" cannot create such object. Get should be called."));
	}
	void _toCache(const K& key, P* product) {
		(void) key;
		(void) product;
	}
	P* _fromCache(const K& key) {
		(void) key;
		return NULL;
	}
};
/*****/

template<class P, class K>
std::set<Factory<P, K>*> * Factory<P, K>::_x_instances_ = NULL;

template<class P, class K>
Factory<P, K> * Factory<P, K>::_singleton_ = NULL;

/*******************************attribute-set ****************************/
/*!
 * \brief An attribute is a templated key in an AttributeSet.
 */
template<typename V>
class Attribute {
public:
	/*!
	 * \brief Construct an attribute for a given name.
	 * @param name string (name of the Attribute to create.)
	 */
	Attribute(const std::string& name) :
			__name(name) {
	}
	/*!
	 * \brief Copy constructor of Attribute class.
	 * @param other copy origin
	 */
	Attribute(const Attribute<V>&other) :
			__name(other.__name) {
	}
	virtual ~Attribute() {
	}
	/// Based on operator==. Returns true (equals) if both attribute objects are equal.
	template<typename U>
	bool operator ==(const util::Attribute<U>& other) {
		return INSTANCEOF(other, Attribute<V>) && (other.__name == this->__name);
	}

	bool operator ==(const util::Attribute<V>& other) {
		return (other.__name == this->__name);
	}
	/// Based on operator!=. Returns true (not equals) if both attribute objects are not equal.
	template<typename U>
	bool operator !=(const util::Attribute<U> &other) {
		return (!(*this == other));
	}
	bool operator !=(const util::Attribute<V> &other) {
		return (!(*this == other));
	}

	bool operator <(const util::Attribute<V> &other) const {
		return this->__name < other.__name;
	}
	friend std::ostream& operator <<(std::ostream& o_s,
			const Attribute<V> & item) {
		o_s << " [ Attribute<V=" << typeid(V).name() << "> = Type= "
				<< typeid(item).name() << "; Name= " << item.getName() << " ]";
		return o_s;

	}
	/*!
	 * Returns the name of this attribute.
	 * @return the name of this attribute.
	 */
	const std::string& getName() const {
		return __name;
	}
private:
	std::string __name;
};
/**
 * \brief  This class represents map that maps unique key-Attributes to values.<br>
 */
template<typename V>
class AttributeSet {
public:
	/*!
	 * \brief Construct an empty attribute set
	 */
	AttributeSet();
	/*!
	 * \brief Construct an empty attribute set
	 * @param read_only to indicate if this attribute set is read only or not.
	 */
	AttributeSet(bool read_only);

	/*!
	 * \brief Copy constructor of Attribute set class.
	 * @param other copy origin
	 */
	AttributeSet(const AttributeSet<V>&other);

	virtual ~AttributeSet();
	/*!
	 * Tests if the specified object identified by the key in contained this set.
	 * @param k  the attribute key.
	 * @return true if the set contains that object.
	 */
	virtual const bool Contains(const Attribute<V>&k) const;
	/*!
	 * \brief Returns an attribute of a given key.
	 * @param k the attribute key.
	 * @return V typed value identified by the given attribute key.
	 */
	virtual const V& Get(const Attribute<V>&k) const throw (std::out_of_range);
	/*!
	 * \brief Puts a attribute-key/value pair in this set
	 * @param k the attribute-key.
	 * @param v the V-typed value.
	 * @throw std::logic_error if the set is in read-only mode.
	 */
	virtual void Insert(const Attribute<V>&k, const V& v)
			throw (std::logic_error);

	/*!
	 * \brief removes an attribute-key/value pair from this set
	 * @param k the attribute-key.
	 * @throw std::logic_error if the set is in read-only mode.
	 * @note if the set doesn't contain the attribute key, no exception will be raised and the method ends silentely.
	 */
	virtual void Remove(const Attribute<V>&k) throw (std::logic_error);
	/*!
	 *\brief indicates if the attribute set is writable or not.
	 * @return true if this attribute set is not read-only.
	 */

	bool IsWriteable() const;
	/*!
	 * \brief  Returns the keys of this set.
	 * @return and std::set that contains all present attribute - keys.
	 */
	virtual const std::set<Attribute<V> > Keys() const;
	/*!
	 *  \brief Removes all the keys.
	 *  @throw std::logic_error if the set is in read-only mode.
	 */
	virtual void Clear() throw (std::logic_error);
	/*!
	 *\brief  Returns true if the %AttributeSet is empty. (Size = 0)
	 * @return Returns true if empty.
	 */
	bool Empty() const;
	/** Returns the size of the %AttributeSet.  */
	size_t Size() const;

protected:
	void __put_value(const Attribute<V>&k, const V& v);
	void __rm_value(const Attribute<V>&k);

private:
	bool __read_only;
	std::map<std::string, V> __values;
	std::vector<Attribute<V> > __att_keys;
};
/* IMPL OF ATTR SET*/
template<typename V>
AttributeSet<V>::AttributeSet() :
		__read_only(false) {
}
template<typename V>
AttributeSet<V>::AttributeSet(bool read_only) :
		__read_only(read_only) {
}

template<typename V>
AttributeSet<V>::AttributeSet(const AttributeSet<V>&other) :
		__read_only(other.__read_only), __values(
				std::map<std::string, V>((other.__values))), __att_keys(
				std::vector<Attribute<V> >(other.__att_keys)) {

}
template<typename V>
AttributeSet<V>::~AttributeSet() {
}
template<typename V>
const bool AttributeSet<V>::Contains(const Attribute<V>&k) const {
	return this->__values.count(k.getName());
}
template<typename V>
const V& AttributeSet<V>::Get(const Attribute<V>&k) const
		throw (std::out_of_range) {
	return __values.find(k.getName())->second;
}
template<typename V>
bool AttributeSet<V>::IsWriteable() const {
	return !__read_only;
}

template<typename V>
void AttributeSet<V>::Clear() throw (std::logic_error) {
	if (__read_only)
		throw(new std::logic_error("read only attribute!"));
	__values.clear();						//std::map<std::string, V> __values;
	__att_keys.clear();
}
template<typename V>
bool AttributeSet<V>::Empty() const {
	return __values.empty();
}
template<typename V> size_t AttributeSet<V>::Size() const {
	return __values.size();
}
template<typename V>
void AttributeSet<V>::Insert(const Attribute<V>&k, const V& v)
		throw (std::logic_error) {
	DLOG_INF();
	if (__read_only) {
		DLOG_LOGL("read only set!!"); DLOG_OUTF();
		throw std::logic_error("cannot insert in a read only attribute!");
	}
	DLOG_LOGL("ITS OK!!!");
	__put_value(k, v);
	DLOG_OUTF();
}
template<typename V>
const std::set<Attribute<V> > AttributeSet<V>::Keys() const {
	std::set<Attribute<V> > keys(__att_keys.begin(), __att_keys.end());
	return keys;
}
template<typename V>
void AttributeSet<V>::Remove(const Attribute<V>&k) throw (std::logic_error) {
	if (__read_only)
		throw(std::logic_error("read only attribute!"));
	__rm_value(k);
}
template<typename V>
void AttributeSet<V>::__put_value(const Attribute<V>&k, const V& v) {
	__values.insert(std::pair<std::string, V>(k.getName(), v));
	__att_keys.push_back(k);
}
template<typename V>
void AttributeSet<V>::__rm_value(const Attribute<V>&k) {
	try {
		__values.erase(k.getName());
		// taken from Item 32 of Scott Myers' Effective STL
		__att_keys.erase(std::remove(__att_keys.begin(), __att_keys.end(), k),
				__att_keys.end());
	} catch (...) {
	}
}
/*!
 *  \def ATTR_EXTENDS_M(TYPE)
 *  \brief  Macro that will be used when creating multi-typed attribute by inheriting different typed AttributeSet.
 *! For example,see the utils.h examples
 */
#define ATTR_EXTENDS_M(TYPE) using util::AttributeSet<TYPE>::Insert;\
							 using util::AttributeSet<TYPE>::Contains;\
							 using util::AttributeSet<TYPE>::Get;\
							 using util::AttributeSet<TYPE>::Remove;
/* DOXY Expls.*/
/*!
 *
 * @example
 * <i><b>Here are some small examples about how to use &quot;util&quot; name space classes:</b></i>
 * \n \n 1- Use of macro <b>synchronized(M)</b>  to create mutual exclusion lock block.
 *  \code
 *  Mutex m;
 *  cout << "entering exclusion lock block" << endl;
 * synchronized (m)
 * {
 * // ... do some stuff ...
 * }
 * cout << "leaving exclusion lock block" << endl;
 * \endcode
 * \n 2- <b>Thread</b> use with a Runnable instance .
 *  \code
 *
 * class MyRun: public Runnable
 * {
 * public:
 * virtual ~MyRun()
 * {
 * //...
 * }
 * void Run(void *d)
 * {
 * // ... do some stuff ...
 * }
 * } task;
 * Thread t(&task);
 * t.Start(NULL);
 * t.Join();
 * \endcode
 *\n 3- <b>Thread</b>  waiting on <b>Mutex</b> within <b>synchronized</b> bloc..
 * \code
 * Mutex m;
 * cout << "entering exclusion lock block" << endl;
 * synchronized(m)
 *  {
 * m.Wait(1000);// wait for 1 seconds Mutex could be notified by another thread during this time.
 * }
 * cout << "leaving exclusion lock block" << endl;
 * \endcode
 *
 * \n 4- Use of <b>EventQueue</b> asynchronous event dispatching mechanism  .
 * \code
 * class Event: public EventObject<string>
 * {
 * public:
 * Event(string* src, int type) :
 * EventObject<string>(src), mtype(type)
 * {}
 * int GetType()
 * {
 * return mtype;
 * }
 * private:
 * int mtype;
 * };
 *
 *  class Listener: public EventListener<Event>
 * {
 * public:
 * virtual ~Listener()
 * {}
 * void onEvent(Event* evt)
 * {
 * cout << " Event occurs [type = " << evt->GetType() << "]"<< endl;
 * }
 * };
 *
 *  static void eq_test()
 * {
 * util::EventQueue<Event>* eq = new util::EventQueue<Event>();
 * eq->AddQListener(new Listener);
 * //Post 5 events
 * for (int i = 0; i < 5; i++)
 * {
 * eq->Post(new Event(new string(" MY EVT"), i));
 * }
 * }
 * \endcode

 *\n 5- Synchronizing a thread on a asynchronous event listening.
 *\n \code
 class OListener: public EventListener<OEvent>
 {
 public:
 Mutex m;
 bool coin;
 PListener() :coin(false)
 {}
 ~OListener()
 {}
 void onEvent(OEvent* evt)
 {
 synchronized(m)
 {
 coin = true;//set coin
 m.Notify();//wakeup waiting thread.
 }
 }
 };
 AnyObservable o();
 OListener *l = new OListener;
 o->AddListener(l);
 o->DoStuff();//start asynchronous action
 synchronized(l->m)
 {
 if (!l->coin) // check the coin
 l->m.Wait();//waiting for asynchronous event.
 }
 * \endcode
 *
 * \n -6 Use of <b>AttributeSet</b> :
 * \n \code
 class MultiAttributeSet: public util::AttributeSet<bool>,public util::AttributeSet<int>,public util::AttributeSet<double>,public util::AttributeSet<std::string>
 {
 public:
 MultiAttributeSet() : util::AttributeSet<bool>(), util::AttributeSet<int>(), util::AttributeSet<std::string>() { }
 MultiAttributeSet(bool ro) : util::AttributeSet<bool>(ro), util::AttributeSet<int>(ro), util::AttributeSet<std::string>(ro) { }
 MultiAttributeSet(const MultiAttributeSet&other) : util::AttributeSet<bool>(other), util::AttributeSet<int>(other), util::AttributeSet<std::string>(other) { }
 ATTR_EXTENDS_M(double)
 ATTR_EXTENDS_M(bool)
 ATTR_EXTENDS_M(int)
 ATTR_EXTENDS_M(std::string)

 bool Empty()
 {
 bool e = util::AttributeSet<bool>::Empty();
 e &= util::AttributeSet<double>::Empty();
 e &= util::AttributeSet<int>::Empty();
 e &= util::AttributeSet<std::string>::Empty();
 return e;
 }

 bool IsWriteable() { return util::AttributeSet<bool>::IsWriteable(); }
 size_t Size() const
 {
 size_t s = util::AttributeSet<bool>::Size();
 s += util::AttributeSet<int>::Size();
 s += util::AttributeSet<double>::Size();
 s += util::AttributeSet<std::string>::Size();
 return s;
 }
 void Clear()throw (std::logic_error)
 {
 util::AttributeSet<bool>::Clear();
 util::AttributeSet<int>::Clear();
 util::AttributeSet<double>::Clear();
 util::AttributeSet<std::string>::Clear();

 }
 };


 void using_it(){
 MultiAttributeSet mset(false);
 mset.Insert(Attribute<std::string> ("STR K 1"), "String1");
 mset.Insert(Attribute<bool> ("BL K 1"), true);
 std::cout<<"mset.get(STF K 1)="<<mset.Contains(Attribute<std::string> ("STR K 1"))<<std::endl;
 std::cout<<"mset.get(BL K 1)="<<mset.Contains(Attribute<bool> ("BL K 1"))<<std::endl;
 MultiAttributeSet mset_copy(mset);
 const std::set< Attribute<std::string> > s = (( util::AttributeSet<std::string>)mset_copy).Keys();
 std::cout << "string set keys :" << std::endl;
 for (std::set<Attribute<std::string> >::iterator it = s.begin(); it != s.end(); ++it)
 std::cout << ">set key =" << *it;
 mset_copy.Clear();
 std::cout << "is mset_copy empty ?=" << mset_copy.Empty() << " s=" << mset_copy.Size() << std::endl;
 std::cout << "is mset empty ?=" << mset.Empty() << " s=" << mset.Size() << std::endl;
 }
 * \endcode
 *
 *\n 7- Create an abstract & specialized Factory: an example of a factory who will create (when extended) a specific Player for a given Url (depends on Url protocol)
 *\n \code
 * class PlayerFactory: public util::Factory< Player, Url> {
 *public:

 *	// *\brief Creates a Player object for a given Url parameter.
 *	// * @param url that will act as the data source of the returned player.
 *	// * @return Player pointer if there is an appropriate player type for the url"s protocol.
 *	// * @throw invalid_argument if no player could be created for this url.
 *
 *	virtual Player* Create(const Url &url) throw (std::invalid_argument)=0;
 *
 *protected:
 *
 *	 // \brief  Construct a new instance of PlayerFactory that should be registered to Factory (through Factory#Register()).
 *
 *	PlayerFactory();
 *	virtual ~PlayerFactory();
 *
 *	void _toCache(const  Url& key, Player* player);
 *	Player* _fromCache(const Url& key);
 * private:
 *	std::map<std::string, Player*> *mymap;
 *};
 *
 *}\n \endcode
 */

}

#endif /* UTILS_H_ */

