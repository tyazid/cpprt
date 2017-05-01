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
 * si_internal.h
 *
 *  Created on: Sep 12, 2013
 *      Author: tyazid
 */

#ifndef SI_INTERNAL_H_
#define SI_INTERNAL_H_
#include <set>
#include <vector>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <queue>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <climits>
#include <errno.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <cassert>
#include "../inc/cpprt.h"
//#include <mutex>
//#include <thread>
#include <vector>
#include <queue>          // std::queue
#include <signal.h>
#include <time.h>
#include <string>
#include <iostream>
#include <typeinfo>
#include <memory.h>
#include <pthread.h>
#include <exception>
#include <stdexcept>
#include <stdio.h>

#define EQUAL_TIME(TM1,TM2) (difftime(mktime(TM1), mktime(TM2)) == 0.0)
#define INIT(V,TYPE) do{V = new TYPE; memset(V, 0, sizeof(TYPE));}while(0)
#define DEL(V) if(V){cout<< "DELETE OBJ" << typeid(V).name() << endl;delete V; V=NULL;}
#define NOT_EQ(THIS,OTHER) (!(THIS == OTHER))
std::ostream& operator<<(std::ostream& stream, tm& time);

#if defined _WIN32
#define EQ_TH(T1,T2) (T1.x == T2.x)
#define TH_LOG(T) T.x
#else
#define EQ_TH(T1,T2) (T1 == T2)
#define TH_LOG(T) T
#endif


#endif /* SI_INTERNAL_H_ */
