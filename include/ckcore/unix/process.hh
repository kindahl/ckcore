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
 * @file include/ckcore/unix/process.hh
 * @brief Defines the Unix process class.
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
            FD_READ = 0,
            FD_WRITE = 1
        };

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

        int pipe_stdin_[2];
        int pipe_stdout_[2];
        int pipe_stderr_[2];

        volatile pid_t pid_;            // Process identifier.
        volatile State state_;          // Process state.

        std::string line_buffer_out_;   // For buffering partial standard output lines before commiting them.
        std::string line_buffer_err_;   // For buffering partial standard error lines before commiting them.

        void close();
        bool read_stdout(int fd);
        bool read_stderr(int fd);

        // For multi-threading.
        pthread_mutex_t mutex_;
        pthread_mutex_t mutex_exec_;    // This mutex will be locked through the entire execution.
        static void *listen(void *param);

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

