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


struct _GyDictHistory
{
  GObject  __parent__;
  GArray  *h;
  gint     iter;

  guint   can_go_back  : 1;
  guint   can_go_next  : 1;
  guint   is_empty     : 1;
};

G_DEFINE_TYPE (GyDictHistory, gy_dict_history, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_CAN_GO_BACK,
  PROP_CAN_G0_NEXT,
  PROP_IS_EMPTY,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
gy_dict_history_finalize (GObject *object)
{
  GyDictHistory *self = (GyDictHistory *) object;

  g_array_free (self->h, TRUE);

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
    case PROP_CAN_GO_BACK:
      g_value_set_boolean (value, self->can_go_back);
      break;
    case PROP_CAN_G0_NEXT:
      g_value_set_boolean (value, self->can_go_next);
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
    case PROP_CAN_GO_BACK:
      self->can_go_back = g_value_get_boolean (value);
      break;
    case PROP_CAN_G0_NEXT:
      self->can_go_next = g_value_get_boolean (value);
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

  properties[PROP_CAN_GO_BACK] =
    g_param_spec_boolean ("can-go-back",
                          "Can go back",
                          "This property indicates if the iter of the history is at the beginning of the history.",
                          TRUE, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  properties[PROP_CAN_G0_NEXT] =
    g_param_spec_boolean ("can-go-next",
                          "Can go next",
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
  self->h = g_array_new (FALSE, FALSE, sizeof (gint));
  self->iter = -1;
  self->can_go_back = FALSE;
  self->can_go_next = FALSE;
  self->is_empty = TRUE;
}

GyDictHistory *
gy_dict_history_new (void)
{
  return g_object_new (GY_TYPE_DICT_HISTORY, NULL);
}

void
gy_dict_history_append (GyDictHistory *self,
                        gint           row_number)
{
  g_return_if_fail (GY_IS_DICT_HISTORY (self));
  g_return_if_fail (row_number >= 0);

  g_array_append_val (self->h, row_number);
  self->iter = self->h->len;

  if (self->is_empty)
    g_object_set (self, "is-empty", FALSE, NULL);

  gy_dict_history_set_state (self);

}

void
gy_dict_history_set_state (GyDictHistory *self)
{
  g_return_if_fail (GY_IS_DICT_HISTORY (self));

  if (self->is_empty)
    return;
  else if (self->iter == -1)
    g_object_set (self, "can-go-back", FALSE, "can-go-next", TRUE, NULL);
  else if (self->iter == (self->h->len))
    g_object_set (self, "can-go-back", TRUE, "can-go-next", FALSE, NULL);
  else
    g_object_set (self, "can-go-back", TRUE, "can-go-next", TRUE, NULL);
}

gint
gy_dict_history_go_back (GyDictHistory *self)
{
  gint elem = -1;

  g_return_val_if_fail (GY_IS_DICT_HISTORY (self), -1);
  g_return_val_if_fail (!self->is_empty, -1);

  if (self->iter != -1)
    {
      if (self->iter == self->h->len)
        self->iter--;

      elem = g_array_index (self->h, gint, self->iter--);
      gy_dict_history_set_state (self);
    }

  return elem;
}

gint
gy_dict_history_go_next (GyDictHistory *self)
{
  gint elem = -1;

  g_return_val_if_fail (GY_IS_DICT_HISTORY (self), -1);
  g_return_val_if_fail (!self->is_empty, -1);

  if (self->iter != (self->h->len))
    {
      if (self->iter == -1)
        self->iter++;

      elem = g_array_index (self->h, gint, self->iter++);
      gy_dict_history_set_state (self);
    }

  return elem;
}

guint
gy_dict_history_size (GyDictHistory *self)
{
  g_return_val_if_fail (GY_IS_DICT_HISTORY (self), 0);

  return self->h->len;
}

void
gy_dict_history_reset_state (GyDictHistory *self)
{
  g_return_if_fail (GY_IS_DICT_HISTORY (self));

  if (!self->is_empty)
    self->iter = self->h->len;

  gy_dict_history_set_state (self);
}
