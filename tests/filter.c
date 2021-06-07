/*
 * Copyright 2021 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
*/

#include <errno.h>
#include <locale.h>
#include <glib.h>
#include <stdlib.h>

#include "usbredirfilter.h"

static void
test_verify_rules_bad(void)
{
    int i;
    const char * const filters[] = {
        /* Check upper and lower limits */
        "0x100,-1,-1,-1,0", /* class */
        "-2,-1,-1,-1,0",
        "0x03,,0x10000-1,-1,0", /* vendor */
        "0x03,-2,-1,-1,0",
        "0x03,-1,0x10000-1,,0", /* product */
        "0x03,-1,-2,-1,0",
        "0x03,-1,-1,0x10000,0", /* bcd */
        "0x03,-1,-1,-2,0",
        /* Extra argument */
        "0x03,-1,-1,-1,0,1",
        /* Missing argument */
        "0x03,-1,-1,-1",
        /* Missing value in argument */
        "0x03,-1,-1,,-1",
        /* Bad char as value in argument (1) */
        "0x03,-1,-1,a,-1",
        /* Bad char as value in argument (2) */
        "0x03,-1,-1,#,-1",
        /* Bad char as value in argument (3) */
        "0x03,-1,-1, ,-1",
        /* Invalid token_sep */
        "0x03;-1;-1;-1;0",
        /* Invalid rule_sep */
        "0x03,-1,-1,-1,0;-1,-1,-1,-1,1",
        /* Bad rule in many */
        "0x03,-1,-1,-1,0|3|-1,-1,-1,-1,1",
    };

    for (i = 0; i < G_N_ELEMENTS (filters); i++) {
        int retval, count = 0;
        struct usbredirfilter_rule *rules = NULL;

        retval = usbredirfilter_string_to_rules(filters[i], ",", "|", &rules, &count);
        g_assert_cmpint(retval, ==, -EINVAL);
        g_assert_null(rules);
    }
}

static void
test_verify_rules_good(void)
{
    int i;
    static const struct test {
        int nrules;
        const char *filter;
        const char *expected_serialized;
        const char *token_sep;
        const char *rule_sep;
    } test_data[] = {
        { 0, "" },
        { 0, "|||", "" },
        { 1, "0x03,-1,-1,-1,0" },
        { 2, "0x03,-1,-1,-1,0|-1,-1,-1,-1,1" },
        /* Ignores trailing rule_sep */
        { 2,  "|0x03,-1,-1,-1,0|-1,-1,-1,-1,1|", "0x03,-1,-1,-1,0|-1,-1,-1,-1,1" },
        /* Ignores empty rules */
        { 2,  "0x03,-1,-1,-1,0|||-1,-1,-1,-1,1", "0x03,-1,-1,-1,0|-1,-1,-1,-1,1" },
        /* Several trailing rule_sep and empty rules */
        { 2,  "||||0x03,-1,-1,-1,0|||-1,-1,-1,-1,1||||", "0x03,-1,-1,-1,0|-1,-1,-1,-1,1" },
        /* Change rule separator using multiple characters */
        { 1, "0x03,-1,-1,-1,0", NULL, ",;", " \t\n" },
        /* Mix of different separators */
        { 1, "\t 0x03,-1;-1;-1,0\n\n", "0x03,-1,-1,-1,0" },
        /* Multiple rules, separators not the first charater */
        { 2, "\n\t0x03;-1,-1,-1,0\n\n-1,-1,-1;-1;1", "0x03,-1,-1,-1,0 -1,-1,-1,-1,1" },
    };
    const char *token_sep = ",";
    const char *rule_sep = "|";

    for (i = 0; i < G_N_ELEMENTS (test_data); i++) {
        const struct test *const data = &test_data[i];
        int retval, count = 0;
        char *filter;
        struct usbredirfilter_rule *rules = NULL;
        const char *const expected =
            data->expected_serialized ? data->expected_serialized : data->filter;

        if (data->token_sep) {
            token_sep = data->token_sep;
        }
        if (data->rule_sep) {
            rule_sep = data->rule_sep;
        }

        retval = usbredirfilter_string_to_rules(data->filter, token_sep, rule_sep, &rules, &count);
        g_assert_cmpint(retval, ==, 0);
        g_assert_cmpint(count, ==, data->nrules);


        filter = usbredirfilter_rules_to_string(rules, count, token_sep, rule_sep);
        g_assert_nonnull(filter);
        g_assert_cmpstr(expected, ==, filter);
        free(filter);
        free(rules);
    }
}

int
main(int argc, char **argv)
{
    setlocale(LC_ALL, "");
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/filter/rules/good", test_verify_rules_good);
    g_test_add_func("/filter/rules/bad", test_verify_rules_bad);

    return g_test_run();
}
