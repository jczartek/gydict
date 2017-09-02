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

  gpointer   owner;
  GPtrArray *observers;
};

G_DEFINE_TYPE (GObservable, g_observable, G_TYPE_OBJECT)

G_LOCK_DEFINE_STATIC (lock);

enum {
  PROP_0,
  PROP_ARG_BOOLEAN,
  PROP_ARG_CHAR,
  PROP_ARG_INT,
  PROP_ARG_LONG,
  PROP_ARG_FLOAT,
  PROP_ARG_DOUBLE,
  PROP_ARG_STRING,
  PROP_ARG_POINTER,
  PROP_ARG_OBJECT,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
g_observable_finalize (GObject *object)
{
  GObservable *self = G_OBSERVABLE (object);

  if (self->owner)
    {
      g_object_remove_weak_pointer (self->owner, &self->owner);
      self->owner = NULL;
    }

  if (self->observers)
    {
      g_ptr_array_free (self->observers, TRUE);
      self->observers = NULL;
    }

  G_OBJECT_CLASS (g_observable_parent_class)->finalize (object);
}

static void
g_observable_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  GObservable *self = G_OBSERVABLE (object);

  if (prop_id >= PROP_ARG_BOOLEAN && prop_id <= PROP_ARG_OBJECT)
    g_observable_notify_observers (self, value);
  else
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);

}

