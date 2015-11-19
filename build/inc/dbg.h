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
 * dbg.h
 *
 *  Created on: Jan 2, 2014
 *      Author: tyazid
 */

#ifndef DBG_H_
#define DBG_H_
#include <iostream>     // std::cout

namespace cpprt_log{
#ifdef DEBUG
extern std::ostream& dbg();
#define DLOG_LOG(L) do{std::cout<< L  ;}while(0)
#define DLOG_LOGL(L) DLOG_LOG(L<<std::endl)
#define DLOG_INF() DLOG_LOGL( " [in] " << __FILE__ << "::" << __FUNCTION__ << "()")
#define DLOG_OUTF() DLOG_LOGL(" [out] " << __FILE__ << "::" << __FUNCTION__ << "()")
#define DLOG_AT() DLOG_LOGL( " >< " << __FILE__ << "::" << __FUNCTION__ << "() @"<<__LINE__)
#else
#define DLOG_LOG(L)
#define DLOG_LOGL(L)
#define DLOG_INF()
#define DLOG_OUTF()
#define DLOG_AT()
#endif
}


#endif /* DBG_H_ */
