/* gb-shortcuts-dialog.c
 *
 * Copyright (C) 2015 Christian Hergert <christian@hergert.me>
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

#include <glib/gi18n.h>
#include <ide.h>

#include "egg-search-bar.h"

#include "gb-scrolled-window.h"
#include "gb-shortcuts-column.h"
#include "gb-shortcuts-dialog.h"
#include "gb-shortcuts-gesture.h"
#include "gb-shortcuts-group.h"
#include "gb-shortcuts-page.h"
#include "gb-shortcuts-shortcut.h"
#include "gb-shortcuts-view.h"

typedef struct
{
  GHashTable     *keywords;
  gchar          *initial_view;
  gchar          *last_view_name;
  GtkSizeGroup   *search_text_group;
  GtkSizeGroup   *search_image_group;
  GHashTable     *search_items_hash;

  GtkStack       *stack;
  GtkMenuButton  *menu_button;
  GtkLabel       *menu_label;
  EggSearchBar   *search_bar;
  GtkSearchEntry *search_entry;
  GtkHeaderBar   *header_bar;
  GtkPopover     *popover;
  GtkListBox     *list_box;
  GtkBox         *search_gestures;
  GtkBox         *search_shortcuts;
} GbShortcutsDialogPrivate;

typedef struct
{
  GbShortcutsDialog *self;
  GtkBuilder        *builder;
  GQueue            *stack;
  GtkWidget         *search_item;
  GQueue            *column_image_size_groups;
  GQueue            *column_desc_size_groups;
  gchar             *property_name;
  guint              translatable : 1;
} ViewsParserData;

static void gtk_buildable_iface_init (GtkBuildableIface *iface);

G_DEFINE_TYPE_EXTENDED (GbShortcutsDialog, gb_shortcuts_dialog, GTK_TYPE_WINDOW, 0,
                        G_ADD_PRIVATE (GbShortcutsDialog)
                        G_IMPLEMENT_INTERFACE (GTK_TYPE_BUILDABLE, gtk_buildable_iface_init))

enum {
  CLOSE,
  LAST_SIGNAL
};

enum {
  PROP_0,
  PROP_VIEW_NAME,
  LAST_PROP
};

static GParamSpec *gParamSpecs [LAST_PROP];
static guint gSignals [LAST_SIGNAL];

static void
gb_shortcuts_dialog_add_view (GbShortcutsDialog *self,
                              GbShortcutsView   *view)
{
  GbShortcutsDialogPrivate *priv = gb_shortcuts_dialog_get_instance_private (self);
  GtkListBoxRow *row;
  const gchar *title;
  const gchar *name;
  GtkWidget *label;

  g_assert (GB_IS_SHORTCUTS_DIALOG (self));
  g_assert (GB_IS_SHORTCUTS_VIEW (view));

  name = gb_shortcuts_view_get_view_name (view);
  title = gb_shortcuts_view_get_title (view);

  gtk_stack_add_titled (priv->stack, GTK_WIDGET (view), name, title);

  row = g_object_new (GTK_TYPE_LIST_BOX_ROW,
                      "visible", TRUE,
                      NULL);
  g_object_set_data_full (G_OBJECT (row), "GB_SHORTCUTS_VIEW_NAME", g_strdup (name), g_free);
  label = g_object_new (GTK_TYPE_LABEL,
                        "margin", 6,
                        "label", title,
                        "xalign", 0.5f,
                        "visible", TRUE,
                        NULL);
  gtk_container_add (GTK_CONTAINER (row), GTK_WIDGET (label));
  gtk_container_add (GTK_CONTAINER (priv->list_box), GTK_WIDGET (row));
}

static void
gb_shortcuts_dialog_add (GtkContainer *container,
                         GtkWidget    *widget)
{
  GbShortcutsDialog *self = (GbShortcutsDialog *)container;

  g_assert (GB_IS_SHORTCUTS_DIALOG (self));

  if (GB_IS_SHORTCUTS_VIEW (widget))
    gb_shortcuts_dialog_add_view (self, GB_SHORTCUTS_VIEW (widget));
  else
    GTK_CONTAINER_CLASS (gb_shortcuts_dialog_parent_class)->add (container, widget);
}

static void
gb_shortcuts_dialog__stack__notify_visible_child (GbShortcutsDialog *self,
                                                  GParamSpec        *pspec,
                                                  GtkStack          *stack)
{
  GbShortcutsDialogPrivate *priv = gb_shortcuts_dialog_get_instance_private (self);
  GtkWidget *visible_child;

  visible_child = gtk_stack_get_visible_child (stack);

  if (GB_IS_SHORTCUTS_VIEW (visible_child))
    {
      const gchar *title;

      title = gb_shortcuts_view_get_title (GB_SHORTCUTS_VIEW (visible_child));
      gtk_label_set_label (priv->menu_label, title);
    }
  else if (visible_child != NULL)
    {
      g_autofree gchar *title = NULL;

      gtk_container_child_get (GTK_CONTAINER (stack), visible_child,
                               "title", &title,
                               NULL);
      gtk_label_set_label (priv->menu_label, title);
    }
}

static void
gb_shortcuts_dialog__list_box__row_activated (GbShortcutsDialog *self,
                                              GtkListBoxRow     *row,
                                              GtkListBox        *list_box)
{
  GbShortcutsDialogPrivate *priv = gb_shortcuts_dialog_get_instance_private (self);
  const gchar *name;

  g_assert (GB_IS_SHORTCUTS_DIALOG (self));
  g_assert (GTK_IS_LIST_BOX_ROW (row));
  g_assert (GTK_IS_LIST_BOX (list_box));

  name = g_object_get_data (G_OBJECT (row), "GB_SHORTCUTS_VIEW_NAME");
  gtk_stack_set_visible_child_name (priv->stack, name);
  gtk_widget_hide (GTK_WIDGET (priv->popover));
}

static void
gb_shortcuts_dialog_add_search_item (GbShortcutsDialog *self,
                                     GtkWidget         *search_item)
{
  GbShortcutsDialogPrivate *priv = gb_shortcuts_dialog_get_instance_private (self);
  GString *str = g_string_new (NULL);
  gchar *downcase;

  g_assert (GB_IS_SHORTCUTS_DIALOG (self));
  g_assert (GB_IS_SHORTCUTS_SHORTCUT (search_item) || GB_IS_SHORTCUTS_GESTURE (search_item));

  if (GB_IS_SHORTCUTS_SHORTCUT (search_item))
    {
      g_autofree gchar *accelerator = NULL;
      g_autofree gchar *title = NULL;
      g_autofree gchar *hash_key = NULL;

      g_object_get (search_item,
                    "accelerator", &accelerator,
                    "title", &title,
                    NULL);

      hash_key = g_strdup_printf ("%s-%s", title, accelerator);
      if (g_hash_table_contains (priv->search_items_hash, hash_key))
        return;

      g_hash_table_insert (priv->search_items_hash, g_strdup (hash_key), GINT_TO_POINTER (1));

      g_object_set (search_item,
                    "accelerator-size-group", priv->search_image_group,
                    "title-size-group", priv->search_text_group,
                    NULL);

      g_string_append_printf (str, "%s %s", accelerator, title);

      gtk_container_add (GTK_CONTAINER (priv->search_shortcuts), search_item);
    }
  else if (GB_IS_SHORTCUTS_GESTURE (search_item))
    {
      g_autofree gchar *subtitle = NULL;
      g_autofree gchar *title = NULL;
      g_autofree gchar *hash_key = NULL;

      g_object_get (search_item,
                    "subtitle", &subtitle,
                    "title", &title,
                    NULL);

      hash_key = g_strdup_printf ("%s-%s", title, subtitle);
      if (g_hash_table_contains (priv->search_items_hash, hash_key))
        return;

      g_hash_table_insert (priv->search_items_hash, g_strdup (hash_key), GINT_TO_POINTER (1));

      g_object_set (search_item,
                    "icon-size-group", priv->search_image_group,
                    "desc-size-group", priv->search_text_group,
                    NULL);

      g_string_append_printf (str, "%s %s", title, subtitle);

      gtk_container_add (GTK_CONTAINER (priv->search_gestures), search_item);
    }

  downcase = g_utf8_strdown (str->str, str->len);
  g_hash_table_insert (priv->keywords, search_item, downcase);
  g_string_free (str, TRUE);
}

static void
gb_shortcuts_dialog__entry__changed (GbShortcutsDialog *self,
                                     GtkSearchEntry    *search_entry)
{
  GbShortcutsDialogPrivate *priv = gb_shortcuts_dialog_get_instance_private (self);
  g_autoptr(IdePatternSpec) spec = NULL;
  g_autofree gchar *downcase = NULL;
  GHashTableIter iter;
  const gchar *text;
  const gchar *last_view_name;
  gpointer key;
  gpointer value;

  g_assert (GB_IS_SHORTCUTS_DIALOG (self));
  g_assert (GTK_IS_SEARCH_ENTRY (search_entry));

  text = gtk_entry_get_text (GTK_ENTRY (search_entry));

  if (!text || !*text)
    {
      if (priv->last_view_name != NULL)
        {
          gtk_stack_set_visible_child_name (priv->stack, priv->last_view_name);
          return;
        }
    }

  last_view_name = gtk_stack_get_visible_child_name (priv->stack);

  if (g_strcmp0 (last_view_name, "internal-search") != 0)
    {
      g_free (priv->last_view_name);
      priv->last_view_name = g_strdup (last_view_name);
    }

  gtk_stack_set_visible_child_name (priv->stack, "internal-search");

  downcase = g_utf8_strdown (text, -1);
  spec = ide_pattern_spec_new (downcase);

  g_hash_table_iter_init (&iter, priv->keywords);

  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      GtkWidget *widget = key;
      const gchar *keywords = value;

      gtk_widget_set_visible (widget, ide_pattern_spec_match (spec, keywords));
    }
}

static gboolean
check_parent (GMarkupParseContext  *context,
              const gchar          *element_name,
              GError              **error)
{
  const GSList *stack;
  const gchar *parent_name;
  const gchar *our_name;

  stack = g_markup_parse_context_get_element_stack (context);
  our_name = stack->data;
  parent_name = stack->next ? stack->next->data : "";

  if (g_strcmp0 (parent_name, element_name) != 0)
    {
      gint line;
      gint col;

      g_markup_parse_context_get_position (context, &line, &col);
      g_set_error (error,
                   GTK_BUILDER_ERROR,
                   GTK_BUILDER_ERROR_INVALID_TAG,
                   "%d:%d: Element <%s> found in <%s>, expected <%s>.",
                   line, col, our_name, parent_name, element_name);
      return FALSE;
    }

  return TRUE;
}

static void
views_parser_start_element (GMarkupParseContext  *context,
                            const gchar          *element_name,
                            const gchar         **attribute_names,
                            const gchar         **attribute_values,
                            gpointer              user_data,
                            GError              **error)
{
  ViewsParserData *parser_data = user_data;
  GtkWidget *item;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (parser_data != NULL);

  if (g_strcmp0 (element_name, "views") == 0)
    {
    }
  else if (g_strcmp0 (element_name, "view") == 0)
    {
      const gchar *name = NULL;

      if (!check_parent (context, "views", error))
        return;

      if (!g_markup_collect_attributes (element_name, attribute_names, attribute_values, error,
                                        G_MARKUP_COLLECT_STRING, "name", &name,
                                        G_MARKUP_COLLECT_INVALID))
        return;

      item = g_object_new (GB_TYPE_SHORTCUTS_VIEW,
                           "view-name", name,
                           "visible", TRUE,
                           NULL);

      g_queue_push_head (parser_data->stack, g_object_ref_sink (item));
    }
  else if (g_strcmp0 (element_name, "page") == 0)
    {
      if (!check_parent (context, "view", error))
        return;

      item = g_object_new (GB_TYPE_SHORTCUTS_PAGE,
                           "visible", TRUE,
                           NULL);
      g_queue_push_head (parser_data->stack, g_object_ref_sink (item));
    }
  else if (g_strcmp0 (element_name, "column") == 0)
    {
      GtkSizeGroup *size_group;

      if (!check_parent (context, "page", error))
        return;

      size_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
      g_queue_push_head (parser_data->column_image_size_groups, size_group);

      size_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
      g_queue_push_head (parser_data->column_desc_size_groups, size_group);

      item = g_object_new (GB_TYPE_SHORTCUTS_COLUMN,
                           "visible", TRUE,
                           NULL);
      g_queue_push_head (parser_data->stack, g_object_ref_sink (item));
    }
  else if (g_strcmp0 (element_name, "group") == 0)
    {
      if (!check_parent (context, "column", error))
        return;

      item = g_object_new (GB_TYPE_SHORTCUTS_GROUP,
                           "visible", TRUE,
                           NULL);
      g_queue_push_head (parser_data->stack, g_object_ref_sink (item));
    }
  else if (g_strcmp0 (element_name, "shortcut") == 0)
    {
      GtkSizeGroup *accel_size_group;
      GtkSizeGroup *desc_size_group;

      if (!check_parent (context, "group", error))
        return;

      accel_size_group = g_queue_peek_head (parser_data->column_image_size_groups);
      desc_size_group = g_queue_peek_head (parser_data->column_desc_size_groups);

      parser_data->search_item = g_object_new (GB_TYPE_SHORTCUTS_SHORTCUT,
                                               "visible", TRUE,
                                               NULL);

      item = g_object_new (GB_TYPE_SHORTCUTS_SHORTCUT,
                           "accelerator-size-group", accel_size_group,
                           "title-size-group", desc_size_group,
                           "visible", TRUE,
                           NULL);
      g_queue_push_head (parser_data->stack, g_object_ref_sink (item));
    }
  else if (g_strcmp0 (element_name, "gesture") == 0)
    {
      GtkSizeGroup *accel_size_group;
      GtkSizeGroup *desc_size_group;

      if (!check_parent (context, "group", error))
        return;

      accel_size_group = g_queue_peek_head (parser_data->column_image_size_groups);
      desc_size_group = g_queue_peek_head (parser_data->column_desc_size_groups);

      parser_data->search_item = g_object_new (GB_TYPE_SHORTCUTS_GESTURE,
                                               "visible", TRUE,
                                               NULL);

      item = g_object_new (GB_TYPE_SHORTCUTS_GESTURE,
                           "desc-size-group", desc_size_group,
                           "icon-size-group", accel_size_group,
                           "visible", TRUE,
                           NULL);
      g_queue_push_head (parser_data->stack, g_object_ref_sink (item));
    }
  else if (g_strcmp0 (element_name, "property") == 0)
    {
      const gchar *name = NULL;
      const gchar *translatable = NULL;

      item = g_queue_peek_head (parser_data->stack);

      if (item == NULL)
        {
          g_set_error (error,
                       GTK_BUILDER_ERROR,
                       GTK_BUILDER_ERROR_INVALID_TAG,
                       "Property called without a parent object");
          return;
        }

      if (!g_markup_collect_attributes (element_name, attribute_names, attribute_values, error,
                                        G_MARKUP_COLLECT_STRING, "name", &name,
                                        G_MARKUP_COLLECT_OPTIONAL | G_MARKUP_COLLECT_STRING, "translatable", &translatable,
                                        G_MARKUP_COLLECT_INVALID))
        return;

      g_free (parser_data->property_name);
      parser_data->property_name = g_strdup (name);
      parser_data->translatable = (g_strcmp0 (translatable, "yes") == 0);
    }
  else
    {
      const GSList *stack;
      const gchar *parent_name;
      const gchar *our_name;
      gint line;
      gint col;

      stack = g_markup_parse_context_get_element_stack (context);
      our_name = stack->data;
      parent_name = stack->next ? stack->next->data : "";

      g_markup_parse_context_get_position (context, &line, &col);
      g_set_error (error,
                   GTK_BUILDER_ERROR,
                   GTK_BUILDER_ERROR_INVALID_TAG,
                   "%d:%d: Unknown element <%s> found in <%s>.",
                   line, col, our_name, parent_name);
    }
}

static void
views_parser_end_element (GMarkupParseContext  *context,
                          const gchar          *element_name,
                          gpointer              user_data,
                          GError              **error)
{
  ViewsParserData *parser_data = user_data;
  GtkWidget *item;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (parser_data != NULL);

  if (g_strcmp0 (element_name, "view") == 0)
    {
      item = g_queue_pop_head (parser_data->stack);
      gb_shortcuts_dialog_add_view (parser_data->self, GB_SHORTCUTS_VIEW (item));
      g_object_unref (item);
    }
  else if ((g_strcmp0 (element_name, "page") == 0) ||
           (g_strcmp0 (element_name, "column") == 0) ||
           (g_strcmp0 (element_name, "group") == 0) ||
           (g_strcmp0 (element_name, "shortcut") == 0) ||
           (g_strcmp0 (element_name, "gesture") == 0))
    {
      GtkWidget *parent;

      item = g_queue_pop_head (parser_data->stack);
      parent = g_queue_peek_head (parser_data->stack);
      if (item != NULL && parent != NULL)
        gtk_container_add (GTK_CONTAINER (parent), GTK_WIDGET (item));
      g_clear_object (&item);

      if ((g_strcmp0 (element_name, "shortcut") == 0) ||
          (g_strcmp0 (element_name, "gesture") == 0))
        {
          gb_shortcuts_dialog_add_search_item (parser_data->self, parser_data->search_item);
          parser_data->search_item = NULL;
        }

      if (g_strcmp0 (element_name, "column") == 0)
        {
          GtkSizeGroup *size_group;

          size_group = g_queue_pop_head (parser_data->column_image_size_groups);
          g_clear_object (&size_group);

          size_group = g_queue_pop_head (parser_data->column_desc_size_groups);
          g_clear_object (&size_group);
        }
    }
  else if (g_strcmp0 (element_name, "property") == 0)
    {
      g_clear_pointer (&parser_data->property_name, g_free);
    }
}

static void
views_parser_text (GMarkupParseContext  *context,
                   const gchar          *text,
                   gsize                 text_len,
                   gpointer              user_data,
                   GError              **error)
{
  ViewsParserData *parser_data = user_data;
  GParamSpec *pspec;
  GtkWidget *item;
  GValue value = { 0 };

  g_assert (parser_data != NULL);

  if (parser_data->property_name == NULL)
    return;

  item = g_queue_peek_head (parser_data->stack);

  if (item == NULL)
    return;

  pspec = g_object_class_find_property (G_OBJECT_GET_CLASS (item), parser_data->property_name);

  if (pspec == NULL)
    {
      g_set_error (error,
                   GTK_BUILDER_ERROR,
                   GTK_BUILDER_ERROR_INVALID_PROPERTY,
                   "No such property: %s",
                   parser_data->property_name);
      return;
    }

  if (parser_data->translatable)
    text = _(text);

  if (g_type_is_a (pspec->value_type, G_TYPE_OBJECT))
    {
      GObject *relative;

      relative = gtk_builder_get_object (parser_data->builder, text);

      if (relative == NULL)
        {
          g_set_error (error,
                       GTK_BUILDER_ERROR,
                       GTK_BUILDER_ERROR_INVALID_VALUE,
                       "Unknown object for property '%s': %s",
                       parser_data->property_name,
                       text);
          return;
        }

      g_value_init (&value, pspec->value_type);
      g_value_set_object (&value, relative);
    }
  else if (!gtk_builder_value_from_string (parser_data->builder,
                                           pspec, text, &value, error))
    return;

  if (parser_data->search_item != NULL)
    g_object_set_property (G_OBJECT (parser_data->search_item),
                           parser_data->property_name,
                           &value);
  g_object_set_property (G_OBJECT (item), parser_data->property_name, &value);
  g_value_unset (&value);
}

static GMarkupParser ViewsParser = {
  views_parser_start_element,
  views_parser_end_element,
  views_parser_text,
};

static gboolean
gb_shortcuts_dialog_custom_tag_start (GtkBuildable  *buildable,
                                      GtkBuilder    *builder,
                                      GObject       *child,
                                      const gchar   *tagname,
                                      GMarkupParser *parser,
                                      gpointer      *data)
{
  g_assert (GTK_IS_BUILDABLE (buildable));
  g_assert (GTK_IS_BUILDER (builder));
  g_assert (tagname != NULL);
  g_assert (parser != NULL);
  g_assert (data != NULL);

  if (g_strcmp0 (tagname, "views") == 0)
    {
      ViewsParserData *parser_data;

      parser_data = g_slice_new0 (ViewsParserData);
      parser_data->self = g_object_ref (buildable);
      parser_data->builder = g_object_ref (builder);
      parser_data->stack = g_queue_new ();
      parser_data->column_image_size_groups = g_queue_new ();
      parser_data->column_desc_size_groups = g_queue_new ();

      *parser = ViewsParser;
      *data = parser_data;

      return TRUE;
    }

  return FALSE;
}

static void
gb_shortcuts_dialog_custom_finished (GtkBuildable *buildable,
                                     GtkBuilder   *builder,
                                     GObject      *child,
                                     const gchar  *tagname,
                                     gpointer      user_data)
{
  g_assert (GB_IS_SHORTCUTS_DIALOG (buildable));
  g_assert (GTK_IS_BUILDER (builder));
  g_assert (tagname != NULL);

  if (g_strcmp0 (tagname, "views") == 0)
    {
      ViewsParserData *parser_data = user_data;

      g_object_unref (parser_data->self);
      g_object_unref (parser_data->builder);
      g_queue_free_full (parser_data->stack, (GDestroyNotify)g_object_unref);
      g_queue_free_full (parser_data->column_image_size_groups, (GDestroyNotify)g_object_unref);
      g_queue_free_full (parser_data->column_desc_size_groups, (GDestroyNotify)g_object_unref);
      g_slice_free (ViewsParserData, parser_data);
    }
}

static void
gb_shortcuts_dialog_real_close (GbShortcutsDialog *self)
{
  g_assert (GB_IS_SHORTCUTS_DIALOG (self));

  gtk_window_close (GTK_WINDOW (self));
}

static void
gb_shortcuts_dialog_constructed (GObject *object)
{
  GbShortcutsDialog *self = (GbShortcutsDialog *)object;
  GbShortcutsDialogPrivate *priv = gb_shortcuts_dialog_get_instance_private (self);

  G_OBJECT_CLASS (gb_shortcuts_dialog_parent_class)->constructed (object);

  if (priv->initial_view != NULL)
    gtk_stack_set_visible_child_name (priv->stack, priv->initial_view);
}

static void
gb_shortcuts_dialog_finalize (GObject *object)
{
  GbShortcutsDialog *self = (GbShortcutsDialog *)object;
  GbShortcutsDialogPrivate *priv = gb_shortcuts_dialog_get_instance_private (self);

  g_clear_pointer (&priv->keywords, g_hash_table_unref);
  g_clear_pointer (&priv->initial_view, g_free);
  g_clear_pointer (&priv->last_view_name, g_free);
  g_clear_pointer (&priv->search_items_hash, g_hash_table_unref);

  g_clear_object (&priv->search_image_group);
  g_clear_object (&priv->search_text_group);

  G_OBJECT_CLASS (gb_shortcuts_dialog_parent_class)->finalize (object);
}

static void
gtk_buildable_iface_init (GtkBuildableIface *iface)
{
  iface->custom_tag_start = gb_shortcuts_dialog_custom_tag_start;
  iface->custom_finished = gb_shortcuts_dialog_custom_finished;
}

static void
gb_shortcuts_dialog_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  GbShortcutsDialog *self = (GbShortcutsDialog *)object;
  GbShortcutsDialogPrivate *priv = gb_shortcuts_dialog_get_instance_private (self);

  switch (prop_id)
    {
    case PROP_VIEW_NAME:
      {
        GtkWidget *child = gtk_stack_get_visible_child (priv->stack);

        if (child != NULL)
          {
            gchar *name = NULL;

            gtk_container_child_get (GTK_CONTAINER (priv->stack), child,
                                     "name", &name,
                                     NULL);
            g_value_take_string (value, name);
          }
      }
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gb_shortcuts_dialog_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  GbShortcutsDialog *self = (GbShortcutsDialog *)object;
  GbShortcutsDialogPrivate *priv = gb_shortcuts_dialog_get_instance_private (self);

  switch (prop_id)
    {
    case PROP_VIEW_NAME:
      g_free (priv->initial_view);
      priv->initial_view = g_value_dup_string (value);
      gtk_stack_set_visible_child_name (priv->stack, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gb_shortcuts_dialog_class_init (GbShortcutsDialogClass *klass)
{
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);
  GtkBindingSet *binding_set = gtk_binding_set_by_class (klass);
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = gb_shortcuts_dialog_constructed;
  object_class->finalize = gb_shortcuts_dialog_finalize;
  object_class->get_property = gb_shortcuts_dialog_get_property;
  object_class->set_property = gb_shortcuts_dialog_set_property;

  container_class->add = gb_shortcuts_dialog_add;

  klass->close = gb_shortcuts_dialog_real_close;

  gParamSpecs [PROP_VIEW_NAME] =
    g_param_spec_string ("view-name",
                         "ViewName",
                         "ViewName",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, LAST_PROP, gParamSpecs);

  gSignals [CLOSE] = g_signal_new ("close",
                                   G_TYPE_FROM_CLASS (klass),
                                   G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                                   G_STRUCT_OFFSET (GbShortcutsDialogClass, close),
                                   NULL, NULL, NULL,
                                   G_TYPE_NONE,
                                   0);
  gtk_binding_entry_add_signal (binding_set, GDK_KEY_Escape, 0, "close", 0);

  g_type_ensure (GB_TYPE_SHORTCUTS_PAGE);
  g_type_ensure (GB_TYPE_SHORTCUTS_COLUMN);
  g_type_ensure (GB_TYPE_SHORTCUTS_GROUP);
  g_type_ensure (GB_TYPE_SHORTCUTS_GESTURE);
  g_type_ensure (GB_TYPE_SHORTCUTS_SHORTCUT);
}

static void
gb_shortcuts_dialog_init (GbShortcutsDialog *self)
{
  GbShortcutsDialogPrivate *priv = gb_shortcuts_dialog_get_instance_private (self);
  GtkToggleButton *search_button;
  GtkScrolledWindow *scroller;
  GtkBox *main_box;
  GtkBox *menu_box;
  GtkBox *box;
  GtkArrow *arrow;
  GtkSearchEntry *entry;

  gtk_window_set_resizable (GTK_WINDOW (self), FALSE);

  priv->keywords = g_hash_table_new_full (NULL, NULL, NULL, g_free);
  priv->search_items_hash = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);

  priv->search_text_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
  priv->search_image_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);

  priv->header_bar = g_object_new (GTK_TYPE_HEADER_BAR,
                                   "show-close-button", TRUE,
                                   "visible", TRUE,
                                   NULL);
  gtk_window_set_titlebar (GTK_WINDOW (self), GTK_WIDGET (priv->header_bar));

  search_button = g_object_new (GTK_TYPE_TOGGLE_BUTTON,
                                "child", g_object_new (GTK_TYPE_IMAGE,
                                                       "visible", TRUE,
                                                       "icon-name", "edit-find-symbolic",
                                                       NULL),
                                "visible", TRUE,
                                NULL);
  gtk_container_add (GTK_CONTAINER (priv->header_bar), GTK_WIDGET (search_button));

  main_box = g_object_new (GTK_TYPE_BOX,
                           "orientation", GTK_ORIENTATION_VERTICAL,
                           "visible", TRUE,
                           NULL);
  gtk_container_add (GTK_CONTAINER (self), GTK_WIDGET (main_box));

  priv->search_bar = g_object_new (EGG_TYPE_SEARCH_BAR,
                                   "visible", TRUE,
                                   NULL);
  g_object_bind_property (priv->search_bar, "search-mode-enabled",
                          search_button, "active",
                          G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL);
  gtk_container_add (GTK_CONTAINER (main_box), GTK_WIDGET (priv->search_bar));

  priv->stack = g_object_new (GTK_TYPE_STACK,
                              "expand", TRUE,
                              "homogeneous", TRUE,
                              "transition-type", GTK_STACK_TRANSITION_TYPE_CROSSFADE,
                              "visible", TRUE,
                              NULL);
  gtk_container_add (GTK_CONTAINER (main_box), GTK_WIDGET (priv->stack));

  priv->menu_button = g_object_new (GTK_TYPE_MENU_BUTTON,
                                    "focus-on-click", FALSE,
                                    "visible", TRUE,
                                    NULL);
  gtk_style_context_add_class (gtk_widget_get_style_context (GTK_WIDGET (priv->menu_button)),
                               "flat");
  gtk_header_bar_set_custom_title (priv->header_bar, GTK_WIDGET (priv->menu_button));

  menu_box = g_object_new (GTK_TYPE_BOX,
                           "orientation", GTK_ORIENTATION_HORIZONTAL,
                           "spacing", 6,
                           "visible", TRUE,
                           NULL);
  gtk_container_add (GTK_CONTAINER (priv->menu_button), GTK_WIDGET (menu_box));

  priv->menu_label = g_object_new (GTK_TYPE_LABEL,
                                   "visible", TRUE,
                                   NULL);
  gtk_container_add (GTK_CONTAINER (menu_box), GTK_WIDGET (priv->menu_label));

  G_GNUC_BEGIN_IGNORE_DEPRECATIONS;
  arrow = g_object_new (GTK_TYPE_ARROW,
                        "arrow-type", GTK_ARROW_DOWN,
                        "visible", TRUE,
                        NULL);
  gtk_container_add (GTK_CONTAINER (menu_box), GTK_WIDGET (arrow));
  G_GNUC_END_IGNORE_DEPRECATIONS;

  priv->popover = g_object_new (GTK_TYPE_POPOVER,
                                "border-width", 6,
                                "relative-to", priv->menu_button,
                                "position", GTK_POS_BOTTOM,
                                NULL);
  gtk_menu_button_set_popover (priv->menu_button, GTK_WIDGET (priv->popover));

  scroller = g_object_new (GB_TYPE_SCROLLED_WINDOW,
                           "min-content-width", 150,
                           "max-content-width", 300,
                           "min-content-height", 10,
                           "max-content-height", 300,
                           "shadow-type", GTK_SHADOW_IN,
                           "visible", TRUE,
                           NULL);
  gtk_container_add (GTK_CONTAINER (priv->popover), GTK_WIDGET (scroller));

  priv->list_box = g_object_new (GTK_TYPE_LIST_BOX,
                                 "selection-mode", GTK_SELECTION_NONE,
                                 "visible", TRUE,
                                 NULL);
  g_signal_connect_object (priv->list_box,
                           "row-activated",
                           G_CALLBACK (gb_shortcuts_dialog__list_box__row_activated),
                           self,
                           G_CONNECT_SWAPPED);
  gtk_container_add (GTK_CONTAINER (scroller), GTK_WIDGET (priv->list_box));

  entry = GTK_SEARCH_ENTRY (egg_search_bar_get_entry (priv->search_bar));
  g_object_set (entry,
                "placeholder-text", _("Search Shortcuts"),
                "width-chars", 40,
                NULL);
  g_signal_connect_object (entry,
                           "changed",
                           G_CALLBACK (gb_shortcuts_dialog__entry__changed),
                           self,
                           G_CONNECT_SWAPPED);

  g_signal_connect_object (priv->stack,
                           "notify::visible-child",
                           G_CALLBACK (gb_shortcuts_dialog__stack__notify_visible_child),
                           self,
                           G_CONNECT_SWAPPED);

  scroller = g_object_new (GTK_TYPE_SCROLLED_WINDOW,
                           "visible", TRUE,
                           NULL);
  box = g_object_new (GTK_TYPE_BOX,
                      "border-width", 24,
                      "halign", GTK_ALIGN_CENTER,
                      "spacing", 24,
                      "orientation", GTK_ORIENTATION_VERTICAL,
                      "visible", TRUE,
                      NULL);
  gtk_container_add (GTK_CONTAINER (scroller), GTK_WIDGET (box));
  gtk_stack_add_titled (priv->stack, GTK_WIDGET (scroller),
                        "internal-search", _("Search Results"));

  priv->search_shortcuts = g_object_new (GTK_TYPE_BOX,
                                         "halign", GTK_ALIGN_CENTER,
                                         "spacing", 6,
                                         "orientation", GTK_ORIENTATION_VERTICAL,
                                         "visible", TRUE,
                                         NULL);
  gtk_container_add (GTK_CONTAINER (box), GTK_WIDGET (priv->search_shortcuts));

  priv->search_gestures = g_object_new (GTK_TYPE_BOX,
                                        "halign", GTK_ALIGN_CENTER,
                                        "spacing", 6,
                                        "orientation", GTK_ORIENTATION_VERTICAL,
                                        "visible", TRUE,
                                        NULL);
  gtk_container_add (GTK_CONTAINER (box), GTK_WIDGET (priv->search_gestures));
}
