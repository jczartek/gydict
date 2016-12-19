/* gy-lex-search-box.c
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
#include "gy-lex-search-box.h"
#include "app/gy-app.h"

struct _GyLexSearchBox
{
  GtkBox          __parent__;

  GtkSearchEntry *search_entry;
  GtkMenuButton  *chars_button;
};

G_DEFINE_TYPE (GyLexSearchBox, gy_lex_search_box, GTK_TYPE_BOX)

enum {
  PROP_0,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

GyLexSearchBox *
gy_lex_search_box_new (void)
{
  return g_object_new (GY_TYPE_LEX_SEARCH_BOX, NULL);
}

static void
gy_lex_search_box_finalize (GObject *object)
{
  GyLexSearchBox *self = (GyLexSearchBox *)object;

  G_OBJECT_CLASS (gy_lex_search_box_parent_class)->finalize (object);
}

static void
gy_lex_search_box_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  GyLexSearchBox *self = GY_LEX_SEARCH_BOX (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_lex_search_box_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  GyLexSearchBox *self = GY_LEX_SEARCH_BOX (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_lex_search_box_class_init (GyLexSearchBoxClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gy_lex_search_box_finalize;
  object_class->get_property = gy_lex_search_box_get_property;
  object_class->set_property = gy_lex_search_box_set_property;

  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (klass), "/org/gtk/gydict/gy-lex-search-box.ui");
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GyLexSearchBox, search_entry);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GyLexSearchBox, chars_button);
}

static void
gy_lex_search_box_init (GyLexSearchBox *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}


GtkSearchEntry *
_gy_lex_search_box_get_search_entry (GyLexSearchBox *self)
{
  g_return_val_if_fail (GY_IS_LEX_SEARCH_BOX (self), NULL);

  return self->search_entry;
}
