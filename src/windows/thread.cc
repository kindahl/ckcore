/*
 * The ckCore library provides core software functionality.
 * Copyright (C) 2006-2011 Christian Kindahl
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.hh"

#include <atlbase.h>
#include <atlapp.h>

#include <memory>
#include "ckcore/thread.hh"

namespace ckcore
{
	namespace thread
	{
		/**
		 * @brief Class for passing parameters to a native thread instance.
		 */
		class NativeThreadParam
		{
		public:
			tfunction func_;
			void *param_;
			
			NativeThreadParam(tfunction func,void *param) :
				func_(func),param_(param)
			{
			}
		};

		/**
		 * The main thread entry point for new threads.
		 * @param [in] param The thread parameter.
		 * @return Always returns 0.
		 */
		unsigned long __stdcall native_thread(void *param)
		{
			std::auto_ptr<NativeThreadParam> native_param(
				static_cast<NativeThreadParam *>(param));
			native_param->func_(native_param->param_);

			return 0;
		}

		/**
		 * Creates and starts the execution of a new thread.
		 * @param [in] func The thread function entry point.
		 * @param [in] param Optional thread parameter.
		 * @return If the thread was successfully created true is returned, if
		 *		   not false is returned.
		 */
		bool create(tfunction func,void *param)
		{
			// Setup the parameters to pass to the native thread.
			NativeThreadParam *native_param = new NativeThreadParam(func,param);
			if (native_param == NULL)
				return false;

			// Create the thread.
			unsigned long thread_id = 0;
			if (CreateThread(NULL,0,native_thread,native_param,0,&thread_id) == NULL)
			{
				delete native_param;
				return false;
			}
			
			return true;
		}

		/**
		 * Sleeps the current thread for a specified amount of milliseconds.
		 * @param [in] milliseconds The number of milliseconds to sleep the
		 *							thread.
		 * @return If successful true is returned, if not false is returned.
		 */
		bool sleep(ckcore::tuint32 milliseconds)
		{
			Sleep(milliseconds);
			return true;
		}

		/**
		 * Constructs a Mutex object.
		 */
		Mutex::Mutex() : handle_(CreateMutex(NULL,FALSE,NULL)),locked_(false)
		{
		}

		/**
		 * Destructs the Mutex object.
		 */
		Mutex::~Mutex()
		{
			if (handle_ != NULL)
			{
				ATLVERIFY( 0 != CloseHandle(handle_) );
				handle_ = NULL;
			}
		}

		/**
		 * Locks the mutex.
		 * @return If successful true is returned, if unsuccessful false is
		 *		   returned.
		 */
		bool Mutex::lock()
		{
			if (handle_ == NULL || locked_)
				return false;

			return locked_ = WaitForSingleObject(handle_,INFINITE) == WAIT_OBJECT_0;
		}

		/**
		 * Unlocks the mutex.
		 * @return If successful true is returned, if unsuccessful false is
		 *		   returned.
		 */
		bool Mutex::unlock()
		{
			if (handle_ == NULL || !locked_)
				return false;

			return !(locked_ = !(ReleaseMutex(handle_) == TRUE));
		}
	};
};
