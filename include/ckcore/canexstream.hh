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
 * @file include/ckcore/canexstream.hh
 * @brief Implementation of stream interfaces for dealing with files.
 */

#pragma once
#include "ckcore/types.hh"
#include "ckcore/stream.hh"

namespace ckcore
{
    /**
     * @brief Canonical input stream using exceptions.
     */
    class CanexInStream
    {
    private:
        InStream &stream_;
		tstring ident_;

    public:
		virtual ~CanexInStream() {}

        CanexInStream(InStream &stream,const tchar *ident);
		CanexInStream(InStream &stream,const tstring &ident);

		virtual void seek(tuint32 distance,InStream::StreamWhence whence);
        virtual tint64 read(void *buffer,tuint32 count);
		bool end();
    };

    /**
     * @brief Canonical output stream using exceptions.
     */
    class CanexOutStream
    {
    private:
        OutStream &stream_;
		tstring ident_;

    public:
		virtual ~CanexOutStream() {}

		CanexOutStream(OutStream &stream,const tchar *ident);
		CanexOutStream(OutStream &stream,const tstring &ident);

        virtual void write(void *buffer,tuint32 count);
    };

	namespace canexstream
    {
		void copy(CanexInStream &from,CanexOutStream &to,Progresser &progresser);
		void copy(CanexInStream &from,CanexOutStream &to,Progresser &progresser,
				  tuint64 size);
    };
};

