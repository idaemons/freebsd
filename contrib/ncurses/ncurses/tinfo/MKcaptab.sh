#!/bin/sh
##############################################################################
# Copyright (c) 2007-2009,2010 Free Software Foundation, Inc.                #
#                                                                            #
# Permission is hereby granted, free of charge, to any person obtaining a    #
# copy of this software and associated documentation files (the "Software"), #
# to deal in the Software without restriction, including without limitation  #
# the rights to use, copy, modify, merge, publish, distribute, distribute    #
# with modifications, sublicense, and/or sell copies of the Software, and to #
# permit persons to whom the Software is furnished to do so, subject to the  #
# following conditions:                                                      #
#                                                                            #
# The above copyright notice and this permission notice shall be included in #
# all copies or substantial portions of the Software.                        #
#                                                                            #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR #
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   #
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    #
# THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      #
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    #
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        #
# DEALINGS IN THE SOFTWARE.                                                  #
#                                                                            #
# Except as contained in this notice, the name(s) of the above copyright     #
# holders shall not be used in advertising or otherwise to promote the sale, #
# use or other dealings in this Software without prior written               #
# authorization.                                                             #
##############################################################################
# $Id: MKcaptab.sh,v 1.13 2010/12/25 23:43:58 tom Exp $
AWK=${1-awk}
OPT1=${2-0}
OPT2=${3-tinfo/MKcaptab.awk}
DATA=${4-../include/Caps}

cat <<EOF
/*
 * generated by $0
 */

EOF

cat <<'EOF'
/*
 *	comp_captab.c -- The names of the capabilities indexed via a hash
 *		         table for the compiler.
 *
 */

#include <curses.priv.h>
#include <tic.h>
#include <hashsize.h>

EOF

./make_hash 1 info $OPT1 <$DATA
./make_hash 3 cap  $OPT1 <$DATA

$AWK -f $OPT2 bigstrings=$OPT1 tablename=capalias <$DATA

$AWK -f $OPT2 bigstrings=$OPT1 tablename=infoalias <$DATA

cat <<EOF

#if $OPT1
static void
next_string(const char *strings, unsigned *offset)
{
    *offset += (unsigned) strlen(strings + *offset) + 1;
}

static const struct name_table_entry *
_nc_build_names(struct name_table_entry **actual,
		const name_table_data *source,
		const char *strings)
{
    if (*actual == 0) {
	*actual = typeCalloc(struct name_table_entry, CAPTABSIZE);
	if (*actual != 0) {
	    unsigned n;
	    unsigned len = 0;
	    for (n = 0; n < CAPTABSIZE; ++n) {
		(*actual)[n].nte_name = strings + len;
		(*actual)[n].nte_type = source[n].nte_type;
		(*actual)[n].nte_index = source[n].nte_index;
		(*actual)[n].nte_link = source[n].nte_link;
		next_string(strings, &len);
	    }
	}
    }
    return *actual;
}

#define add_alias(field) \\
	if (source[n].field >= 0) { \\
		(*actual)[n].field = strings + source[n].field; \\
	}

static const struct alias *
_nc_build_alias(struct alias **actual,
		const alias_table_data *source,
		const char *strings,
		unsigned tablesize)
{
    if (*actual == 0) {
	*actual = typeCalloc(struct alias, tablesize + 1);
	if (*actual != 0) {
	    unsigned n;
	    for (n = 0; n < tablesize; ++n) {
		add_alias(from);
		add_alias(to);
		add_alias(source);
	    }
	}
    }
    return *actual;
}

#define build_names(root) _nc_build_names(&_nc_##root##_table, \\
					  root##_names_data, \\
					  root##_names_text)
#define build_alias(root) _nc_build_alias(&_nc_##root##alias_table, \\
					  root##alias_data, \\
					  root##alias_text, \\
					  SIZEOF(root##alias_data))
#else
#define build_names(root) _nc_ ## root ## _table
#define build_alias(root) _nc_ ## root ## alias_table
#endif

NCURSES_EXPORT(const struct name_table_entry *) _nc_get_table (bool termcap)
{
    return termcap ? build_names(cap) : build_names(info) ;
}

/* entrypoint used by tack (do not alter) */
NCURSES_EXPORT(const HashValue *) _nc_get_hash_table (bool termcap)
{
    return termcap ? _nc_cap_hash_table: _nc_info_hash_table ;
}

NCURSES_EXPORT(const struct alias *) _nc_get_alias_table (bool termcap)
{
    return termcap ? build_alias(cap) : build_alias(info) ;
}

static HashValue
info_hash(const char *string)
{
    long sum = 0;

    DEBUG(9, ("hashing %s", string));
    while (*string) {
	sum += (long) (*string + (*(string + 1) << 8));
	string++;
    }

    DEBUG(9, ("sum is %ld", sum));
    return (HashValue) (sum % HASHTABSIZE);
}

#define TCAP_LEN 2		/* only 1- or 2-character names are used */

static HashValue
tcap_hash(const char *string)
{
    char temp[TCAP_LEN + 1];
    int limit = 0;

    while (*string) {
	temp[limit++] = *string++;
	if (limit >= TCAP_LEN)
	    break;
    }
    temp[limit] = '\0';
    return info_hash(temp);
}

static int
compare_tcap_names(const char *a, const char *b)
{
    return !strncmp(a, b, TCAP_LEN);
}

static int
compare_info_names(const char *a, const char *b)
{
    return !strcmp(a, b);
}

static const HashData hash_data[2] = {
    { HASHTABSIZE, _nc_info_hash_table, info_hash, compare_info_names },
    { HASHTABSIZE, _nc_cap_hash_table, tcap_hash, compare_tcap_names }
};

NCURSES_EXPORT(const HashData *) _nc_get_hash_info (bool termcap)
{
    return &hash_data[(termcap != FALSE)];
}

#if NO_LEAKS
NCURSES_EXPORT(void) _nc_comp_captab_leaks(void)
{
#if $OPT1
    FreeIfNeeded(_nc_cap_table);
    FreeIfNeeded(_nc_info_table);
    FreeIfNeeded(_nc_capalias_table);
    FreeIfNeeded(_nc_infoalias_table);
#endif
}
#endif /* NO_LEAKS */
EOF
