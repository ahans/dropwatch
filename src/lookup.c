/*
 * Copyright (C) 2009, Neil Horman <nhorman@redhat.com>
 * 
 * This program file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program in a file named COPYING; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 */

/*
 * This is a translator.  given an input address, this will convert it into a
 * function and offset.  Unless overridden, it will automatically determine
 * tranlations using the following methods, in order of priority:
 * 1) /usr/lib/debug/<kernel version> using libbfd
 * 2) /proc/kallsyms
 */
#include <stdlib.h>

#include "lookup.h"

struct lookup_methods {
	int (*lookup_init)(void);
};


static int bfd_init(void)
{
	return 0;
}

static int kas_init(void)
{
	return 0;
}

static struct lookup_methods bfd_methods = {
	bfd_init
};

static struct lookup_methods kallsym_methods = {
	kas_init
};

static struct lookup_methods *methods = NULL;

int init_lookup(lookup_init_method_t method)
{
	int rc;
	switch (method) {
	case METHOD_AUTO:
		methods = &bfd_methods;
		if (methods->lookup_init() == 0)
			return 0;
		methods = &kallsym_methods;
		if (methods->lookup_init() == 0)
			return 0;
		methods = NULL;
		return -1;
	case METHOD_DEBUGINFO:
		methods = &bfd_methods;
		break;
	case METHOD_KALSYMS:
		methods = &kallsym_methods;
		break;
	}

	rc = methods->lookup_init();
	if (rc < 0)
		methods = NULL;
	return rc;
}
