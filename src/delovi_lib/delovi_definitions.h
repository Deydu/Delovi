/*****************************************************************************
 *
 * Copyright (C) 2016 Denny Duphorn <deydu@gmx-topmail.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License version 2
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * Alternatively, you find an online version of the license text under
 * http://www.gnu.org/licenses/gpl-2.0.html.
 *
 *****************************************************************************/



#pragma once



#define DELOVI_DEBUGGING_ENABLED
#ifdef NDEBUG
#	undef DELOVI_DEBUGGING_ENABLED
#endif // NDEBUG

#ifdef DELOVI_DEBUGGING_ENABLED
#	ifdef QT_NO_DEBUG_OUTPUT
#		undef QT_NO_DEBUG_OUTPUT
#	endif
#else
#	ifndef QT_NO_DEBUG_OUTPUT
#		define QT_NO_DEBUG_OUTPUT
#	endif
#endif // DELOVI_DEBUGGING_ENABLED
// QT_NO_WARNING_OUTPUT
// QT_FATAL_WARNINGS ==> warning -> exit

#define DELOVI_VERSION_MAJOR 1u
#define DELOVI_VERSION_MINOR 1u
#define DELOVI_VERSION_BUILD 4u

//#define DELOVI_FILE_DIALOG_OPTIONS QFileDialog::DontUseNativeDialog
#define DELOVI_FILE_DIALOG_OPTIONS 0

#define DELOVI_CONFIGURATION_FILENAME ".delovi.cfgx"

#include <delovi_compiler_detection.h>
