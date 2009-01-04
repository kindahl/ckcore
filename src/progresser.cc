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

#include <stdarg.h>
#include "ckcore/progresser.hh"

namespace ckcore
{
    /**
     * Constructs a Progresser object.
	 * @param [in] progress The progress interface to report the progress to.
	 * @param [in] total The total number of units to progress.
     */
	Progresser::Progresser(Progress &progress,tuint64 total) :
		progress_(progress),total_(total),count_(0)
	{
	}

	/**
	 * Updates the progress depending on the number of units processed.
	 * @param [in] count The number of units processed..
     */
	void Progresser::update(tuint64 count)
	{
		count_ += count;
		progress_.set_progress((unsigned char)(((double)count_/total_) * 100));
	}

	/**
	 * Transmits a message to the progress interface. This message i
	 * intended to be displayed to the end user.
	 * @param [in] type The type of message.
	 * @param [in] format The message format.
	 */
	void Progresser::notify(Progress::MessageType type,const tchar *format,...)
	{
		va_list ap;
		va_start(ap,format);

		progress_.notify(type,format,ap);

		va_end(ap);
	}

	/**
	 * Checks wether the operation has been cancelled or not
	 * @return If the process has been cancelled true is returned, if no
	 *		   cancelled false is returned.
	 */
	bool Progresser::cancelled()
	{
		return progress_.cancelled();
	}
};

