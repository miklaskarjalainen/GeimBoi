#ifndef _GB_PRINT_H
#define _GB_PRINT_H

#ifdef _GB_DISABLE_ANSI_CODES
	#define GB_COLOR_RESET
	#define GB_COLOR_RED
	#define GB_COLOR_YELLOW
	#define GB_COLOR_GREEN
	#define GB_COLOR_CYAN
	#define GB_COLOR_BOLDRED
#else
	#define GB_COLOR_RESET "\x1b[0m"
	#define GB_COLOR_RED "\x1b[31m"
	#define GB_COLOR_YELLOW "\x1b[33m"
	#define GB_COLOR_GREEN "\x1b[32m"
	#define GB_COLOR_CYAN "\x1b[36m"
	#define GB_COLOR_BOLDRED "\x1b[1;31m"
#endif

#define GB_STR(s) #s
#define GB_XSTR(s) GB_STR(s)

#if defined(_GB_DISABLE_STDLIB) || defined(_GB_DISABLE_PRINT)
	#define GB_INFO(...)
	#define GB_WARN(...)
	#define GB_ERROR(...)
	#define GB_FATAL(...)
	#define GB_ASSERT(...)
#else
	#include <stdio.h>
	#include <stdlib.h>

	#ifndef _GB_LOG_OUT
		#define _GB_LOG_OUT stdout
	#endif
	#ifndef _GB_LOG_ERR
		#define _GB_LOG_ERR stderr
	#endif

	#define GB_INFO(fmt, ...)                                                  \
		do {                                                                   \
			fprintf(                                                           \
				_GB_LOG_OUT,                                                   \
				GB_COLOR_CYAN "[INFO] " GB_COLOR_RESET fmt "\n",               \
				__VA_ARGS__                                                    \
			);                                                                 \
		} while (0)

	#define GB_WARN(fmt, ...)                                                  \
		do {                                                                   \
			fprintf(                                                           \
				_GB_LOG_OUT,                                                   \
				GB_COLOR_YELLOW "[WARN] " GB_COLOR_RESET fmt "\n",             \
				__VA_ARGS__                                                    \
			);                                                                 \
		} while (0)

	#define GB_ERROR(fmt, ...)                                                 \
		do {                                                                   \
			fprintf(                                                           \
				_GB_LOG_ERR,                                                   \
				GB_COLOR_RED "[ERROR] " fmt GB_COLOR_RESET "\n",               \
				__VA_ARGS__                                                    \
			);                                                                 \
		} while (0)

	#define GB_FATAL(...)                                                      \
		do {                                                                   \
			fprintf(                                                           \
				_GB_LOG_ERR,                                                   \
				GB_COLOR_BOLDRED "[FATAL] " GB_COLOR_RESET __VA_ARGS__         \
			);                                                                 \
			fprintf(_GB_LOG_ERR, "\n");                                        \
			exit(EXIT_FAILURE);                                                \
		} while (0)

	#define GB_ASSERT(expr, ...)                                               \
		if (!(expr)) {                                                         \
			fprintf(                                                           \
				_GB_LOG_ERR,                                                   \
				GB_COLOR_BOLDRED                                               \
				"[ASSERTION] " __FILE__                                        \
				":" GB_XSTR(__LINE__) ":  " GB_COLOR_RESET __VA_ARGS__         \
			);                                                                 \
			fprintf(_GB_LOG_ERR, "\n");                                        \
			exit(EXIT_FAILURE);                                                \
		}
#endif

#endif
