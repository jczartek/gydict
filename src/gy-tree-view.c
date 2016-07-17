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

#include "gy-tree-view.h"

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
gy_tree_view_class_init (GyTreeViewClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gy_tree_view_finalize;
  object_class->get_property = gy_tree_view_get_property;
  object_class->set_property = gy_tree_view_set_property;
}

static void
gy_tree_view_init (GyTreeView *self)
{
}
