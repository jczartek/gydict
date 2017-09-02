/* test-observer.c
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

struct _TestObserver
{
  GObject parent_instance;
};

static void test_observer_interface_init (GObserverInterface *iface);

G_DEFINE_TYPE_WITH_CODE (TestObserver, test_observer, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (G_TYPE_OBSERVER,
                                                test_observer_interface_init))

static void
test_observer_class_init (TestObserverClass *klass)
{
}

static void
test_observer_init (TestObserver *self)
{
}

static void
test_observer_update (GObserver    *observer,
                      GObject      *observable,
                      const GValue *arg)
{

  g_assert_null (observable);

  switch (G_VALUE_TYPE(arg))
    {
    case G_TYPE_BOOLEAN:
      g_assert_true (g_value_get_boolean (arg));
      break;
    case G_TYPE_CHAR:
      g_assert (g_value_get_schar (arg) == 'A');
      break;
    case G_TYPE_INT:
      g_assert_cmpint (-1, ==, g_value_get_int (arg));
      break;
    case G_TYPE_LONG:
      g_assert (1 == g_value_get_long (arg));
      break;
    case G_TYPE_FLOAT:
      g_assert (1.0 == g_value_get_float (arg));
      break;
    case G_TYPE_DOUBLE:
      g_assert (1.0 == g_value_get_double (arg));
      break;
    case G_TYPE_STRING:
      g_assert_cmpstr ("GOBJECT", ==, g_value_get_string (arg));
      break;
    case G_TYPE_POINTER:
      g_assert (1 == GPOINTER_TO_UINT(g_value_get_pointer (arg)));
      break;
    case G_TYPE_OBJECT:
      g_assert (observer == (GObserver *) g_value_get_object (arg));
      break;
    default:
      g_assert_not_reached();
    }
}

static void
test_observer_interface_init (GObserverInterface *iface)
{
  iface->update = test_observer_update;
}

TestObserver *
test_observer_new (void)
{
  return TEST_OBSERVER (g_object_new (TEST_TYPE_OBSERVER, NULL));
}
