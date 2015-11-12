/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * gy-dialog-pref.h
 * Copyright (C) 2014 Jakub Czartek <kuba@linux.pl>
 *
 * gy-dialog-pref.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * gy-dialog-pref.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GY_DIALOG_PREF_H_
#define _GY_DIALOG_PREF_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GY_TYPE_DIALOG_PREF             (gy_dialog_pref_get_type ())
#define GY_DIALOG_PREF(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GY_TYPE_DIALOG_PREF, GyDialogPref))
#define GY_DIALOG_PREF_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GY_TYPE_DIALOG_PREF, GyDialogPrefClass))
#define GY_IS_DIALOG_PREF(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GY_TYPE_DIALOG_PREF))
#define GY_IS_DIALOG_PREF_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GY_TYPE_DIALOG_PREF))
#define GY_DIALOG_PREF_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GY_TYPE_DIALOG_PREF, GyDialogPrefClass))

typedef struct _GyDialogPref GyDialogPref;
typedef struct _GyDialogPrefClass GyDialogPrefClass;

struct _GyDialogPref
{
  GtkDialog parent_instance;
};

struct _GyDialogPrefClass
{
  GtkDialogClass parent_class;
};

GType gy_dialog_pref_get_type     (void) G_GNUC_CONST;
void  gy_dialog_pref_show_dialog  (GtkWindow *window);

G_END_DECLS

#endif /* _GY_DIALOG_PREF_H_ */

