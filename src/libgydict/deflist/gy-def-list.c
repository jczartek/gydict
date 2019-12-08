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

  gchar *value_selected_row;
  gint   number_selected_row;
};

enum
{
  PROP_0,
  PROP_VALUE_SELECTED_ROW,
  PROP_NUMBER_SELECTED_ROW,
  N_PROPERTIES
};

GParamSpec* properties[N_PROPERTIES];

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
                  "value-selected-row", value,
                  "number-selected-row", number, NULL);

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
}


static void
gy_def_list_finalize (GObject *object)
{
  GyDefList *self = (GyDefList *) object;

  self->number_selected_row = -1;
  g_clear_pointer (&self->value_selected_row, g_free);

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
    case PROP_VALUE_SELECTED_ROW:
      g_value_set_string (value, self->value_selected_row);
      break;
    case PROP_NUMBER_SELECTED_ROW:
      g_value_set_int (value, self->number_selected_row);
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
    case PROP_VALUE_SELECTED_ROW:
      if (self->value_selected_row != NULL) g_clear_pointer (&self->value_selected_row, g_free);
      self->value_selected_row = g_value_dup_string (value);
      break;
    case PROP_NUMBER_SELECTED_ROW:
      self->number_selected_row = g_value_get_int (value);
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

  properties[PROP_VALUE_SELECTED_ROW] =
     g_param_spec_string ("value-selected-row",
                          "value-selected-row",
                          "The value of a selected row.",
                          NULL, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  properties[PROP_NUMBER_SELECTED_ROW] =
    g_param_spec_int ("number-selected-row",
                      "number-selected-row",
                      "The number of a selected row",
                      G_MININT, G_MAXINT, -1,
                      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);
}

static void
gy_def_list_init (GyDefList *self)
{
  self->value_selected_row = NULL;
  self->number_selected_row = -1;
  self->selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (self));

}

gint
gy_def_list_get_selected_n_row (GyDefList *self)
{
  GtkTreeSelection *selection;
  GtkTreeModel     *model;
  GtkTreeIter       iter;

  g_return_val_if_fail (GY_IS_DEF_LIST (self), -1);

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (self));

  if (gtk_tree_selection_get_selected (selection, &model, &iter))
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
  GtkTreeSelection *selection;
  GtkTreeModel     *model;
  GtkTreeIter       iter;

  g_return_val_if_fail (GY_IS_DEF_LIST (self), NULL);

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (self));

  if (gtk_tree_selection_get_selected (selection, &model, &iter))
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
  GtkTreeSelection *selection = NULL;
  GtkTreePath      *path      = NULL;

  g_return_if_fail (GY_IS_DEF_LIST (self));
  g_return_if_fail (row >= 0);

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (self));
  path = gtk_tree_path_new_from_indices (row, -1);

  if (!gtk_tree_selection_path_is_selected (selection, path))
    {
      gtk_tree_selection_select_path (selection, path);
      gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (self),
                                    path, NULL, TRUE,
                                    0.5, 0.0);
    }

  gtk_tree_path_free (path);
}

static void
select_item (GyDefList        *self,
             GtkDirectionType  direction)
{
  GtkTreeSelection *selection;
  GtkTreeModel     *model;
  GtkTreeIter       iter;

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (self));

  if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
      if (direction == GTK_DIR_UP && gtk_tree_model_iter_previous (model, &iter))
      {
        gtk_tree_selection_select_iter (selection, &iter);
      }
      else if (direction == GTK_DIR_DOWN && gtk_tree_model_iter_next (model, &iter))
      {
        gtk_tree_selection_select_iter (selection, &iter);
      }
    }
}

void
gy_def_list_select_previous_item (GyDefList *self)
{
  g_return_if_fail (GY_IS_DEF_LIST (self));
  select_item (self, GTK_DIR_UP);
}

void
gy_def_list_select_next_item (GyDefList *self)
{
  g_return_if_fail (GY_IS_DEF_LIST (self));
  select_item (self, GTK_DIR_DOWN);
}
