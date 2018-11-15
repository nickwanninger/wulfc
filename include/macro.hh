/*
 * A compiler for the wulf language
 * Copyright (C) 2018  Nick Wanninger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#ifndef __MACRO_HH
#define __MACRO_HH

#include <value.hh>
#include <scope.hh>
#include <vector>
#include <string>

namespace macro {
	class Expansion {
		public:
			std::string name;
			std::vector<value::Argument> args;
			value::Object body;
			// expand a macro using the argument literal values
			value::Object expand(vm::Machine*, std::vector<value::Object>, scope::Scope*);
	};
}

#endif