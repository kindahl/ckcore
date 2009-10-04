/*
 * The ckCore library provides core software functionality.
 * Copyright (C) 2006-2009 Christian Kindahl
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
		 * @return Always returns NULL.
		 */
		void *native_thread(void *param)
		{
			std::auto_ptr<NativeThreadParam> native_param(
				static_cast<NativeThreadParam *>(param));
			native_param->func_(native_param->param_);

			return NULL;
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
            pthread_t thread;
            if (pthread_create(&thread,NULL,native_thread,native_param) != 0)
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
			return usleep(milliseconds * 1000) == 0;
		}

		/**
		 * Constructs a Mutex object.
		 */
		Mutex::Mutex() : initialized_(false),locked_(false)
		{
            initialized_ = pthread_mutex_init(&mutex_,NULL) == 0;
		}

		/**
		 * Destructs the Mutex object.
		 */
		Mutex::~Mutex()
		{
			if (initialized_)
			{
                if (pthread_mutex_destroy(&mutex_) == 0)
                    initialized_ = false;
			}
		}

		/**
		 * Locks the mutex.
		 * @return If successful true is returned, if unsuccessful false is
		 *		   returned.
		 */
		bool Mutex::lock()
		{
			if (!initialized_ || locked_)
				return false;

            return locked_ = pthread_mutex_lock(&mutex_) == 0;
		}

		/**
		 * Unlocks the mutex.
		 * @return If successful true is returned, if unsuccessful false is
		 *		   returned.
		 */
		bool Mutex::unlock()
		{
			if (!initialized_ || !locked_)
				return false;

			return !(locked_ = !(pthread_mutex_unlock(&mutex_) == 0));
		}
	}
}

