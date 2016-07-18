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
#include "gy-utility-func.h"


struct _GyTreeView
{
  GtkTreeView parent_instance;
};

G_DEFINE_TYPE (GyTreeView, gy_tree_view, GTK_TYPE_TREE_VIEW)

enum {
  PROP_0,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

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
  GyTreeView *self = (GyTreeView *)object;

  G_OBJECT_CLASS (gy_tree_view_parent_class)->finalize (object);
}

static void
gy_tree_view_get_property (GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  GyTreeView *self = GY_TREE_VIEW (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_tree_view_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  GyTreeView *self = GY_TREE_VIEW (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
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
  object_class->get_property = gy_tree_view_get_property;
  object_class->set_property = gy_tree_view_set_property;
}

static void
gy_tree_view_init (GyTreeView *self)
{
}
