/* gy-workspace.c
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

#include "gy-workspace.h"
#include "dictionaries/gy-dict.h"
#include "dictionaries/gy-dict-manager.h"
#include "entrylist/gy-tree-view.h"
#include "entryview/gy-text-view.h"
#include "entryview/gy-text-buffer.h"
#include "search/gy-search-bar.h"

struct _GyWorkspace
{
  PnlDockOverlay      __parent__;
  PnlDockBin         *dockbin;
  GyTreeView         *treeview;
  GyTextView         *textview;
  GySearchBar        *search_bar;
  GyDictManager      *manager;
  GyTextBuffer       *buffer;
  GSimpleActionGroup *actions;
};

enum
{
  PROP_0,
  PROP_MANAGER,
  PROP_LEFT_WIDGET,
  PROP_RIGHT_WIDGET,
  N_PROPS
};
static GParamSpec *properties[N_PROPS];

G_DEFINE_TYPE (GyWorkspace, gy_workspace, PNL_TYPE_DOCK_OVERLAY)

static void
gy_workspace_action_alter_dict (GSimpleAction *action,
                                GVariant      *parameter,
                                gpointer       data)
{
  GyWorkspace *self = (GyWorkspace *) data;
  GyDict      *dict = NULL;
  const gchar *str;

  str = g_variant_get_string (parameter, NULL);

  dict = gy_dict_manager_set_dict (self->manager, str);
  if (!dict) return;

  gy_text_buffer_clean_buffer (self->buffer);

  gtk_tree_view_set_model (GTK_TREE_VIEW (self->treeview),
                           gy_dict_get_tree_model (dict));

  g_action_change_state (G_ACTION (action), parameter);
}

static void
gy_workspace_visibility_notify_signal (PnlDockBin *bin,
                                       gboolean    visible,
                                       guint       type_child,
                                       gpointer    data)
{
  GyWorkspace *self = (GyWorkspace *) data;

  if (visible && type_child == GTK_POS_TOP)
    {
      gy_search_bar_set_search_mode_enabled (self->search_bar, TRUE);
    }
  else if (type_child == GTK_POS_TOP)
    {
      GtkWidget *toplevel = NULL;

      gy_search_bar_set_search_mode_enabled (self->search_bar, FALSE);

      toplevel = gtk_widget_get_toplevel (GTK_WIDGET (self));
      if (gtk_widget_is_toplevel (toplevel))
        gy_window_grab_focus (GY_WINDOW (toplevel));
    }
}

static void
gy_workspace_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  GyWorkspace *self = GY_WORKSPACE (object);

  switch (prop_id)
    {
    case PROP_MANAGER:
      self->manager = g_value_dup_object (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_workspace_get_property (GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  GyWorkspace *self = GY_WORKSPACE (object);

  switch (prop_id)
    {
    case PROP_MANAGER:
      g_value_set_object (value, self->manager);
      break;
    case PROP_LEFT_WIDGET:
      g_value_take_object (value, self->treeview);
      break;
    case PROP_RIGHT_WIDGET:
      g_value_take_object (value, self->textview);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_workspace_restore_panel_state (GyWorkspace *self)
{
  g_autoptr(GSettings) settings = NULL;
  GtkWidget *pane;
  gboolean reveal;
  guint position;

  settings = g_settings_new ("org.gtk.gydict.workspace");

  pane = pnl_dock_bin_get_left_edge (PNL_DOCK_BIN (self->dockbin));
  reveal = g_settings_get_boolean (settings, "left-visible");
  position = g_settings_get_int (settings, "left-position");
  pnl_dock_revealer_set_reveal_child (PNL_DOCK_REVEALER (pane), reveal);
  pnl_dock_revealer_set_position (PNL_DOCK_REVEALER (pane), position);
}

static void
gy_workspace_save_panel_state (GyWorkspace *self)
{
  g_autoptr(GSettings) settings = NULL;
  GtkWidget *pane;
  gboolean reveal;
  guint position;

  settings = g_settings_new ("org.gtk.gydict.workspace");

  pane = pnl_dock_bin_get_left_edge (PNL_DOCK_BIN (self->dockbin));
  position = pnl_dock_revealer_get_position (PNL_DOCK_REVEALER (pane));
  reveal = pnl_dock_revealer_get_reveal_child (PNL_DOCK_REVEALER (pane));
  g_settings_set_boolean (settings, "left-visible", reveal);
  g_settings_set_int (settings, "left-position", position);
}

static void
gy_workspace_unrealize (GtkWidget *widget)
{
  GyWorkspace *self = (GyWorkspace *) widget;

  gy_workspace_save_panel_state (self);

  GTK_WIDGET_CLASS (gy_workspace_parent_class)->unrealize (widget);
}

static void
gy_workspace_destroy (GtkWidget *widget)
{
  GyWorkspace *self = GY_WORKSPACE (widget);

  if (self->manager)
    {
      g_clear_object (&self->manager);
    }

  g_clear_object (&self->actions);

  GTK_WIDGET_CLASS (gy_workspace_parent_class)->destroy (widget);
}

static void
gy_workspace_constructed (GObject *object)
{
  GyWorkspace *self = (GyWorkspace *) object;

  gy_workspace_restore_panel_state (self);

  G_OBJECT_CLASS (gy_workspace_parent_class)->constructed (object);
}

static void
gy_workspace_class_init (GyWorkspaceClass *klass)
{
  GObjectClass   *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->set_property = gy_workspace_set_property;
  object_class->get_property = gy_workspace_get_property;
  object_class->constructed = gy_workspace_constructed;

  widget_class->unrealize = gy_workspace_unrealize;
  widget_class->destroy = gy_workspace_destroy;

  gtk_widget_class_set_template_from_resource (widget_class , "/org/gtk/gydict/gy-workspace.ui");
  gtk_widget_class_bind_template_child (widget_class, GyWorkspace, dockbin);
  gtk_widget_class_bind_template_child (widget_class, GyWorkspace, treeview);
  gtk_widget_class_bind_template_child (widget_class, GyWorkspace, textview);
  gtk_widget_class_bind_template_child (widget_class, GyWorkspace, search_bar);
  gtk_widget_class_bind_template_child (widget_class, GyWorkspace, buffer);

  properties[PROP_MANAGER] =
    g_param_spec_object ("manager-dicts",
                         "manager-dicts",
                         "The manager of dictionaries.",
                         GY_TYPE_DICT_MANAGER,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
  properties[PROP_LEFT_WIDGET] =
    g_param_spec_object ("left-widget",
                         "left-widget",
                         "The left widget of the workspace.",
                         GY_TYPE_TREE_VIEW,
                         G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
  properties[PROP_RIGHT_WIDGET] =
    g_param_spec_object ("right-widget",
                         "right-widget",
                         "The right widget of the workspace.",
                         GY_TYPE_TEXT_VIEW,
                         G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
gy_workspace_init (GyWorkspace *self)
{

  static const GActionEntry entries[] = {
      {"alter-dict", gy_workspace_action_alter_dict, "s", "''", NULL}
    };

  gtk_widget_init_template (GTK_WIDGET (self));

  self->actions = g_simple_action_group_new ();
  g_action_map_add_action_entries (G_ACTION_MAP (self->actions),
                                   entries, G_N_ELEMENTS (entries), self);
  gtk_widget_insert_action_group (GTK_WIDGET (self), "workspace",
                                  G_ACTION_GROUP (self->actions));

  g_object_set_data (G_OBJECT (self->treeview), "textview", self->textview);
  g_object_set_data (G_OBJECT (self->textview), "manager", self->manager);
  g_object_set_data (G_OBJECT (self->buffer), "textview", self->textview);

  g_signal_connect (self->dockbin, "visibility-notify",
                    G_CALLBACK (gy_workspace_visibility_notify_signal), self);

}

void
gy_workspace_attach_action (GyWorkspace *self,
                            GyWindow    *win)
{
  GActionGroup *group;

  g_return_if_fail (GY_IS_WORKSPACE (self));
  g_return_if_fail (GY_IS_WINDOW    (win));

  group = gtk_widget_get_action_group (GTK_WIDGET (self->dockbin), "dockbin");
  gtk_widget_insert_action_group (GTK_WIDGET (win), "dockbin", group);

  gtk_widget_insert_action_group (GTK_WIDGET (win), "workspace",
                                  G_ACTION_GROUP (self->actions));
  group = gy_dict_manager_get_action_group (self->manager);
  gtk_widget_insert_action_group (GTK_WIDGET (win), "history", group);
}
