/* g-observable.c
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

#include "g-observer.h"
#include "g-observable.h"

struct _GObservable
{
  GObject parent_instance;

  GPtrArray *observers;
};

G_DEFINE_TYPE (GObservable, g_observable, G_TYPE_OBJECT)

enum {
  PROP_0,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
g_observable_finalize (GObject *object)
{
  GObservable *self = (GObservable *)object;

  G_OBJECT_CLASS (g_observable_parent_class)->finalize (object);
}

static void
g_observable_get_property (GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  GObservable *self = G_OBSERVABLE (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
g_observable_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  GObservable *self = G_OBSERVABLE (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
g_observable_class_init (GObservableClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = g_observable_finalize;
  object_class->get_property = g_observable_get_property;
  object_class->set_property = g_observable_set_property;
}

static void
g_observable_init (GObservable *self)
{
  self->observers = g_ptr_array_new_with_free_func (g_object_unref);
}

GObservable *
g_observable_new (void)
{
  return g_object_new (G_TYPE_OBSERVABLE, NULL);
}

void
g_observable_add_observer (GObservable *self,
                           GObserver   *observer)
{
  g_return_if_fail (G_IS_OBSERVABLE (self));
  g_return_if_fail (G_IS_OBSERVER (observer));

  g_ptr_array_add (self->observers, (gpointer) g_object_ref (G_OBJECT (observer)));
}

void
g_observable_delete_observer (GObservable *self,
                              GObserver   *observer)
{
  g_return_if_fail (G_IS_OBSERVABLE (self));
  g_return_if_fail (G_IS_OBSERVER (observer));

  g_ptr_array_remove (self->observers, (gpointer) observer);
}

void
g_observable_delete_all_observers (GObservable *self)
{
  g_return_if_fail (G_IS_OBSERVABLE (self));

  g_ptr_array_remove_range (self->observers, 0, self->observers->len - 1);
}

void
g_observable_notify_observers (GObservable *self)
{
}

gint
g_observable_count_observers (GObservable *self)
{
  return self->observers->len;
}
