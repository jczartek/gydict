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

static
void test_history_with_one_element (void)
{
  GyDictHistory *history = NULL;
  gboolean is_beginning = FALSE;
  gboolean is_end = FALSE;
  gboolean is_empty = FALSE;
  gconstpointer data = NULL;

  history = gy_dict_history_new ();

  data = gy_dict_history_next (history);
  g_assert_null (data);

  data = gy_dict_history_prev (history);
  g_assert_null (data);

  g_object_get (history, "is-empty", &is_empty, NULL);
  g_assert_true (is_empty);

  g_object_get (history, "is-beginning", &is_beginning, "is-end", &is_end, NULL);
  g_assert (is_beginning && is_end);

  gy_dict_history_append (history, "xxxfff");
  g_assert (gy_dict_history_size (history) == 1);

  g_object_get (history, "is-empty", &is_empty, NULL);
  g_assert_false (is_empty);

  g_object_get (history, "is-beginning", &is_beginning, "is-end", &is_end, NULL);
  g_assert_false (is_beginning);
  g_assert_true  (is_end);

  data = gy_dict_history_prev (history);
  g_assert_cmpstr(data, ==, "xxxfff");
  g_object_get (history, "is-beginning", &is_beginning, "is-end", &is_end, NULL);
  g_assert_false (is_beginning);
  g_assert_true  (is_end);

  /* Once again */
  data = gy_dict_history_prev (history);
  g_assert_cmpstr(data, ==, "xxxfff");
  g_object_get (history, "is-beginning", &is_beginning, "is-end", &is_end, NULL);
  g_assert_false (is_beginning);
  g_assert_true  (is_end);

  data = gy_dict_history_next (history);

  g_object_unref (history);
}

static gchar *words[] = {
  "make",
  "get",
  "set",
  "history",
  "new",
  "test",
  "come",
  "write",
  "read",
  "in",
  "on",
  "end",
  "finish",
  "start",
  "lose",
  "the",
  "same",
  "some",
  "any",
  "most",
  "more",
  "devote",
  NULL
};

static
void test_history_with_some_elements (void)
{
  GyDictHistory *history = NULL;
  gboolean is_beginning = FALSE;
  gboolean is_end = FALSE;
  const gchar *data = NULL;

  history = gy_dict_history_new ();

  for (gint i = 0; words[i] != NULL; i++)
    gy_dict_history_append (history, words[i]);

  g_assert (gy_dict_history_size (history) == 22);

  g_object_get (history, "is-beginning", &is_beginning, "is-end", &is_end, NULL);
  g_assert_false (is_beginning);
  g_assert_true  (is_end);

  for (gint i = G_N_ELEMENTS (words) - 2; i >= 0; i--)
    {
       data = gy_dict_history_prev (history);
       g_object_get (history, "is-beginning", &is_beginning, NULL);

      if (is_beginning == FALSE) g_assert_cmpstr(words[i], ==, data);
      else break;
    }
  g_assert_null (gy_dict_history_prev (history));
  g_object_unref (history);
}

gint
main (gint   argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);
  g_test_add_func ("/test/history/with/one/element", test_history_with_one_element);
  g_test_add_func ("/test/history/with/some/elements", test_history_with_some_elements);
  g_test_run ();
  return 0;
}
