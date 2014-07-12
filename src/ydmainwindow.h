/* ydmainwindow.h generated by valac 0.22.1, the Vala compiler, do not modify */


#ifndef __YDMAINWINDOW_H__
#define __YDMAINWINDOW_H__

#include <glib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS
#define YD_TYPE_MAIN_WINDOW (yd_main_window_get_type ())
#define YD_MAIN_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), YD_TYPE_MAIN_WINDOW, YdMainWindow))
#define YD_MAIN_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), YD_TYPE_MAIN_WINDOW, YdMainWindowClass))
#define YD_IS_MAIN_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), YD_TYPE_MAIN_WINDOW))
#define YD_IS_MAIN_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), YD_TYPE_MAIN_WINDOW))
#define YD_MAIN_WINDOW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), YD_TYPE_MAIN_WINDOW, YdMainWindowClass))
typedef struct _YdMainWindow YdMainWindow;
typedef struct _YdMainWindowClass YdMainWindowClass;
typedef struct _YdMainWindowPrivate YdMainWindowPrivate;

struct _YdMainWindow {
    GtkWindow parent_instance;
    YdMainWindowPrivate *priv;
};

struct _YdMainWindowClass {
    GtkWindowClass parent_class;
};


GType yd_main_window_get_type(void) G_GNUC_CONST;
YdMainWindow *yd_main_window_new(void);
YdMainWindow *yd_main_window_construct(GType object_type);


void yd_main_window_show(YdMainWindow * window);


G_END_DECLS
#endif