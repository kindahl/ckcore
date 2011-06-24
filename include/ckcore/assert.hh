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

/**
 * @file include/ckcore/assert.hh
 * @brief Assertion support.
 */

#pragma once
#if defined(_WINDOWS)
#  include <crtdbg.h>
#elif defined(_UNIX)
#  #include <cassert>
#endif
#include "ckcore/types.hh"

#ifdef _WINDOWS
#  ifndef ckASSERT
#    define ckASSERT(expr) _ASSERTE(expr)
#  else
#    define ckASSERT(expr) assert(expr)
#  endif
#endif

#ifndef ckVERIFY
#  ifdef _DEBUG
#    define ckVERIFY(expr) ckASSERT(expr)
#  else
#    define ckVERIFY(expr) (expr)
#  endif
#endif

#ifndef ckDEBUG
#  ifdef _DEBUG
#    define ckDEBUG(expr) (expr)
#  else
#    define ckDEBUG(ignore) ((void)0)
#  endif
#endif
