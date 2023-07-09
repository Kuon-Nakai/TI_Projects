#include <Common.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
char *format(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	char *buf = (char *)malloc(FORMAT_SIZE);
	vsprintf(buf, fmt, ap);
	return buf;
}
