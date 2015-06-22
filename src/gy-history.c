/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#include <gtk/gtk.h>

#include "gy-history.h"
#include "gy-history-iterable.h"

static void          gy_history_next_item      (GyHistoryIterable *iterable);
static void          gy_history_previous_item  (GyHistoryIterable *iterable);
static gboolean      gy_history_is_begin       (GyHistoryIterable *iterable);
static gboolean      gy_history_is_end         (GyHistoryIterable *iterable);
static gconstpointer gy_history_get_item       (GyHistoryIterable *iterable);
static void          gy_history_interface_init (GyHistoryIterableInterface *iface);
static void history_get_property (GObject    *object,
	                          guint       prop_id,
				  GValue     *value,
				  GParamSpec *pspec);
static void history_set_property (GObject      *object,
	                          guint         prop_id,
				  const GValue *value,
				  GParamSpec   *pspec);
static void dispose (GObject *object);

struct _GyHistoryPrivate
{
  GList		*history;
  GyHistoryIter *iter;
  gboolean       is_enabled_action_next;
  gboolean       is_enabled_action_prev;
};

enum
{
  PROP_0,
  PROP_IS_ENABLED_ACTION_NEXT,
  PROP_IS_ENABLED_ACTION_PREV,
  N_PROPERTIES
};
static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

G_DEFINE_TYPE_WITH_CODE (GyHistory, gy_history, G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GY_HISTORY_TYPE_ITERABLE,
				                gy_history_interface_init));

#define GET_PRIVATE(instance) G_TYPE_INSTANCE_GET_PRIVATE \
    (instance, GY_TYPE_HISTORY, GyHistoryPrivate)

static void
gy_history_init (GyHistory *self)
{
  self->priv = GET_PRIVATE (self);

  self->priv->history = NULL;
  self->priv->iter = NULL;
}

