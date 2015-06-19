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

typedef struct _HistoryList HistoryList;

static void          gy_history_next_item      (GyHistoryIterable *iterable);
static void          gy_history_previous_item  (GyHistoryIterable *iterable);
static gboolean      gy_history_is_begin       (GyHistoryIterable *iterable);
static gboolean      gy_history_is_end         (GyHistoryIterable *iterable);
static gconstpointer gy_history_get_item       (GyHistoryIterable *iterable);
static void          gy_history_interface_init (GyHistoryIterableInterface *iface);
/* Static Prototypes */
static HistoryList * history_list_alloc (void);
static void history_list_free (HistoryList *list);
static HistoryList * history_list_append (HistoryList  *list,
					  HistoryList **end,
					  GQuark        data);
static HistoryList * history_list_remove_duplicate (HistoryList  *list,
						    HistoryList **end,
						    GQuark        data);
static void history_list_remove_all (HistoryList *list);
static void history_get_property (GObject    *object,
	                          guint       prop_id,
				  GValue     *value,
				  GParamSpec *pspec);
static void history_set_property (GObject      *object,
	                          guint         prop_id,
				  const GValue *value,
				  GParamSpec   *pspec);
static void dispose (GObject *object);

struct _HistoryList
{
  HistoryList *next;
  HistoryList *prev;
  GQuark data;
};

struct _GyHistoryPrivate
{
  HistoryList *list;
  HistoryList *end;
  HistoryList *current;

  guint start_list: 1;
  guint end_list: 1;

  GList		*history;
  GyHistoryIter *iter;
  gboolean       is_begin;
  gboolean       is_end;
  gboolean       is_enabled_action_next;
  gboolean       is_enabled_action_prev;
};

enum
{
  PROP_0,
  PROP_START_LIST,
  PROP_END_LIST,
  PROP_IS_ENABLED_ACTION_NEXT,
  PROP_IS_ENABLED_ACTION_PREV,
  /* iterable */
  PROP_IS_BEGIN,
  PROP_IS_END,
  N_PROPERTIES = PROP_IS_BEGIN
};
static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

//G_DEFINE_TYPE (GyHistory, gy_history, G_TYPE_OBJECT);
G_DEFINE_TYPE_WITH_CODE (GyHistory, gy_history, G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GY_HISTORY_TYPE_ITERABLE,
				                gy_history_interface_init));

#define GET_PRIVATE(instance) G_TYPE_INSTANCE_GET_PRIVATE \
    (instance, GY_TYPE_HISTORY, GyHistoryPrivate)

/* STATIC FUNCTIONS */
static HistoryList *
history_list_alloc (void)
{
  return g_slice_new0 (HistoryList);
}

static void
history_list_free (HistoryList *list)
{
  g_slice_free (HistoryList, list);
}

static HistoryList *
history_list_append (HistoryList  *list,
		     HistoryList **end,
		     GQuark        data)
{
  HistoryList *new_list;
    

  new_list = history_list_alloc();
  new_list->next = NULL;
  new_list->data = data;
    
  if(list)
  {
    (*end)->next = new_list;
    new_list->prev = (*end);
    (*end) = new_list;
    return list;
  }
  else
  {
    new_list->prev = NULL;
    (*end) = new_list;
    return new_list;
  }

  return NULL;
}

static HistoryList *
history_list_remove_duplicate (HistoryList  *list,
			       HistoryList **end,
			       GQuark 	     data)
{
  HistoryList *tmp, *start;
  start = list;

  while (list)
  {
    if (list->data == data)
    {
      tmp = list;
      if (list->next && list->prev)
      {
	list->prev->next = list->next;
	list->next->prev = list->prev;
      }
      else if (!(list->prev) && (list->next))
      {
	list->next->prev = NULL;
	start = list->next;
      }
      else if (!(list->next) && (list->prev))
      {
	list->prev->next = NULL;
	(*end) = list->prev;
      }
      else if (!(list->next) && !(list->prev))
      {
	history_list_free(list);
	return NULL;
      }

    list = list->next;
    history_list_free(tmp);
    continue;
  }

  list = list->next;
}
    
    return start;
}

static void 
history_list_remove_all (HistoryList *list)
{
  HistoryList *tmp;

  while (list)
  {
    tmp = list;
    list = list->next;
    history_list_free(tmp);
  }
}

#if 0
static void
history_list_write_data (HistoryList *list)
{
  if (list)
  {
    g_print ("[");
    while (list)
    {
      printf ("%s,", g_quark_to_string( list->data));
      list = list->next;
    }
    g_print ("]\n");
  }
}
#endif