static void
g_observable_class_init (GObservableClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = g_observable_finalize;
  object_class->set_property = g_observable_set_property;

  properties[PROP_ARG_BOOLEAN] =
    g_param_spec_boolean ("arg-boolean", "ArgBoolean",
                          "This parameter will be passed as argument to a GObserver's update function.",
                          FALSE, G_PARAM_WRITABLE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_ARG_CHAR] =
    g_param_spec_char ("arg-char", "ArgChar",
                       "This parameter will be passed as an argument to a GObserver's update function.",
                       -128, 127, 0, G_PARAM_WRITABLE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_ARG_INT] =
    g_param_spec_int ("arg-int", "ArgInt",
                      "This parameter will be passed as an argument to a GObserver's update function.",
                      G_MININT, G_MAXINT, 0, G_PARAM_WRITABLE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_ARG_LONG] =
    g_param_spec_long ("arg-long", "ArgLong",
                       "This parameter will be passed as an argument to a GObserver's update function.",
                       G_MINLONG, G_MAXLONG, 0, G_PARAM_WRITABLE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_ARG_FLOAT] =
    g_param_spec_float ("arg-float", "ArgFloat",
                        "This parameter will be passed as an argument to a GObserver's update function.",
                        -G_MINFLOAT, G_MAXFLOAT, 0.0, G_PARAM_WRITABLE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_ARG_DOUBLE] =
    g_param_spec_double ("arg-double", "ArgDouble",
                         "This parameter will be passed as an argument to a GObserver's update function.",
                         -G_MINDOUBLE, G_MAXDOUBLE, 0.0, G_PARAM_WRITABLE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_ARG_STRING] =
    g_param_spec_string ("arg-string", "ArgString",
                         "This parameter will be passed as an argument to a GObserver's update function.",
                         NULL, G_PARAM_WRITABLE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_ARG_POINTER] =
    g_param_spec_pointer ("arg-pointer", "ArgPointer",
                          "This parameter will be passed as an argument to a GObserver's update function.",
                          G_PARAM_WRITABLE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_ARG_OBJECT] =
    g_param_spec_object ("arg-object", "ArgObject",
                         "This parameter will be passed as an argument to a GObserver's update function.",
                         G_TYPE_OBJECT, G_PARAM_WRITABLE | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
g_observable_init (GObservable *self)
{
  self->observers = g_ptr_array_new_with_free_func (g_object_unref);
  self->owner = NULL;
}

GObservable *
g_observable_new (GObject *owner)
{
  GObservable *self;

  self = g_object_new (G_TYPE_OBSERVABLE, NULL);

  if (self && G_IS_OBJECT (owner))
    g_object_add_weak_pointer (owner, &self->owner);

  return self;
}

void
g_observable_add_observer (GObservable *self,
                           GObserver   *observer)
{
  gboolean existed = FALSE;
  g_return_if_fail (G_IS_OBSERVABLE (self));
  g_return_if_fail (G_IS_OBSERVER (observer));

  G_LOCK (lock);

  for (int i = 0; i < self->observers->len; i++)
    {
      GObserver *o = g_ptr_array_index (self->observers, i);
      if (o == observer)
        {
          existed = TRUE;
          break;
        }
    }

  if (!existed)
    g_ptr_array_add (self->observers, (gpointer) g_object_ref (G_OBJECT (observer)));

  G_UNLOCK (lock);
}

void
g_observable_delete_observer (GObservable *self,
                              GObserver   *observer)
{
  g_return_if_fail (G_IS_OBSERVABLE (self));
  g_return_if_fail (G_IS_OBSERVER (observer));

  G_LOCK (lock);
  g_ptr_array_remove (self->observers, (gpointer) observer);
  G_UNLOCK (lock);
}

void
g_observable_delete_all_observers (GObservable *self)
{
  g_return_if_fail (G_IS_OBSERVABLE (self));

  G_LOCK (lock);
  g_ptr_array_remove_range (self->observers, 0, self->observers->len);
  G_UNLOCK (lock);
}


void
g_observable_notify_observers (GObservable  *self,
                               const GValue *arg)
{
  g_return_if_fail (G_IS_OBSERVABLE (self));
  g_return_if_fail (self->observers->len > 0);

  GObject *real_observable = self->owner != NULL ? G_OBJECT (self->owner) : NULL;

  G_LOCK (lock);
  for (int i = 0; i < self->observers->len; i++)
    {
      GObserver *o = g_ptr_array_index (self->observers, i);
      g_observer_update (o, real_observable, arg);
    }
  G_UNLOCK (lock);

}

gint
g_observable_count_observers (GObservable *self)
{
  gint count = 0;

  G_LOCK (lock);
  count = self->observers->len;
  G_UNLOCK (lock);

  return count;
}

void
g_observable_dispatch_boolean (GObservable *self,
                               gboolean     arg)
{
  g_return_if_fail (G_IS_OBSERVABLE (self));

  g_object_set (G_OBJECT (self), "arg-boolean", arg, NULL);
}

void
g_observable_dispatch_char (GObservable *self,
                            gchar        arg)
{
  g_return_if_fail (G_IS_OBSERVABLE (self));

  g_object_set (G_OBJECT (self), "arg-char", arg, NULL);
}

void
g_observable_dispatch_int (GObservable *self,
                           gint         arg)
{
  g_return_if_fail (G_IS_OBSERVABLE (self));

  g_object_set (G_OBJECT (self), "arg-int", arg, NULL);
}

void
g_observable_dispatch_long (GObservable *self,
                            long         arg)
{
  g_return_if_fail (G_IS_OBSERVABLE (self));

  g_object_set (G_OBJECT (self), "arg-long", arg, NULL);
}

void
g_observable_dispatch_float (GObservable *self,
                             float        arg)
{
  g_return_if_fail (G_IS_OBSERVABLE (self));

  g_object_set (G_OBJECT (self), "arg-float", arg, NULL);
}

void
g_observable_dispatch_double (GObservable *self,
                              double       arg)
{
  g_return_if_fail (G_IS_OBSERVABLE (self));

  g_object_set (G_OBJECT (self), "arg-double", arg, NULL);
}

void
g_observable_dispatch_string (GObservable *self,
                              const gchar *arg)
{
  g_return_if_fail (G_IS_OBSERVABLE (self));

  g_object_set (G_OBJECT (self), "arg-string", arg, NULL);
}

void
g_observable_dispatch_pointer (GObservable *self,
                               gpointer     arg)
{
  g_return_if_fail (G_IS_OBSERVABLE (self));

  g_object_set (G_OBJECT (self), "arg-pointer", arg, NULL);
}

void
g_observable_dispatch_object (GObservable *self,
                              GObject     *arg)
{
  g_return_if_fail (G_IS_OBSERVABLE (self));

  g_object_set (G_OBJECT (self), "arg-object", arg, NULL);
}