static void
gy_history_class_init (GyHistoryClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->set_property = history_set_property;
    object_class->get_property = history_get_property;
    object_class->dispose = dispose;

    g_type_class_add_private (klass, sizeof (GyHistoryPrivate));

  obj_properties[PROP_IS_ENABLED_ACTION_NEXT] =
     g_param_spec_boolean ("is-enabled-action-next",
			   "IsEnabledActionNext",
			   "This property indicate whether the action \"go-forward\" should be enabled.",
			   FALSE, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  obj_properties[PROP_IS_ENABLED_ACTION_PREV] =
     g_param_spec_boolean ("is-enabled-action-prev",
			   "IsEnabledActionPrev",
			   "This property indicate whether the action \"go-back\" should be enabled.",
			   FALSE, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class,
				     N_PROPERTIES,
				     obj_properties);
}

static void
history_get_property (GObject    *object,
		      guint       prop_id,
		      GValue     *value,
		      GParamSpec *pspec)
{

  switch (prop_id)
  {
  case PROP_IS_ENABLED_ACTION_NEXT:
    g_value_set_boolean (value,
			 GY_HISTORY (object)->priv->is_enabled_action_next);
    break;
  case PROP_IS_ENABLED_ACTION_PREV:
    g_value_set_boolean (value,
			 GY_HISTORY (object)->priv->is_enabled_action_prev);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
history_set_property (GObject      *object,
		      guint         prop_id,
		      const GValue *value,
		      GParamSpec   *pspec)
{

  switch(prop_id)
  {
  case PROP_IS_ENABLED_ACTION_NEXT:
    GY_HISTORY (object)->priv->is_enabled_action_next = g_value_get_boolean (value);
    break;
  case PROP_IS_ENABLED_ACTION_PREV:
    GY_HISTORY (object)->priv->is_enabled_action_prev = g_value_get_boolean (value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
dispose (GObject *object)
{
  GyHistory *self = GY_HISTORY(object);

  G_OBJECT_CLASS (gy_history_parent_class)->dispose(object);
}

/* PUBLIC METHOD */

GyHistory *
gy_history_new (void)
{
  return GY_HISTORY (g_object_new (GY_TYPE_HISTORY, NULL));
}

static gint
g_history_compare_data_func (gconstpointer a,
                             gconstpointer b)
{
  return g_strcmp0 ((const gchar *) a, (const gchar *) b);
}

/**
 * gy_history_append:
 * @obj: a GyHistory.
 * @str: the string for the new item.
 *
 * Adds a new item to the end of @obj.
 */
void
gy_history_append (GyHistory   *obj,
		   const gchar *str)
{
  /* Sprawdź czy słowo znajduje się w liście.*/
  if (( g_list_find_custom (obj->priv->history, (gconstpointer) str,
			    g_history_compare_data_func)) != NULL)
      return;

  obj->priv->history = g_list_append (obj->priv->history,
				      (gpointer) g_strdup (str));
  obj->priv->iter = g_list_last (obj->priv->history);

  g_object_set (obj,
		"is-enabled-action-prev", ((g_list_length (obj->priv->history) != 1 )? TRUE : FALSE),
		"is-enabled-action-next",  FALSE,
		NULL);

  g_debug ("%s\t\tCount items in history: %d\t\tIsEnabledActionPrev: %d\t\tIsEnabledActionEnd: %d:\n", __func__,
	   g_list_length (obj->priv->history),
	   obj->priv->is_enabled_action_prev,
           obj->priv->is_enabled_action_next);

  return;
}

/**
 * gy_history_update:
 * @obj: a GyHistory.
 */
void
gy_history_update (GyHistory *obj)
{
  g_return_if_fail (GY_IS_HISTORY (obj));

  g_object_set (obj,
		"is-enabled-action-prev", ((g_list_length (obj->priv->history) == 0) || gy_history_iterable_is_begin (GY_HISTORY_ITERABLE (obj))) ? FALSE : TRUE,
		"is-enabled-action-next", FALSE, NULL);
}

/***********************INTERFACE IMPLEMENTATION******************************/
static void
gy_history_interface_init (GyHistoryIterableInterface *iface)
{
  iface->next = gy_history_next_item;
  iface->previous = gy_history_previous_item;
  iface->is_begin = gy_history_is_begin;
  iface->is_end = gy_history_is_end;
  iface->get = gy_history_get_item;
}

static void
gy_history_next_item (GyHistoryIterable *iterable)
{

  GY_HISTORY (iterable)->priv->iter = g_list_next (GY_HISTORY (iterable)->priv->iter);

  g_object_set (GY_HISTORY (iterable),
		"is-enabled-action-prev", TRUE,
		"is-enabled-action-next",   gy_history_iterable_is_end (iterable) ? FALSE : TRUE,
		NULL);

      g_debug ("%s\t\tIsEnabledActionPrev: %d\t\tIsEnabledActionEnd: %d:\n", __func__,
	       GY_HISTORY (iterable)->priv->is_enabled_action_prev,
	       GY_HISTORY (iterable)->priv->is_enabled_action_next);
}

static void
gy_history_previous_item (GyHistoryIterable *iterable)
{

  GY_HISTORY (iterable)->priv->iter = g_list_previous (GY_HISTORY (iterable)->priv->iter);

  g_object_set (GY_HISTORY (iterable),
		"is-enabled-action-prev",  gy_history_iterable_is_begin (iterable) ? FALSE : TRUE,
		"is-enabled-action-next",  TRUE,
		NULL);

    g_debug ("%s\t\tIsEnabledActionPrev: %d\t\tIsEnabledActionEnd: %d:\n", __func__,
	     GY_HISTORY (iterable)->priv->is_enabled_action_prev,
	     GY_HISTORY (iterable)->priv->is_enabled_action_next);
}

static gboolean
gy_history_is_begin (GyHistoryIterable *iterable)
{
  return (g_list_previous (GY_HISTORY (iterable)->priv->iter)) == NULL;
}

static gboolean
gy_history_is_end (GyHistoryIterable *iterable)
{
  return (g_list_next (GY_HISTORY (iterable)->priv->iter)) == NULL;
}

static gconstpointer
gy_history_get_item (GyHistoryIterable *iterable)
{
  return (gconstpointer) GY_HISTORY (iterable)->priv->iter->data;
}

