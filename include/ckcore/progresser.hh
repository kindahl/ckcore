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

/**
 * @file include/ckcore/progresser.hh
 * @brief Defines the class for progressing progress interfaces.
 */

#pragma once
#include "ckcore/types.hh"
#include "ckcore/progress.hh"

namespace ckcore
{
	/**
	 * @brief Class for calculating and updating progress.
	 *
	 * This class calculates the total progress from partially processed data.
	 */
	class Progresser
	{
	private:
		Progress &progress_;
		tuint64 total_;
		tuint64 count_;

	public:
		Progresser(Progress &progress,tuint64 total);

		void update(tuint64 count);
		void notify(Progress::MessageType type,const tchar *format,...);

		bool cancelled();
	};
};

