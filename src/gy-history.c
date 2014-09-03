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

typedef struct _HistoryList HistoryList;

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
};

enum
{
  PROP_0,
  PROP_START_LIST,
  PROP_END_LIST,
  N_PROPERTIES
};
static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

G_DEFINE_TYPE (GyHistory, gy_history, G_TYPE_OBJECT);

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

    g_object_class_install_properties (object_class, N_PROPERTIES,
				       obj_properties);
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
