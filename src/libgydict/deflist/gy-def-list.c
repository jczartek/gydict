/* gy-def-list.c
 *
 * Copyright (C) 2016 Jakub Czartek <kuba@linux.pl>
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

#include <string.h>
#include "gy-def-list.h"
#include "entryview/gy-text-view.h"
#include "helpers/gy-utility-func.h"


struct _GyDefList
{
  GtkTreeView       __parent__;
  GtkTreeSelection *selection;

  gchar *selected_value;
  gint   selected_index;
  gboolean has_model;
};

enum
{
  PROP_0,
  PROP_SELECTED_VALUE,
  PROP_SELECTED_INDEX,
  PROP_HAS_MODEL,
  N_PROPERTIES
};

static GParamSpec* properties[N_PROPERTIES];

enum
{
  MOVE_SELECTION,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE (GyDefList, gy_def_list, GTK_TYPE_TREE_VIEW)

static gboolean
gy_def_list_search_equal_func (GtkTreeModel *model,
                               gint          column,
                               const gchar  *key,
                               GtkTreeIter  *iter,
                               gpointer      search_data)
{
  gboolean retval = TRUE;
  const gchar *str;
  gchar *normalized_string;
  gchar *normalized_key;
  gchar *case_normalized_string = NULL;
  gchar *case_normalized_key = NULL;
  GValue value = G_VALUE_INIT;
  GValue transformed = G_VALUE_INIT;

  gtk_tree_model_get_value (model, iter, column, &value);

  g_value_init (&transformed, G_TYPE_STRING);

  if (!g_value_transform (&value, &transformed))
    {
      g_value_unset (&value);
      return TRUE;
    }

  g_value_unset (&value);

  str = g_value_get_string (&transformed);
  if (!str)
    {
      g_value_unset (&transformed);
      return TRUE;
    }

  normalized_string = g_utf8_normalize (str, -1, G_NORMALIZE_ALL);
  normalized_key = g_utf8_normalize (key, -1, G_NORMALIZE_ALL);

  if (normalized_string && normalized_key)
    {
      case_normalized_string = g_utf8_casefold (normalized_string, -1);
      case_normalized_key = g_utf8_casefold (normalized_key, -1);

      if (gy_utility_strcmp (case_normalized_key, case_normalized_string, strlen (case_normalized_key)) == 0)
        retval = FALSE;
    }

  g_value_unset (&transformed);
  g_free (normalized_key);
  g_free (normalized_string);
  g_free (case_normalized_key);
  g_free (case_normalized_string);

  return retval;
}

static void
gy_def_list_selection_changed_cb (GtkTreeSelection *selection,
                                  gpointer          data)
{
  GyDefList *self = GY_DEF_LIST (data);
  g_autofree gchar* value = NULL;
  gint number;

  value = gy_def_list_get_value_for_selected_row (self);
  number = gy_def_list_get_selected_n_row (self);

  if (value != NULL && number != -1)
    g_object_set (G_OBJECT (self),
                  "selected-value", value,
                  "selected-index", number, NULL);

}

static void
gy_def_list_selection_move_cb (GyDefList        *self,
                               GtkDirectionType  direction,
                               gpointer          data)
{
  GtkTreeModel     *model;
  GtkTreeIter       iter;

  self->selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (self));

  if (gtk_tree_selection_get_selected (self->selection, &model, &iter) && self->has_model)
    {
      if (direction == GTK_DIR_UP && gtk_tree_model_iter_previous (model, &iter))
      {
        gtk_tree_selection_select_iter (self->selection, &iter);
      }
      else if (direction == GTK_DIR_DOWN && gtk_tree_model_iter_next (model, &iter))
      {
        gtk_tree_selection_select_iter (self->selection, &iter);
      }
    }
}

static void
gy_def_list_constructed (GObject *object)
{
  G_OBJECT_CLASS (gy_def_list_parent_class)->constructed (object);

  gtk_tree_view_set_search_equal_func (GTK_TREE_VIEW (object),
                                       gy_def_list_search_equal_func,
                                       NULL, NULL);

  g_signal_connect (GY_DEF_LIST (object)->selection, "changed",
                    G_CALLBACK (gy_def_list_selection_changed_cb), GY_DEF_LIST (object));
  g_signal_connect (GY_DEF_LIST (object), "move-selection",
                    G_CALLBACK (gy_def_list_selection_move_cb), NULL);
}


static void
gy_def_list_finalize (GObject *object)
{
  GyDefList *self = (GyDefList *) object;

  self->selected_index = -1;
  g_clear_pointer (&self->selected_value, g_free);

  G_OBJECT_CLASS (gy_def_list_parent_class)->finalize (object);
}

static void
gy_def_list_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  GyDefList *self = GY_DEF_LIST (object);

  switch (prop_id)
    {
    case PROP_SELECTED_VALUE:
      g_value_set_string (value, self->selected_value);
      break;
    case PROP_SELECTED_INDEX:
      g_value_set_int (value, self->selected_index);
      break;
    case PROP_HAS_MODEL:
      g_value_set_boolean (value, self->has_model);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_def_list_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  GyDefList *self = GY_DEF_LIST (object);

  switch (prop_id)
    {
    case PROP_SELECTED_VALUE:
      if (self->selected_value != NULL) g_clear_pointer (&self->selected_value, g_free);
      self->selected_value = g_value_dup_string (value);
      break;
    case PROP_SELECTED_INDEX:
      self->selected_index = g_value_get_int (value);
      break;
    case PROP_HAS_MODEL:
      self->has_model = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}


static void
gy_def_list_class_init (GyDefListClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = gy_def_list_constructed;
  object_class->finalize = gy_def_list_finalize;
  object_class->set_property = gy_def_list_set_property;
  object_class->get_property = gy_def_list_get_property;

  signals[MOVE_SELECTION] =
    g_signal_new ("move-selection",
                  GY_TYPE_DEF_LIST,
                  G_SIGNAL_ACTION,
                  0, NULL, NULL,
                  g_cclosure_marshal_VOID__ENUM,
                  G_TYPE_NONE, 1,
                  GTK_TYPE_DIRECTION_TYPE);

  properties[PROP_SELECTED_VALUE] =
     g_param_spec_string ("selected-value",
                          "selected-value",
                          "The value of the selected row.",
                          NULL, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  properties[PROP_SELECTED_INDEX] =
    g_param_spec_int ("selected-index",
                      "selected-index",
                      "The index of a selected row. The default value is negative one (-1).",
                      G_MININT, G_MAXINT, -1,
                      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  properties[PROP_HAS_MODEL] =
    g_param_spec_boolean ("has-model",
                          "has-model",
                          "The property determines if the tree view has model.",
                          FALSE, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);
}

static void
gy_def_list_init (GyDefList *self)
{
  self->selected_value = NULL;
  self->selected_index = -1;
  self->has_model = FALSE;
  self->selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (self));

}

gint
gy_def_list_get_selected_n_row (GyDefList *self)
{
  GtkTreeModel     *model;
  GtkTreeIter       iter;

  g_return_val_if_fail (GY_IS_DEF_LIST (self), -1);

  if (gtk_tree_selection_get_selected (self->selection, &model, &iter))
    {
      GtkTreePath *path;
      gint        *row;
      gint         number;

      path = gtk_tree_model_get_path (model, &iter);
      row  = gtk_tree_path_get_indices (path);

      number = row ? *row : -1;

      gtk_tree_path_free (path);

      return number;
    }
  return -1;
}

gchar *
gy_def_list_get_value_for_selected_row (GyDefList *self)
{
  GtkTreeModel     *model;
  GtkTreeIter       iter;

  g_return_val_if_fail (GY_IS_DEF_LIST (self), NULL);


  if (gtk_tree_selection_get_selected (self->selection, &model, &iter))
    {
      GValue  value = G_VALUE_INIT;
      gchar  *s = NULL;

      gtk_tree_model_get_value (model, &iter, 0, &value);

      s = g_value_dup_string (&value);
      g_value_unset (&value);

      return s;
    }

  return NULL;
}

void
gy_def_list_select_row (GyDefList *self,
                        gint       row)
{
  GtkTreePath      *path      = NULL;

  g_return_if_fail (GY_IS_DEF_LIST (self));
  g_return_if_fail (row >= 0);

  path = gtk_tree_path_new_from_indices (row, -1);

  if (!gtk_tree_selection_path_is_selected (self->selection, path))
    {
      gtk_tree_selection_select_path (self->selection, path);
      gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (self),
                                    path, NULL, TRUE,
                                    0.5, 0.0);
    }

  gtk_tree_path_free (path);
}

void
gy_def_list_set_model (GyDefList    *self,
                       GtkTreeModel *model)
{
  g_return_if_fail (GY_IS_DEF_LIST (self));

  gtk_tree_view_set_model (GTK_TREE_VIEW (self), model);

  gboolean has_model = !!model;

  g_object_set (G_OBJECT (self), "has-model", has_model, NULL);
}
