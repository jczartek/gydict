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
test_history_append_to_history (void)
{
  GyDictHistory *h = NULL;

  h = gy_dict_history_new ();

  gy_dict_history_append (h, 0);
  g_assert (gy_dict_history_size (h) == 1);

  gy_dict_history_append (h, 1);
  g_assert (gy_dict_history_size (h) == 2);

  gy_dict_history_append (h, 2);
  g_assert (gy_dict_history_size (h) == 3);

  gy_dict_history_append (h, 3);
  g_assert (gy_dict_history_size (h) == 4);

  g_object_unref (h);
}

static void
test_history_empty (void)
{
  GyDictHistory *h = NULL;
  gboolean can_go_back,
           can_go_next,
           is_empty;
  can_go_back = can_go_next = is_empty = FALSE;

  h = gy_dict_history_new ();

  g_object_get (h, "can-go-back", &can_go_back,
                   "can-go-next", &can_go_next,
                   "is-empty", &is_empty, NULL);

  g_assert_false (can_go_back || can_go_next || !is_empty);

  gy_dict_history_set_state (h);

  g_object_get (h, "can-go-back", &can_go_back,
                   "can-go-next", &can_go_next, NULL);

  g_assert_false (can_go_back || can_go_next);

  g_assert (gy_dict_history_size (h) == 0);

  gy_dict_history_reset_state (h);

  g_object_get (h, "can-go-back", &can_go_back,
                   "can-go-next", &can_go_next, NULL);

  g_assert_false (can_go_back || can_go_next);

  g_object_unref (h);
}

gint
main (gint   argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);
  g_test_add_func ("/test/history/append/to/history", test_history_append_to_history);
  g_test_add_func ("/test/history/empty/history", test_history_empty);
  g_test_run ();
  return 0;
}
