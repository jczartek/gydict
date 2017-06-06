/* test-history.c
 *
 * Copyright (C) 2017 Jakub Czartek <kuba@linux.pl>
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

#include <gydict.h>

static void
test_history_empty (void)
{
  GyDictHistory *h = NULL;
  const gchar *data = NULL;
  gboolean is_beginning,
           is_end,
           is_empty;
  is_beginning = is_end = is_empty = FALSE;

  h = gy_dict_history_new ();

  g_object_get (h, "is-beginning", &is_beginning, "is-end", &is_end, "is-empty", &is_empty, NULL);

  g_assert (is_beginning && is_end && is_empty);

  g_assert_null (gy_dict_history_next (h));
  g_assert_null (gy_dict_history_prev (h));

  gy_dict_history_set_state (h);

  g_object_get (h, "is-beginning", &is_beginning, "is-end", &is_end, NULL);

  g_assert (is_beginning && is_end);

  g_assert (gy_dict_history_size (h) == 0);

  gy_dict_history_reset_state (h);

  g_object_get (h, "is-beginning", &is_beginning, "is-end", &is_end, NULL);

  g_assert (is_beginning && is_end);

  g_object_unref (h);
}

static
void test_history_one_element (void)
{
  GyDictHistory *h = NULL;
  gboolean is_beginning = FALSE;
  gboolean is_end = FALSE;
  gboolean is_empty = FALSE;
  gconstpointer data = NULL;

  h = gy_dict_history_new ();

  gy_dict_history_append (h, "A");
  g_assert (gy_dict_history_size (h) == 1);

  g_object_get (h, "is-empty", &is_empty, NULL);
  g_assert_false (is_empty);

  g_object_get (h, "is-beginning", &is_beginning, "is-end", &is_end, NULL);
  g_assert_false (is_beginning);
  g_assert_true  (is_end);

  data = gy_dict_history_prev (h);
  g_assert_cmpstr(data, ==, "A");
  g_object_get (h, "is-beginning", &is_beginning, "is-end", &is_end, NULL);
  g_assert_true (is_beginning);
  g_assert_true (is_end);

  /* Once again */
  data = gy_dict_history_prev (h);
  g_assert_null (data);
  g_object_get (h, "is-beginning", &is_beginning, "is-end", &is_end, NULL);
  g_assert_true (is_beginning);
  g_assert_true  (is_end);

  data = gy_dict_history_next (h);
  g_assert_null (data);
  g_object_get (h, "is-beginning", &is_beginning, "is-end", &is_end, NULL);
  g_assert_true (is_beginning);
  g_assert_true  (is_end);

  /* Reset */

  gy_dict_history_reset_state (h);

  g_object_get (h, "is-beginning", &is_beginning, "is-end", &is_end, NULL);
  g_assert_false (is_beginning);
  g_assert_true  (is_end);

  data = gy_dict_history_prev (h);
  g_assert_cmpstr(data, ==, "A");
  g_object_get (h, "is-beginning", &is_beginning, "is-end", &is_end, NULL);
  g_assert_true (is_beginning);
  g_assert_true (is_end);

  /* Once again */
  data = gy_dict_history_prev (h);
  g_assert_null (data);
  g_object_get (h, "is-beginning", &is_beginning, "is-end", &is_end, NULL);
  g_assert_true (is_beginning);
  g_assert_true  (is_end);

  data = gy_dict_history_next (h);
  g_assert_null (data);
  g_object_get (h, "is-beginning", &is_beginning, "is-end", &is_end, NULL);
  g_assert_true (is_beginning);
  g_assert_true  (is_end);

  g_object_unref (h);
}

gint
main (gint   argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);
  g_test_add_func ("/test/history/empty/history", test_history_empty);
  g_test_add_func ("/test/history/empty/history/one/element", test_history_one_element);
  g_test_run ();
  return 0;
}
