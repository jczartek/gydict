/* gy-dict-history.c
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

#include "gy-dict-history.h"

static GList Nil;

struct _GyDictHistory
{
  GObject  __parent__;
  GList   *h;
  GList   *iter;

  guint   is_beginning : 1;
  guint   is_end       : 1;
  guint   is_empty     : 1;
};

G_DEFINE_TYPE (GyDictHistory, gy_dict_history, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_IS_BEGINNING,
  PROP_IS_END,
  PROP_IS_EMPTY,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
gy_dict_history_finalize (GObject *object)
{
  GyDictHistory *self = (GyDictHistory *) object;

  if (!self->is_empty)
    {
      Nil.prev->next = NULL;
      g_list_free_full (self->h, g_free);
    }
  self->h = NULL;
  self->iter = NULL;

  G_OBJECT_CLASS (gy_dict_history_parent_class)->finalize (object);
}

static void
gy_dict_history_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  GyDictHistory *self = GY_DICT_HISTORY (object);

  switch (prop_id)
    {
    case PROP_IS_BEGINNING:
      g_value_set_boolean (value, self->is_beginning);
      break;
    case PROP_IS_END:
      g_value_set_boolean (value, self->is_end);
      break;
    case PROP_IS_EMPTY:
      g_value_set_boolean (value, self->is_empty);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_dict_history_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  GyDictHistory *self = GY_DICT_HISTORY (object);

  switch (prop_id)
    {
    case PROP_IS_BEGINNING:
      self->is_beginning = g_value_get_boolean (value);
      break;
    case PROP_IS_END:
      self->is_end = g_value_get_boolean (value);
      break;
    case PROP_IS_EMPTY:
      self->is_empty = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_dict_history_class_init (GyDictHistoryClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gy_dict_history_finalize;
  object_class->get_property = gy_dict_history_get_property;
  object_class->set_property = gy_dict_history_set_property;

  properties[PROP_IS_BEGINNING] =
    g_param_spec_boolean ("is-beginning",
                          "IsBeginning",
                          "This property indicates if the iter of the history is at the beginning of the history.",
                          TRUE, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  properties[PROP_IS_END] =
    g_param_spec_boolean ("is-end",
                          "IsEnd",
                          "This property indicates if the iter of this history is at the end of the history.",
                          TRUE, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  properties[PROP_IS_EMPTY] =
    g_param_spec_boolean ("is-empty",
                          "IsEmpty",
                          "This property indicates if the list is empty.",
                          TRUE, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
gy_dict_history_init (GyDictHistory *self)
{
  self->h = &Nil;
  self->iter = &Nil;
  self->is_beginning = TRUE;
  self->is_end = TRUE;
  self->is_empty = TRUE;
}

GyDictHistory *
gy_dict_history_new (void)
{
  return g_object_new (GY_TYPE_DICT_HISTORY, NULL);
}

void
gy_dict_history_append (GyDictHistory *self,
                        const gchar   *str)
{
  GList *l = NULL;

  g_return_if_fail (GY_IS_DICT_HISTORY (self));
  g_return_if_fail (str != NULL && g_utf8_strlen (str, -1));
  g_return_if_fail (g_utf8_validate (str, -1, NULL));

  l = g_list_find_custom (self->h, str, (GCompareFunc) g_strcmp0);

  if (l) return;

  self->h = g_list_insert_before (self->h, &Nil, g_strdup (str));
  self->iter = &Nil;

  if (self->is_empty)
    g_object_set (self, "is-empty", FALSE, NULL);

  gy_dict_history_set_state (self);

}

void
gy_dict_history_set_state (GyDictHistory *self)
{
  g_return_if_fail (GY_IS_DICT_HISTORY (self));

  /* The history has not any elements. */
  if (self->is_empty)
    {
      g_object_set (self, "is-beginning", TRUE, "is-end", TRUE, NULL);
      return;
    }

  /* The history has at least one element and its iter is at the end of one. */
  if (self->iter->prev != NULL && self->iter == &Nil)
    {
      g_object_set (self, "is-beginning", FALSE, "is-end", TRUE, NULL);
      return;
    }

  /* The history has some elements and its iter is in the midlle of one. */
  if (self->iter->prev != NULL && self->iter->next != NULL && self->iter->next != &Nil)
    {
      g_object_set (self, "is-beginning", FALSE, "is-end", FALSE, NULL);
      return;
    }

  /* The history has some elements and its iter is at the beginning of one. */
  if (self->iter->prev == NULL && self->iter->next != NULL)
    {
      g_object_set(self, "is-beginning", TRUE, "is-end", FALSE, NULL);
      return;
    }

  g_assert_not_reached();
}

gconstpointer
gy_dict_history_next (GyDictHistory *self)
{
  gconstpointer data = NULL;
  g_return_val_if_fail (GY_IS_DICT_HISTORY (self), NULL);

  /* the empty history or the iter is at the end */
  if (self->iter->next == NULL)
    {
      return NULL;
    }

  if (self->iter->next != &Nil)
    {
      self->iter = self->iter->next;
      data = self->iter->data;

      if (self->iter->next == &Nil)
        {
          self->iter = &Nil;
        }
    }
  gy_dict_history_set_state (self);

  return data;
}

gconstpointer
gy_dict_history_prev (GyDictHistory *self)
{
  g_return_val_if_fail (GY_IS_DICT_HISTORY (self), NULL);

  /* the empty history or the iter is at the beginning.*/
  if (self->iter->prev == NULL)
    {
      return NULL;
    }

  /* The iter is not moving to a previous element. */
  if (Nil.prev->prev == NULL)
    {
      return self->iter->prev->data;
    }

  if (self->iter->prev != NULL)
    {
      self->iter = self->iter->prev;
      gy_dict_history_set_state (self);
      return self->iter->data;
    }

  return NULL;
}
