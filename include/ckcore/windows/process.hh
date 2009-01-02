/*
 * The ckCore library provides core software functionality.
 * Copyright (C) 2006-2008 Christian Kindahl
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
 * @file include/ckcore/windows/process.hh
 * @brief Defines the Windows process class.
 */
#pragma once
#include <vector>
#include "ckcore/types.hh"
#include "ckcore/stream.hh"

namespace ckcore
{
    /**
     * @brief The class for creating processes on Unix.
     */
    class Process : public OutStream
    {
    public:
        friend class ProcessMonitor;

    protected:
        volatile bool invalid_inheritor_;   // Set to true to indicate that the
                                            // inheritor is no loner valid and
                                            // thuss the events are purely
                                            // virtual.

    private:
        enum
        {
            MAX_ARG_COUNT = 127,
            READ_BUFFER_SIZE = 128
        };

        enum State
        {
            STATE_STOPPED,
            STATE_RUNNING
        };

        HANDLE pipe_stdin_;             // Pipe handle for standard input.
        HANDLE pipe_output_;            // Pipe handle for both standard output and standard error.

        HANDLE process_handle_;
        HANDLE stop_event_;
        HANDLE thread_handle_;
        volatile unsigned long thread_id_;  // Thread identifier.
        volatile State state_;              // Process state.

        std::string line_buffer_;       // For buffering partial standard output lines before commiting them.

        void close();
        bool read_output(int fd);

        // For multi-threading.
        HANDLE mutex_;
        HANDLE mutex_exec_;             // This mutex will be locked through the entire execution.
        static unsigned long WINAPI listen(void *param);

    public:
        Process();
        virtual ~Process();

        bool create(const tchar *path,std::vector<tstring> &args);
        bool running() const;
        bool wait() const;

        // OutStream interface.
        tint64 write(void *buffer,tuint32 count);

        /**
         * Called when the process has finished.
         */
        virtual void event_finished() = 0;

        /**
         * Called when a line has been read from either standard output or
         * standard error.
         * @param [in] line The line that has been read.
         */
        virtual void event_output(const std::string &line) = 0;
    };
};

