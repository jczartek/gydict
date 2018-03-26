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

  GObservable *observable;
};

G_DEFINE_TYPE (GyDefList, gy_def_list, GTK_TYPE_TREE_VIEW)

static void
gy_def_list_selection_changed (GtkTreeSelection *selection,
                               gpointer          data)
{
  GtkTreeIter   iter;
  GtkTreeModel *model;
  GyDefList   *self = GY_DEF_LIST (data);

  if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
      GtkTreePath *path;
      gint *row;

      path = gtk_tree_model_get_path (model, &iter);
      row = gtk_tree_path_get_indices (path);

      if (row)
        {
          if (self->observable)
            g_observable_dispatch_int (self->observable, *row);
        }
      gtk_tree_path_free (path);
    }
}

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
gy_def_list_constructed (GObject *object)
{
  G_OBJECT_CLASS (gy_def_list_parent_class)->constructed (object);

  gtk_tree_view_set_search_equal_func (GTK_TREE_VIEW (object),
                                       gy_def_list_search_equal_func,
                                       NULL, NULL);
}

static void
gy_def_list_dispose (GObject *object)
{
  GyDefList *self = GY_DEF_LIST (object);

  if (self->observable)
    {
      g_object_unref (self->observable);
      self->observable = NULL;
    }
  G_OBJECT_CLASS (gy_def_list_parent_class)->dispose (object);
}

static void
gy_def_list_class_init (GyDefListClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose     = gy_def_list_dispose;
  object_class->constructed = gy_def_list_constructed;
}

static void
gy_def_list_init (GyDefList *self)
{
  self->observable = g_observable_new (G_OBJECT (self));

  self->selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (self));
  g_signal_connect (self->selection, "changed",
                    G_CALLBACK (gy_def_list_selection_changed), self);

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

void
gy_def_list_register_observer (GyDefList *self,
                               GObserver *observer)
{
  g_return_if_fail (GY_IS_DEF_LIST (self));
  g_return_if_fail (G_IS_OBSERVER (observer));

  if (self->observable)
    g_observable_add_observer (self->observable, observer);

}

void
gy_def_list_unregister_observer (GyDefList *self,
                                 GObserver *observer)
{
  g_return_if_fail (GY_IS_DEF_LIST (self));
  g_return_if_fail (G_IS_OBSERVER (observer));

  if (self->observable)
    g_observable_delete_observer (self->observable, observer);
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
