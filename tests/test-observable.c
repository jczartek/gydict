/* test-observable.c
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
#include "test-observer.h"
#include <g-pattern.h>

static void
test_adding_deleting_observers (void)
{
  TestObserver *ore_1 = test_observer_new ();
  TestObserver *ore_2 = test_observer_new ();
  GObservable *o = g_observable_new (NULL);

  g_observable_add_observer (o, G_OBSERVER (ore_1));
  g_assert_cmpint (1, ==, g_observable_count_observers (o));
  g_assert_cmpint (2, ==, G_OBJECT(ore_1)->ref_count);
  g_observable_add_observer (o, G_OBSERVER (ore_2));
  g_assert_cmpint (2, ==, g_observable_count_observers (o));
  g_assert_cmpint (2, ==, G_OBJECT(ore_1)->ref_count);
  g_observable_delete_observer (o, G_OBSERVER (ore_2));
  g_observable_delete_observer (o, G_OBSERVER (ore_1));
  g_assert_cmpint (0, ==, g_observable_count_observers (o));
  g_assert_cmpint (1, ==, G_OBJECT(ore_1)->ref_count);
  g_assert_cmpint (1, ==, G_OBJECT(ore_2)->ref_count);

  g_observable_add_observer (o, G_OBSERVER (ore_1));
  g_observable_add_observer (o, G_OBSERVER (ore_2));
  g_observable_delete_all_observers (o);
  g_assert_cmpint (0, ==, g_observable_count_observers (o));
  g_assert_cmpint (1, ==, G_OBJECT(ore_1)->ref_count);
  g_assert_cmpint (1, ==, G_OBJECT(ore_2)->ref_count);

  g_observable_add_observer (o, G_OBSERVER (ore_1));
  g_observable_add_observer (o, G_OBSERVER (ore_1));
  g_assert_cmpint (1, ==, g_observable_count_observers (o));
  g_observable_delete_observer (o, G_OBSERVER (ore_1));

  g_object_unref (G_OBJECT (ore_1));
  g_object_unref (G_OBJECT (ore_2));
}

static void
test_dispatching_argument (void)
{
  TestObserver *ore_1 = test_observer_new ();
  GObservable *o = g_observable_new (NULL);

  g_observable_add_observer (o, G_OBSERVER (ore_1));

  g_observable_dispatch_boolean (o, TRUE);
  g_observable_dispatch_char    (o, 'A');
  g_observable_dispatch_int     (o, -1);
  g_observable_dispatch_long    (o, 1);
  g_observable_dispatch_float   (o, 1.0);
  g_observable_dispatch_double  (o, 1.0);
  g_observable_dispatch_string  (o, "GOBJECT");
  g_observable_dispatch_pointer (o, GUINT_TO_POINTER(1));
  g_observable_dispatch_object  (o, G_OBJECT(ore_1));

  g_observable_delete_observer (o, G_OBSERVER (ore_1));

  g_object_unref (G_OBJECT (o));
  g_object_unref (G_OBJECT (ore_1));
}

gint
main (gint   argc,
      gchar *argv[])
{
  g_test_init(&argc, &argv, NULL);
  g_test_add_func ("/test/adding/deleting/observers", test_adding_deleting_observers);
  g_test_add_func ("/test/dispatching/arguments", test_dispatching_argument);
  return g_test_run ();
}