static void
gy_history_init (GyHistory *self)
{
  self->priv = GET_PRIVATE (self);
  self->priv->list = NULL;
  self->priv->end = NULL;
  self->priv->current = NULL;
  self->priv->start_list = FALSE;
  self->priv->end_list = FALSE;

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

    obj_properties[PROP_START_LIST] =
	g_param_spec_boolean ("start-list",
			      "Start List",
			      "FALSE for start list",
			      FALSE,
			      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_END_LIST] =
	g_param_spec_boolean ("end-list",
			      "End List",
			      "FALSE for end list",
			      FALSE,
			      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

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

  g_object_class_override_property (object_class,
				    PROP_IS_BEGIN,
				    "is-begin");

  g_object_class_override_property (object_class,
				    PROP_IS_END,
				    "is-end");
}

static void 
history_get_property (GObject    *object,
		      guint       prop_id,
		      GValue     *value,
		      GParamSpec *pspec)
{
  GyHistory *history = GY_HISTORY (object);

  switch (prop_id)
  {
    case PROP_START_LIST:
	g_value_set_boolean (value, history->priv->start_list);
	break;
    case PROP_END_LIST:
	g_value_set_boolean (value, history->priv->end_list);
	break;
  case PROP_IS_ENABLED_ACTION_NEXT:
    g_value_set_boolean (value,
			 GY_HISTORY (object)->priv->is_enabled_action_next);
    break;
  case PROP_IS_ENABLED_ACTION_PREV:
    g_value_set_boolean (value,
			 GY_HISTORY (object)->priv->is_enabled_action_prev);
    break;
  case PROP_IS_BEGIN:
    g_value_set_boolean (value,
			 GY_HISTORY (object)->priv->is_begin);
    break;
  case PROP_IS_END:
    g_value_set_boolean (value,
			 GY_HISTORY (object)->priv->is_end);
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
  GyHistory *history = GY_HISTORY (object);

  switch(prop_id)
  {
    case PROP_START_LIST:
	gy_history_set_start_list (history, g_value_get_boolean(value));
	break;
    case PROP_END_LIST:
	gy_history_set_end_list (history, g_value_get_boolean(value));
	break;
  case PROP_IS_ENABLED_ACTION_NEXT:
    GY_HISTORY (object)->priv->is_enabled_action_next = g_value_get_boolean (value);
    break;
  case PROP_IS_ENABLED_ACTION_PREV:
    GY_HISTORY (object)->priv->is_enabled_action_prev = g_value_get_boolean (value);
    break;
  case PROP_IS_BEGIN:
    GY_HISTORY (object)->priv->is_begin = g_value_get_boolean (value);
    break;
  case PROP_IS_END:
    GY_HISTORY (object)->priv->is_end = g_value_get_boolean (value);
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

  history_list_remove_all(self->priv->list);
  self->priv->end = NULL;
  self->priv->current = NULL;

  G_OBJECT_CLASS (gy_history_parent_class)->dispose(object);
}

/* PUBLIC METHOD */

GyHistory *
gy_history_new (void)
{
  return GY_HISTORY (g_object_new (GY_TYPE_HISTORY, NULL));
}

void
gy_history_set_start_list (GyHistory *self,
	                   gboolean   start_list)
{
   
  g_return_if_fail (GY_IS_HISTORY(self));

  g_object_freeze_notify (G_OBJECT (self));

  if (self->priv->start_list != start_list)
  {
    self->priv->start_list = start_list;
    g_object_notify (G_OBJECT (self), "start-list");
  }

  g_object_thaw_notify (G_OBJECT (self));
}

void
gy_history_set_end_list (GyHistory *self,
	                 gboolean   end_list)
{
   
  g_return_if_fail (GY_IS_HISTORY(self));

  g_object_freeze_notify (G_OBJECT (self));

  if (self->priv->end_list != end_list)
  {
    self->priv->end_list = end_list;
    g_object_notify (G_OBJECT (self), "end-list");
  }

  g_object_thaw_notify (G_OBJECT (self));
}

gboolean
gy_history_get_start_list (GyHistory *self)
{
  return (gboolean) self->priv->start_list;
}

gboolean
gy_history_get_end_list (GyHistory *self)
{
  return (gboolean) self->priv->end_list;
}

void
gy_history_add_list (GyHistory    *self,
		     const gchar  *string)
{
  GQuark data;

  data = g_quark_from_string (string);
  self->priv->list = history_list_remove_duplicate (self->priv->list,
						    &self->priv->end,
						     data);
  self->priv->list = history_list_append(self->priv->list,
					 &self->priv->end,
					 data);
  self->priv->current = self->priv->end;

#if 0
  history_list_write_data(self->priv->list);
#endif

  if (!(self->priv->list->next) && !(self->priv->list->prev))
  {
    gy_history_set_start_list(self, TRUE);
    gy_history_set_end_list(self, FALSE);
    return;
  }
  gy_history_update_current_history(self);
}

const gchar *
gy_history_get_string_from_quark (GyHistory *self)
{
  return g_quark_to_string (self->priv->current->data);
}

void
gy_history_update_current_history (GyHistory *self)
{
  gboolean back;
  gboolean forward;

  back = self->priv->current->prev ? TRUE : FALSE;
  forward = self->priv->current->next ? TRUE : FALSE;

  gy_history_set_start_list(self, back);
  gy_history_set_end_list(self, forward);

}

void
gy_history_go_back (GyHistory *self)
{
  if (self->priv->current->prev)
  {
      self->priv->current = self->priv->current->prev;
  }
}

void
gy_history_go_forward (GyHistory *self)
{
  if (self->priv->current->next)
  {
    self->priv->current = self->priv->current->next;
  }
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

  g_print ("%s\t\tCount items in history: %d\t\tIsEnabledActionPrev: %d\t\tIsEnabledActionEnd: %d:\n", __func__,
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
		"is-enabled-action-prev", ((g_list_length (obj->priv->history) == 0) && gy_history_iterable_is_begin (GY_HISTORY_ITERABLE (obj))) ? FALSE : TRUE,
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

      g_print ("%s\t\tIsEnabledActionPrev: %d\t\tIsEnabledActionEnd: %d:\n", __func__,
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

    g_print ("%s\t\tIsEnabledActionPrev: %d\t\tIsEnabledActionEnd: %d:\n", __func__,
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

