/*	Copyright (C) 2004 Garrett A. Kajmowicz

	This file is part of the uClibc C++ Library.  This library is free
	software; you can redistribute it and/or modify it under the
	terms of the GNU General Public License as published by the
	Free Software Foundation; either version 2, or (at your option)
	any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with this library; see the file COPYING.  If not, write to the Free
	Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
	USA.
*/

#include <cstdlib>
#include <typeinfo>
#include <basic_definitions>

/* This file implements a number of the language support features
 * needed to deal with the C++ abi, as originally documented in the
 * Itanium C++ ABI, though now industry standard
 */

extern "C" {

#if defined(ARDUINO)
        /* Arduino defines some of these.
	 * There can be link issues if they're redefined
         */
#else
	/* This function is called in the event that a non-overidden
	 * pure virtual function is called.  The compiler should never
	 * let that happen.  We get to choose what to do - we will abort
	 */
	void __cxa_pure_virtual (){
		abort();
	}

#endif
}
