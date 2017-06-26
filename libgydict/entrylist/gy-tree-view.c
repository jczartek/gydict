/* gy-tree-view.c
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
#include "gy-tree-view.h"
#include "entryview/gy-text-view.h"
#include "helpers/gy-utility-func.h"


struct _GyTreeView
{
  GtkTreeView       __parent__;
  GtkTreeSelection *selection;
};

G_DEFINE_TYPE (GyTreeView, gy_tree_view, GTK_TYPE_TREE_VIEW)

static void
gy_tree_view_selection_changed (GtkTreeSelection *selection,
                                gpointer          data)
{
  GtkTreeIter   iter;
  GtkTreeModel *model;
  GyTreeView   *self = GY_TREE_VIEW (data);

  if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
      GtkTreePath *path;
      gint *row;

      path = gtk_tree_model_get_path (model, &iter);
      row = gtk_tree_path_get_indices (path);

      if (row)
        {
          GyTextView *tv;

          tv = g_object_get_data (G_OBJECT (self), "textview");
          g_assert (GY_IS_TEXT_VIEW (tv));

          gy_text_view_msg_activated_row (tv, *row);
        }
      gtk_tree_path_free (path);
    }
}

static gboolean
gy_tree_view_search_equal_func (GtkTreeModel *model,
                                gint          column,
                                const gchar  *key,
                                GtkTreeIter  *iter,
                                gpointer      search_data G_GNUC_UNUSED)
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
gy_tree_view_finalize (GObject *object)
{
  G_OBJECT_CLASS (gy_tree_view_parent_class)->finalize (object);
}

static void
gy_tree_view_constructed (GObject *object)
{
  G_OBJECT_CLASS (gy_tree_view_parent_class)->constructed (object);

  gtk_tree_view_set_search_equal_func (GTK_TREE_VIEW (object),
                                       gy_tree_view_search_equal_func,
                                       NULL, NULL);
}

static void
gy_tree_view_class_init (GyTreeViewClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = gy_tree_view_constructed;
  object_class->finalize = gy_tree_view_finalize;
}

static void
gy_tree_view_init (GyTreeView *self)
{

  self->selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (self));
  g_signal_connect (self->selection, "changed",
                    G_CALLBACK (gy_tree_view_selection_changed), self);

}

gint
gy_tree_view_get_selected_row_number (GyTreeView *self)
{
  GtkTreeSelection *selection;
  GtkTreeModel     *model;
  GtkTreeIter       iter;

  g_return_val_if_fail (GY_IS_TREE_VIEW (self), -1);

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
gy_tree_view_get_value_for_selected_row (GyTreeView *self)
{
  GtkTreeSelection *selection;
  GtkTreeModel     *model;
  GtkTreeIter       iter;

  g_return_val_if_fail (GY_IS_TREE_VIEW (self), NULL);

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
gy_tree_view_select_row (GyTreeView *self,
                         gint        row)
{
  GtkTreeSelection *selection = NULL;
  GtkTreePath      *path      = NULL;

  g_return_if_fail (GY_IS_TREE_VIEW (self));
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
