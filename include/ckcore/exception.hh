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
 * @file include/ckcore/exception.hh
 * @brief Defines the exception base class.
 */

#pragma once
#include "ckcore/types.hh"

namespace ckcore
{
    /**
     * @brief Exception base class.
     */
    class Exception
    {
	protected:
		tstring message_;

    public:
		/**
		 * Constructs an Exception object.
		 * @param [in] message The message describing the reason for the error.
		 */
		Exception(const tchar *message) : message_(message) {}

		/**
		 * Constructs an Exception object.
		 * @param [in] message The message describing the reason for the error.
		 */
		Exception(const tstring &message) : message_(message) {}

		/**
		 * Constructs an Exception object without an error mesasge.
		 */
		Exception() {}

        virtual ~Exception() {};

		/**
		 * Returns the message describing the reason for the error.
		 * @return The message describing the reason for the error.
		 */
		virtual const tstring &what() const
		{
			return message_;
		}
    };
};

