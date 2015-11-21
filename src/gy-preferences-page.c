/* gy-preferences-page.c
 *
 * Copyright (C) 2015 Jakub Czarek <kuba@linux.pl>
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

#include "gy-preferences-page.h"

typedef struct
{
  gchar   *title;
} GyPreferencesPagePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GyPreferencesPage, gy_preferences_page, GTK_TYPE_BIN);

enum {
  PROP_0,
  PROP_TITLE,
  LAST_PROP
};

static GParamSpec *gParamSpecs [LAST_PROP];

static const gchar *
gy_preferences_page_get_title (GyPreferencesPage *self)
{
  GyPreferencesPagePrivate *priv = gy_preferences_page_get_instance_private (self);

  g_return_val_if_fail (GY_IS_PREFERENCES_PAGE (self), NULL);

  return priv->title;
}

static void
gy_preferences_page_finalize (GObject *object)
{
  GyPreferencesPage *self = (GyPreferencesPage *)object;
  GyPreferencesPagePrivate *priv = gy_preferences_page_get_instance_private (self);

  g_clear_pointer (&priv->title, g_free);

  G_OBJECT_CLASS (gy_preferences_page_parent_class)->finalize (object);
}

static void
gy_preferences_page_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  GyPreferencesPage *self = GY_PREFERENCES_PAGE (object);

  switch (prop_id)
    {
    case PROP_TITLE:
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_preferences_page_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  GyPreferencesPage *self = GY_PREFERENCES_PAGE (object);

  switch (prop_id)
    {
    case PROP_TITLE:
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_preferences_page_class_init (GyPreferencesPageClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gy_preferences_page_finalize;
  object_class->get_property = gy_preferences_page_get_property;
  object_class->set_property = gy_preferences_page_set_property;

  gParamSpecs[PROP_TITLE] = g_param_spec_string ("title",
                                                 "Title",
                                                 "The title for the preferences page.",
                                                 NULL,
                                                 (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, LAST_PROP, gParamSpecs);
}

static void
gy_preferences_page_init (GyPreferencesPage *self)
{
}

void
gy_preferences_page_set_title (GyPreferencesPage *page,
                               const gchar        *title)
{
  GyPreferencesPagePrivate *priv = gy_preferences_page_get_instance_private (page);

  g_return_if_fail (GY_IS_PREFERENCES_PAGE (page));

  if (g_strcmp0 (title, priv->title) != 0)
    {
      g_free(priv->title);
      priv->title = g_strdup (title);
      g_object_notify_by_pspec (G_OBJECT (page),
                                gParamSpecs [PROP_TITLE]);
    }
}
