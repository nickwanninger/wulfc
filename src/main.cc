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

#include <wulf.hh>
#include <state.hh>
#include <map>
#include <unordered_map>
#include <scope.hh>
#include <thread>
#include <value.hh>
#include <vm.hh>
#include <value.hh>
#include <parser.hh>
#include <scanner.hh>
#include <getopt.h>
#include <unistd.h>
#include <autonum.hh>
#include <stdlib.h>
#include <dlfcn.h>

#define USE_GC

#ifdef USE_GC
#define GC_THREADS
#include <gc.h>
#include <gc/gc_cpp.h>
#endif

#define EXIT_FILE_ERROR 1
#define STDIN_READ_SIZE 100

#define PROMPT_HEADER \
	"Wulf Lisp\n" \
	"Type 'wulf/copyright', 'wulf/warranty' or, 'wulf/help' for more information"\

int main(int argc, char** argv) {

#ifdef USE_GC
	GC_INIT();
	GC_allow_register_threads();
#endif

	// setup the random number generator
	srand((unsigned int)time(nullptr));

	try {
		bool interactive = false;
		auto *state = new State();
		char opt;
		while ((opt = getopt(argc, argv, "i")) != -1) {
			switch (opt) {
				case 'i': interactive = true; break;
				default:
					fprintf(stderr, "Usage: %s [-i] [file]\n", argv[0]);
					exit(EXIT_FAILURE);
			}
		}

		if (optind >= argc) {
			if (isatty(fileno(stdin))) {
				std::cout << PROMPT_HEADER << "\n";
				state->run_repl();
			} else {
				std::string contents;
				char buffer[STDIN_READ_SIZE];
				while (fgets(buffer, STDIN_READ_SIZE, stdin)) {
					contents += buffer;
				}
				state->eval(const_cast<char*>(contents.c_str()));
			}
			delete state;
			exit(0);
		}


		// if there was a file argument, evaluate that instead
		apathy::Path filepath = apathy::Path::cwd().relative(argv[optind]);
		std::string fp_str = filepath.string();
		state->eval_file(ccharcopy(fp_str.c_str()));

		// if the user requested interactive mode, do that.
		if (interactive) {
			state->run_repl();
		}

		delete state;

	} catch (std::string err) {
		std::cerr << "Error: " << err << "\n";
	} catch (const char* err) {
		std::cerr << "Error: " << err << "\n";
	} catch (std::bad_weak_ptr err) {
		std::cerr << err.what() << "\n";
	}

	return 0;
}

#ifdef USE_GC

#define alloc_fn GC_MALLOC
#define free_fn GC_FREE
void* operator new(size_t size) {
	return alloc_fn(size);
}
void* operator new[](size_t size) {
	return alloc_fn(size);
}


#ifdef __GLIBC__
#define _NOEXCEPT _GLIBCXX_USE_NOEXCEPT
#endif
// delete operators should NOP because it would be faster
// for the GC to just pick it up (dont know why, just is)
void operator delete(void* ptr) _NOEXCEPT {
	// std::cout << "free " << ptr << std::endl;
	free_fn(ptr);
}
void operator delete[](void* ptr) _NOEXCEPT {
	// std::cout << "free[] " << ptr << std::endl;
	free_fn(ptr);
}

#endif
