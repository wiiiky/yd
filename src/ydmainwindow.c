/* ydmainwindow.c generated by valac 0.22.1, the Vala compiler
 * generated from ydmainwindow.vala, do not modify */


#include <glib.h>
#include <glib-object.h>
#include "ydmainwindow.h"
#include "ydtcpdetail.h"
#include "procnet.h"
#include "wdetect.h"
#include "notify.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <gdk/gdk.h>
#include <pango/pango.h>

#define YD_MAIN_WINDOW_UPDATE_TIMEOUT   (1200)


#define YD_MAIN_WINDOW_TYPE_TCP_COLUMNS (yd_main_window_tcp_columns_get_type ())
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_free0(var) (var = (g_free (var), NULL))
typedef struct _Block1Data Block1Data;

struct _YdMainWindowPrivate {
    GtkStack *stack;
    GtkTreeView *tcpview;
    GtkTreeView *udpview;

    guint tcp_to;
    guint udp_to;

    GAsyncQueue *queue;
};

typedef enum {
    YD_MAIN_WINDOW_TCP_COLUMNS_INFO = 0,
    YD_MAIN_WINDOW_TCP_COLUMNS_LOCALADDR,
    YD_MAIN_WINDOW_TCP_COLUMNS_REMOTEADDR,
    YD_MAIN_WINDOW_TCP_COLUMNS_STATE,
    YD_MAIN_WINDOW_TCP_COLUMNS_UID,
    YD_MAIN_WINDOW_TCP_COLUMNS_POINTER,
    YD_MAIN_WINDOW_TCP_COLUMNS_FGRGBA,
} YdMainWindowTcpColumns;

struct _Block1Data {
    int _ref_count_;
    YdMainWindow *self;
    GtkAboutDialog *dialog;
};


static gpointer yd_main_window_parent_class = NULL;

#define YD_MAIN_WINDOW_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), YD_TYPE_MAIN_WINDOW, YdMainWindowPrivate))
enum {
    YD_MAIN_WINDOW_DUMMY_PROPERTY
};
static GType yd_main_window_tcp_columns_get_type(void) G_GNUC_UNUSED;


#define YD_MAIN_WINDOW_TCP_TAB_TITLE "  TCP  "
#define YD_MAIN_WINDOW_UDP_TAB_TITLE "  UDP  "

#define YD_MAIN_WINDOW_TCP_TAB_NAME "tcp"
#define YD_MAIN_WINDOW_UDP_TAB_NAME "udp"

#define YD_MAIN_WINDOW_TCP_HDR_NO " INFO "
#define YD_MAIN_WINDOW_TCP_HDR_LOCAL " Local Address "
#define YD_MAIN_WINDOW_TCP_HDR_REMOTE " Foreign Address "
#define YD_MAIN_WINDOW_TCP_HDR_STATE " State "
#define YD_MAIN_WINDOW_TCP_HDR_TRQUEUE " Send-Q "
#define YD_MAIN_WINDOW_TCP_HDR_REQUEUE " Recv-Q "
#define YD_MAIN_WINDOW_TCP_HDR_UID " Uid "


static void _gtk_main_quit_gtk_widget_destroy(GtkWidget * _sender,
                                              gpointer self);
static void yd_main_window_about_item_activate(YdMainWindow * self);
static void
_yd_main_window_about_item_activate_gtk_menu_item_activate(GtkMenuItem *
                                                           _sender,
                                                           gpointer self);
static void yd_main_window_stack_name_changed(YdMainWindow * self);
static Block1Data *block1_data_ref(Block1Data * _data1_);
static void block1_data_unref(void *_userdata_);
static void __lambda2_(Block1Data * _data1_, gint response_id);
static void ___lambda2__gtk_dialog_response(GtkDialog * _sender,
                                            gint response_id,
                                            gpointer self);
static void yd_main_window_finalize(GObject * obj);
static void _vala_array_destroy(gpointer array, gint array_length,
                                GDestroyNotify destroy_func);
static void _vala_array_free(gpointer array, gint array_length,
                             GDestroyNotify destroy_func);

/*
 * 从列表中查找匹配的TCP项，只匹配本地地址和端口号
 * 找到返回TRUE，并设置GtkTreeIter *iter，否则返回FALSE
 */
static gboolean gtk_tree_model_find_tcp_entry(GtkTreeModel * model,
                                              uint32_t addr, uint16_t port,
                                              GtkTreeIter * iter);

static gboolean yd_main_window_listen_timeout(gpointer data);

/*
 * 双击TCP列表中的一项
 */
static void yd_main_window_tcpview_activated(GtkTreeView * tcpview,
                                             GtkTreePath * path,
                                             GtkTreeViewColumn * column,
                                             gpointer user_data);


static GType yd_main_window_tcp_columns_get_type(void)
{
    static volatile gsize yd_main_window_tcp_columns_type_id__volatile = 0;
    if (g_once_init_enter(&yd_main_window_tcp_columns_type_id__volatile)) {
        static const GEnumValue values[] =
            { {YD_MAIN_WINDOW_TCP_COLUMNS_INFO,
               "YD_MAIN_WINDOW_TCP_COLUMNS_INFO", "tcp-col-no"},
        {YD_MAIN_WINDOW_TCP_COLUMNS_LOCALADDR,
         "YD_MAIN_WINDOW_TCP_COLUMNS_LOCALADDR",
         "tcp-col-localaddr"},
        {YD_MAIN_WINDOW_TCP_COLUMNS_REMOTEADDR,
         "YD_MAIN_WINDOW_TCP_COLUMNS_REMOTEADDR",
         "tcp-col-remoteaddr"},
        {YD_MAIN_WINDOW_TCP_COLUMNS_STATE,
         "YD_MAIN_WINDOW_TCP_COLUMNS_STATE",
         "tcp-col-state"},
        {YD_MAIN_WINDOW_TCP_COLUMNS_UID,
         "YD_MAIN_WINDOW_TCP_COLUMNS_UID", "tcp-col-uid"},
        {YD_MAIN_WINDOW_TCP_COLUMNS_POINTER,
         "YD_MAIN_WINDOW_TCP_COLUMNS_POINTER", "tcp-col-pointer"},
        {0,
         NULL,
         NULL}
        };
        GType yd_main_window_tcp_columns_type_id;
        yd_main_window_tcp_columns_type_id =
            g_enum_register_static("YdMainWindowTcpColumns", values);
        g_once_init_leave(&yd_main_window_tcp_columns_type_id__volatile,
                          yd_main_window_tcp_columns_type_id);
    }
    return yd_main_window_tcp_columns_type_id__volatile;
}


void yd_main_window_show(YdMainWindow * self)
{
    g_return_if_fail(self != NULL);
    gtk_widget_show_all((GtkWidget *) self);
    yd_detect_run(self->priv->queue);
    g_timeout_add(500, yd_main_window_listen_timeout, self);
}


static void _gtk_main_quit_gtk_widget_destroy(GtkWidget * _sender,
                                              gpointer self)
{
    gtk_main_quit();
}


static void
_yd_main_window_about_item_activate_gtk_menu_item_activate(GtkMenuItem *
                                                           _sender,
                                                           gpointer self)
{
    yd_main_window_about_item_activate(self);
}


YdMainWindow *yd_main_window_construct(GType object_type)
{
    YdMainWindow *self = NULL;
    GtkAccelGroup *accel_group = NULL;
    GtkAccelGroup *_tmp0_ = NULL;
    GtkBox *mainbox = NULL;
    GtkBox *_tmp1_ = NULL;
    GtkMenuBar *menubar = NULL;
    GtkMenuBar *_tmp2_ = NULL;
    GtkMenuItem *fileitem = NULL;
    GtkMenuItem *_tmp3_ = NULL;
    GtkMenu *filemenu = NULL;
    GtkMenu *_tmp4_ = NULL;
    GtkMenuItem *openitem = NULL;
    GtkMenuItem *_tmp5_ = NULL;
    GtkMenuItem *aboutitem = NULL;
    GtkMenuItem *_tmp6_ = NULL;
    GtkMenu *aboutmenu = NULL;
    GtkMenu *_tmp7_ = NULL;
    GtkMenuItem *real_aboutitem = NULL;
    GtkMenuItem *_tmp8_ = NULL;
    GtkBox *vbox = NULL;
    GtkBox *_tmp9_ = NULL;
    GtkStackSwitcher *switcher = NULL;
    GtkStackSwitcher *_tmp10_ = NULL;
    GtkStack *_tmp11_ = NULL;
    GtkStack *_tmp12_ = NULL;
    GtkStack *_tmp13_ = NULL;
    GtkStack *_tmp14_ = NULL;
    GtkStack *_tmp15_ = NULL;
    GtkStack *_tmp16_ = NULL;
    GtkListStore *store = NULL;
    GtkListStore *_tmp17_ = NULL;
    GtkListStore *_tmp18_ = NULL;
    GtkTreeView *_tmp19_ = NULL;
    GtkCellRendererText *cell = NULL;
    GtkCellRendererText *_tmp20_ = NULL;
    GtkTreeView *_tmp21_ = NULL;
    GtkCellRendererText *_tmp22_ = NULL;
    GtkTreeView *_tmp23_ = NULL;
    GtkCellRendererText *_tmp24_ = NULL;
    GtkTreeView *_tmp25_ = NULL;
    GtkCellRendererText *_tmp26_ = NULL;
    GtkTreeView *_tmp27_ = NULL;
    GtkCellRendererText *_tmp28_ = NULL;
    GtkTreeView *_tmp33_ = NULL;
    GtkCellRendererText *_tmp34_ = NULL;
    GtkListStore *_tmp35_ = NULL;
    GtkListStore *_tmp36_ = NULL;
    GtkTreeView *_tmp37_ = NULL;
    GtkCellRendererText *_tmp38_ = NULL;
    GtkTreeView *_tmp39_ = NULL;
    GtkCellRendererText *_tmp40_ = NULL;
    GtkTreeView *_tmp41_ = NULL;
    GtkCellRendererText *_tmp42_ = NULL;
    GtkTreeView *_tmp43_ = NULL;
    GtkCellRendererText *_tmp44_ = NULL;
    GtkTreeView *_tmp45_ = NULL;
    GtkCellRendererText *_tmp46_ = NULL;
    GtkStack *_tmp47_ = NULL;
    GtkStack *_tmp49_ = NULL;
    GtkStack *_tmp51_ = NULL;
    GtkLabel *_tmp52_ = NULL;
    GtkLabel *_tmp53_ = NULL;
    self = (YdMainWindow *) g_object_new(object_type, NULL);
    gtk_window_set_title((GtkWindow *) self, "Network Detection");
    g_object_set((GtkWindow *) self, "window-position", GTK_WIN_POS_CENTER,
                 NULL);
    g_signal_connect((GtkWidget *) self, "destroy",
                     (GCallback) _gtk_main_quit_gtk_widget_destroy, NULL);
    gtk_window_set_default_size((GtkWindow *) self, 685, 500);
    _tmp0_ = gtk_accel_group_new();
    accel_group = _tmp0_;
    gtk_window_add_accel_group((GtkWindow *) self, accel_group);
    _tmp1_ = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    g_object_ref_sink(_tmp1_);
    mainbox = _tmp1_;
    gtk_container_add((GtkContainer *) self, (GtkWidget *) mainbox);
    _tmp2_ = (GtkMenuBar *) gtk_menu_bar_new();
    g_object_ref_sink(_tmp2_);
    menubar = _tmp2_;
    gtk_box_pack_start(mainbox, (GtkWidget *) menubar, FALSE, FALSE,
                       (guint) 0);
    _tmp3_ = (GtkMenuItem *) gtk_menu_item_new_with_label("File");
    g_object_ref_sink(_tmp3_);
    fileitem = _tmp3_;
    gtk_container_add((GtkContainer *) menubar, (GtkWidget *) fileitem);
    _tmp4_ = (GtkMenu *) gtk_menu_new();
    g_object_ref_sink(_tmp4_);
    filemenu = _tmp4_;
    gtk_menu_item_set_submenu(fileitem, (GtkWidget *) filemenu);
    _tmp5_ = (GtkMenuItem *) gtk_menu_item_new_with_label("Open");
    g_object_ref_sink(_tmp5_);
    openitem = _tmp5_;
    gtk_container_add((GtkContainer *) filemenu, (GtkWidget *) openitem);
    _tmp6_ = (GtkMenuItem *) gtk_menu_item_new_with_mnemonic("_About");
    g_object_ref_sink(_tmp6_);
    aboutitem = _tmp6_;
    gtk_container_add((GtkContainer *) menubar, (GtkWidget *) aboutitem);
    _tmp7_ = (GtkMenu *) gtk_menu_new();
    g_object_ref_sink(_tmp7_);
    aboutmenu = _tmp7_;
    gtk_menu_item_set_submenu(aboutitem, (GtkWidget *) aboutmenu);
    _tmp8_ = (GtkMenuItem *) gtk_menu_item_new_with_label("About");
    g_object_ref_sink(_tmp8_);
    real_aboutitem = _tmp8_;
    gtk_container_add((GtkContainer *) aboutmenu,
                      (GtkWidget *) real_aboutitem);
    g_signal_connect_object(real_aboutitem, "activate", (GCallback)
                            _yd_main_window_about_item_activate_gtk_menu_item_activate,
                            self, 0);
    gtk_widget_add_accelerator((GtkWidget *) real_aboutitem, "activate",
                               accel_group, (guint) 'a', GDK_CONTROL_MASK,
                               GTK_ACCEL_VISIBLE);
    _tmp9_ = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    g_object_ref_sink(_tmp9_);
    vbox = _tmp9_;
    gtk_container_set_border_width((GtkContainer *) vbox, (guint) 6);
    gtk_box_pack_start(mainbox, (GtkWidget *) vbox, TRUE, TRUE, (guint) 0);
    _tmp10_ = (GtkStackSwitcher *) gtk_stack_switcher_new();
    g_object_ref_sink(_tmp10_);
    switcher = _tmp10_;
    gtk_box_pack_start(vbox, (GtkWidget *) switcher, FALSE, FALSE,
                       (guint) 0);
    _tmp11_ = (GtkStack *) gtk_stack_new();
    g_object_ref_sink(_tmp11_);
    _g_object_unref0(self->priv->stack);
    self->priv->stack = _tmp11_;
    _tmp12_ = self->priv->stack;
    g_signal_connect_swapped(_tmp12_, "notify::visible-child-name",
                             (GCallback) yd_main_window_stack_name_changed,
                             self);
    _tmp13_ = self->priv->stack;
    gtk_stack_switcher_set_stack(switcher, _tmp13_);
    _tmp14_ = self->priv->stack;
    gtk_stack_set_transition_duration(_tmp14_, (guint) 1000);
    _tmp15_ = self->priv->stack;
    gtk_stack_set_transition_type(_tmp15_,
                                  GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    _tmp16_ = self->priv->stack;
    gtk_box_pack_start(vbox, (GtkWidget *) _tmp16_, TRUE, TRUE, (guint) 0);
    _tmp17_ =
        gtk_list_store_new(7, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                           G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER,
                           G_TYPE_STRING);
    store = _tmp17_;
    _tmp18_ = store;
    _tmp19_ = (GtkTreeView *) gtk_tree_view_new_with_model((GtkTreeModel *)
                                                           _tmp18_);
    gtk_tree_view_set_activate_on_single_click(_tmp19_, FALSE);
    g_signal_connect(G_OBJECT(_tmp19_), "row-activated",
                     G_CALLBACK(yd_main_window_tcpview_activated), NULL);
    g_object_ref_sink(_tmp19_);
    _g_object_unref0(self->priv->tcpview);
    self->priv->tcpview = _tmp19_;
    _tmp20_ = (GtkCellRendererText *) gtk_cell_renderer_text_new();
    g_object_ref_sink(_tmp20_);
    cell = _tmp20_;
    _tmp21_ = self->priv->tcpview;
    _tmp22_ = cell;
    gtk_tree_view_insert_column_with_attributes(_tmp21_, -1,
                                                YD_MAIN_WINDOW_TCP_HDR_NO,
                                                (GtkCellRenderer *)
                                                _tmp22_, "text",
                                                YD_MAIN_WINDOW_TCP_COLUMNS_INFO,
                                                "foreground",
                                                YD_MAIN_WINDOW_TCP_COLUMNS_FGRGBA,
                                                NULL);
    _tmp23_ = self->priv->tcpview;
    _tmp24_ = cell;
    gtk_tree_view_insert_column_with_attributes(_tmp23_, -1,
                                                YD_MAIN_WINDOW_TCP_HDR_LOCAL,
                                                (GtkCellRenderer *)
                                                _tmp24_, "text",
                                                YD_MAIN_WINDOW_TCP_COLUMNS_LOCALADDR,
                                                "foreground",
                                                YD_MAIN_WINDOW_TCP_COLUMNS_FGRGBA,
                                                NULL);
    _tmp25_ = self->priv->tcpview;
    _tmp26_ = cell;
    gtk_tree_view_insert_column_with_attributes(_tmp25_, -1,
                                                YD_MAIN_WINDOW_TCP_HDR_REMOTE,
                                                (GtkCellRenderer *)
                                                _tmp26_, "text",
                                                YD_MAIN_WINDOW_TCP_COLUMNS_REMOTEADDR,
                                                "foreground",
                                                YD_MAIN_WINDOW_TCP_COLUMNS_FGRGBA,
                                                NULL);
    _tmp27_ = self->priv->tcpview;
    _tmp28_ = cell;
    gtk_tree_view_insert_column_with_attributes(_tmp27_, -1,
                                                YD_MAIN_WINDOW_TCP_HDR_STATE,
                                                (GtkCellRenderer *)
                                                _tmp28_, "text",
                                                YD_MAIN_WINDOW_TCP_COLUMNS_STATE,
                                                "foreground",
                                                YD_MAIN_WINDOW_TCP_COLUMNS_FGRGBA,
                                                NULL);
    _tmp33_ = self->priv->tcpview;
    _tmp34_ = cell;
    gtk_tree_view_insert_column_with_attributes(_tmp33_, -1,
                                                YD_MAIN_WINDOW_TCP_HDR_UID,
                                                (GtkCellRenderer *)
                                                _tmp34_, "text",
                                                YD_MAIN_WINDOW_TCP_COLUMNS_UID,
                                                "foreground",
                                                YD_MAIN_WINDOW_TCP_COLUMNS_FGRGBA,
                                                NULL);
    _tmp35_ =
        gtk_list_store_new(6, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                           G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    _g_object_unref0(store);
    store = _tmp35_;
    _tmp36_ = store;
    _tmp37_ = (GtkTreeView *) gtk_tree_view_new_with_model((GtkTreeModel *)
                                                           _tmp36_);
    g_object_ref_sink(_tmp37_);
    _g_object_unref0(self->priv->udpview);
    self->priv->udpview = _tmp37_;
    _tmp38_ = (GtkCellRendererText *) gtk_cell_renderer_text_new();
    g_object_ref_sink(_tmp38_);
    _g_object_unref0(cell);
    cell = _tmp38_;
    _tmp39_ = self->priv->udpview;
    _tmp40_ = cell;
    gtk_tree_view_insert_column_with_attributes(_tmp39_, -1, " No. ",
                                                (GtkCellRenderer *)
                                                _tmp40_, "text", 0, NULL);
    _tmp41_ = self->priv->udpview;
    _tmp42_ = cell;
    gtk_tree_view_insert_column_with_attributes(_tmp41_, -1,
                                                " local address ",
                                                (GtkCellRenderer *)
                                                _tmp42_, "text", 0, NULL);
    _tmp43_ = self->priv->udpview;
    _tmp44_ = cell;
    gtk_tree_view_insert_column_with_attributes(_tmp43_, -1,
                                                " remote address ",
                                                (GtkCellRenderer *)
                                                _tmp44_, "text", 0, NULL);
    _tmp45_ = self->priv->udpview;
    _tmp46_ = cell;
    gtk_tree_view_insert_column_with_attributes(_tmp45_, -1, " state ",
                                                (GtkCellRenderer *)
                                                _tmp46_, "text", 0, NULL);
    _tmp47_ = self->priv->stack;
    GtkWidget *scrollview = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrollview),
                      GTK_WIDGET(self->priv->tcpview));
    gtk_stack_add_titled(_tmp47_, scrollview, YD_MAIN_WINDOW_TCP_TAB_NAME,
                         YD_MAIN_WINDOW_TCP_TAB_TITLE);
    _tmp49_ = self->priv->stack;
    scrollview = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrollview),
                      GTK_WIDGET(self->priv->udpview));
    gtk_stack_add_titled(_tmp49_, scrollview, YD_MAIN_WINDOW_UDP_TAB_NAME,
                         YD_MAIN_WINDOW_UDP_TAB_TITLE);
    _tmp51_ = self->priv->stack;
    _tmp52_ = (GtkLabel *) gtk_label_new("TODO");
    g_object_ref_sink(_tmp52_);
    _tmp53_ = _tmp52_;
    gtk_stack_add_titled(_tmp51_, (GtkWidget *) _tmp53_, "todo", "TODO");
    _g_object_unref0(_tmp53_);
    _g_object_unref0(cell);
    _g_object_unref0(store);
    _g_object_unref0(switcher);
    _g_object_unref0(vbox);
    _g_object_unref0(real_aboutitem);
    _g_object_unref0(aboutmenu);
    _g_object_unref0(aboutitem);
    _g_object_unref0(openitem);
    _g_object_unref0(filemenu);
    _g_object_unref0(fileitem);
    _g_object_unref0(menubar);
    _g_object_unref0(mainbox);
    _g_object_unref0(accel_group);

    self->priv->tcp_to = 0;
    self->priv->udp_to = 0;

    self->priv->queue = g_async_queue_new();

    return self;
}


YdMainWindow *yd_main_window_new(void)
{
    return yd_main_window_construct(YD_TYPE_MAIN_WINDOW);
}

static void yd_main_window_update_tcp(YdMainWindow * self)
{
    GList *tcps = proc_net_tcp_open();

    GtkTreeModel *model = gtk_tree_view_get_model(self->priv->tcpview);
    GtkListStore *store = (GtkListStore *) model;
    GtkTreeIter iter;
    if (gtk_tree_model_get_iter_first(model, &iter)) {
        do {
            ProcNetTcpEntry *tcp = NULL;
            gtk_tree_model_get(model, &iter,
                               YD_MAIN_WINDOW_TCP_COLUMNS_POINTER, &tcp,
                               -1);
            GList *ptr = tcps;
            while (ptr) {
                ProcNetTcpEntry *entry = (ProcNetTcpEntry *) ptr->data;
                if (g_strcmp0(entry->local_address, tcp->local_address) ==
                    0
                    && g_strcmp0(entry->rem_address,
                                 tcp->rem_address) == 0) {
                    /* 在列表中找到与当前行对应的项,更新 */
                    gtk_list_store_set(store, &iter,
                                       YD_MAIN_WINDOW_TCP_COLUMNS_LOCALADDR,
                                       make_tcp_local_address_with_port
                                       (entry),
                                       YD_MAIN_WINDOW_TCP_COLUMNS_REMOTEADDR,
                                       make_tcp_remote_address_with_port
                                       (entry),
                                       YD_MAIN_WINDOW_TCP_COLUMNS_STATE,
                                       make_tcp_state(entry),
                                       YD_MAIN_WINDOW_TCP_COLUMNS_UID,
                                       make_tcp_uid(entry),
                                       YD_MAIN_WINDOW_TCP_COLUMNS_POINTER,
                                       entry, -1);
                    proc_net_tcp_entry_free(tcp);
                    tcps = g_list_remove_link(tcps, ptr);
                    g_list_free_1(ptr);
                    break;
                }
                ptr = g_list_next(ptr);
            }

            if (ptr == NULL) {
                /* 未找到，删除 */
                proc_net_tcp_entry_free(tcp);
                gtk_list_store_remove(store, &iter);
            } else {
                gtk_tree_model_iter_next(model, &iter);
            }
        } while (gtk_list_store_iter_is_valid(store, &iter));
    }

    /* 如果列表中还有项，那么是新加入的 */
    GList *ptr = tcps;
    while (ptr) {
        ProcNetTcpEntry *entry = (ProcNetTcpEntry *) ptr->data;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                           YD_MAIN_WINDOW_TCP_COLUMNS_INFO, "",
                           YD_MAIN_WINDOW_TCP_COLUMNS_LOCALADDR,
                           make_tcp_local_address_with_port(entry),
                           YD_MAIN_WINDOW_TCP_COLUMNS_REMOTEADDR,
                           make_tcp_remote_address_with_port(entry),
                           YD_MAIN_WINDOW_TCP_COLUMNS_STATE,
                           make_tcp_state(entry),
                           YD_MAIN_WINDOW_TCP_COLUMNS_UID,
                           make_tcp_uid(entry),
                           YD_MAIN_WINDOW_TCP_COLUMNS_POINTER, entry, -1);
        ptr = g_list_next(ptr);
    }

    g_list_free(tcps);
    //proc_net_tcp_close (tcps);
}

static gboolean yd_main_window_update_tcp_timeout(gpointer data)
{
    YdMainWindow *self = (YdMainWindow *) data;
    yd_main_window_update_tcp(self);
    return TRUE;
}

static void yd_main_window_remove_timeout(YdMainWindow * self)
{
    if (self->priv->tcp_to) {
        g_source_remove(self->priv->tcp_to);
    }
    if (self->priv->udp_to) {
        g_source_remove(self->priv->udp_to);
    }
    self->priv->tcp_to = 0;
    self->priv->udp_to = 0;
}

static void yd_main_window_switch_to_tcp(YdMainWindow * self)
{
    yd_main_window_remove_timeout(self);
    yd_main_window_update_tcp_timeout(self);
    self->priv->tcp_to = g_timeout_add(YD_MAIN_WINDOW_UPDATE_TIMEOUT,
                                       yd_main_window_update_tcp_timeout,
                                       self);
}

static gboolean yd_main_window_update_udp_timeout(gpointer data)
{
    return TRUE;
}

static void yd_main_window_switch_to_udp(YdMainWindow * self)
{
    yd_main_window_remove_timeout(self);
    self->priv->udp_to = g_timeout_add(YD_MAIN_WINDOW_UPDATE_TIMEOUT,
                                       yd_main_window_update_udp_timeout,
                                       self);
}

static void yd_main_window_switch_to_todo(YdMainWindow * self)
{
    yd_main_window_remove_timeout(self);
}


static void yd_main_window_stack_name_changed(YdMainWindow * self)
{
    GtkStack *stack = self->priv->stack;
    const gchar *name = gtk_stack_get_visible_child_name(stack);

    if (g_strcmp0(name, YD_MAIN_WINDOW_TCP_TAB_NAME) == 0) {
        yd_main_window_switch_to_tcp(self);
    } else if (g_strcmp0(name, YD_MAIN_WINDOW_UDP_TAB_NAME) == 0) {
        yd_main_window_switch_to_udp(self);
    } else {
        yd_main_window_switch_to_todo(self);
    }
}


static Block1Data *block1_data_ref(Block1Data * _data1_)
{
    g_atomic_int_inc(&_data1_->_ref_count_);
    return _data1_;
}


static void block1_data_unref(void *_userdata_)
{
    Block1Data *_data1_;
    _data1_ = (Block1Data *) _userdata_;
    if (g_atomic_int_dec_and_test(&_data1_->_ref_count_)) {
        YdMainWindow *self;
        self = _data1_->self;
        _g_object_unref0(_data1_->dialog);
        _g_object_unref0(self);
        g_slice_free(Block1Data, _data1_);
    }
}


static void __lambda2_(Block1Data * _data1_, gint response_id)
{
    gboolean _tmp0_ = FALSE;
    gint _tmp1_ = 0;
    gboolean _tmp3_ = FALSE;
    _tmp1_ = response_id;
    if (_tmp1_ == ((gint) GTK_RESPONSE_CANCEL)) {
        _tmp0_ = TRUE;
    } else {
        gint _tmp2_ = 0;
        _tmp2_ = response_id;
        _tmp0_ = _tmp2_ == ((gint) GTK_RESPONSE_DELETE_EVENT);
    }
    _tmp3_ = _tmp0_;
    if (_tmp3_) {
        gtk_widget_hide_on_delete((GtkWidget *) _data1_->dialog);
    }
}


static void ___lambda2__gtk_dialog_response(GtkDialog * _sender,
                                            gint response_id,
                                            gpointer self)
{
    __lambda2_(self, response_id);
}


static void yd_main_window_about_item_activate(YdMainWindow * self)
{
    Block1Data *_data1_;
    GtkAboutDialog *_tmp0_ = NULL;
    gchar *_tmp1_ = NULL;
    gchar *_tmp2_ = NULL;
    gchar **_tmp3_ = NULL;
    gchar **_tmp4_ = NULL;
    gint _tmp4__length1 = 0;
    gchar *_tmp5_ = NULL;
    gchar *_tmp6_ = NULL;
    gchar **_tmp7_ = NULL;
    gchar **_tmp8_ = NULL;
    gint _tmp8__length1 = 0;
    g_return_if_fail(self != NULL);
    _data1_ = g_slice_new0(Block1Data);
    _data1_->_ref_count_ = 1;
    _data1_->self = g_object_ref(self);
    _tmp0_ = (GtkAboutDialog *) gtk_about_dialog_new();
    g_object_ref_sink(_tmp0_);
    _data1_->dialog = _tmp0_;
    gtk_window_set_destroy_with_parent((GtkWindow *) _data1_->dialog,
                                       TRUE);
    gtk_window_set_transient_for((GtkWindow *) _data1_->dialog,
                                 (GtkWindow *) self);
    gtk_window_set_modal((GtkWindow *) _data1_->dialog, TRUE);
    gtk_about_dialog_set_logo_icon_name(_data1_->dialog, "");
    _tmp1_ = g_strdup("Wiky L (wiiiky@yeah.net)");
    _tmp2_ = g_strdup("Xiaoduo Yuan (531657145@qq.com)");
    _tmp3_ = g_new0(gchar *, 2 + 1);
    _tmp3_[0] = _tmp1_;
    _tmp3_[1] = _tmp2_;
    _tmp4_ = _tmp3_;
    _tmp4__length1 = 2;
    gtk_about_dialog_set_authors(_data1_->dialog, (const gchar **) _tmp4_);
    _tmp4_ =
        (_vala_array_free(_tmp4_, _tmp4__length1, (GDestroyNotify) g_free),
         NULL);
    _tmp5_ = g_strdup("Wiky L (wiiiky@yeah.net)");
    _tmp6_ = g_strdup("Xiaoduo Yuan (531657145@qq.com)");
    _tmp7_ = g_new0(gchar *, 2 + 1);
    _tmp7_[0] = _tmp5_;
    _tmp7_[1] = _tmp6_;
    _tmp8_ = _tmp7_;
    _tmp8__length1 = 2;
    gtk_about_dialog_set_documenters(_data1_->dialog,
                                     (const gchar **) _tmp8_);
    _tmp8_ =
        (_vala_array_free(_tmp8_, _tmp8__length1, (GDestroyNotify) g_free),
         NULL);
    gtk_about_dialog_set_artists(_data1_->dialog, NULL);
    gtk_about_dialog_set_translator_credits(_data1_->dialog, NULL);
    gtk_about_dialog_set_program_name(_data1_->dialog, "YD");
    gtk_about_dialog_set_comments(_data1_->dialog,
                                  "袁小朵的攻击检测程序");
    gtk_about_dialog_set_copyright(_data1_->dialog,
                                   "Copyright (C) 2014-2014 Wiky L,Xiaoduo Yuan");
    gtk_about_dialog_set_version(_data1_->dialog, "1.0");
    gtk_about_dialog_set_license_type(_data1_->dialog,
                                      GTK_LICENSE_GPL_3_0);
    gtk_about_dialog_set_wrap_license(_data1_->dialog, TRUE);
    gtk_about_dialog_set_website(_data1_->dialog,
                                 "https://github.com/wiiiky/yd");
    gtk_about_dialog_set_website_label(_data1_->dialog,
                                       "Github Repository");
    g_signal_connect_data((GtkDialog *) _data1_->dialog, "response",
                          (GCallback) ___lambda2__gtk_dialog_response,
                          block1_data_ref(_data1_),
                          (GClosureNotify) block1_data_unref, 0);
    gtk_window_present((GtkWindow *) _data1_->dialog);
    block1_data_unref(_data1_);
    _data1_ = NULL;
}


static void yd_main_window_class_init(YdMainWindowClass * klass)
{
    yd_main_window_parent_class = g_type_class_peek_parent(klass);
    g_type_class_add_private(klass, sizeof(YdMainWindowPrivate));
    G_OBJECT_CLASS(klass)->finalize = yd_main_window_finalize;
}


static void yd_main_window_instance_init(YdMainWindow * self)
{
    self->priv = YD_MAIN_WINDOW_GET_PRIVATE(self);
}


static void yd_main_window_finalize(GObject * obj)
{
    YdMainWindow *self;
    self =
        G_TYPE_CHECK_INSTANCE_CAST(obj, YD_TYPE_MAIN_WINDOW, YdMainWindow);
    _g_object_unref0(self->priv->stack);
    _g_object_unref0(self->priv->tcpview);
    _g_object_unref0(self->priv->udpview);
    G_OBJECT_CLASS(yd_main_window_parent_class)->finalize(obj);
}


GType yd_main_window_get_type(void)
{
    static volatile gsize yd_main_window_type_id__volatile = 0;
    if (g_once_init_enter(&yd_main_window_type_id__volatile)) {
        static const GTypeInfo g_define_type_info =
            { sizeof(YdMainWindowClass), (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) yd_main_window_class_init,
            (GClassFinalizeFunc) NULL,
            NULL, sizeof(YdMainWindow), 0,
            (GInstanceInitFunc) yd_main_window_instance_init, NULL
        };
        GType yd_main_window_type_id;
        yd_main_window_type_id =
            g_type_register_static(GTK_TYPE_WINDOW, "YdMainWindow",
                                   &g_define_type_info, 0);
        g_once_init_leave(&yd_main_window_type_id__volatile,
                          yd_main_window_type_id);
    }
    return yd_main_window_type_id__volatile;
}

static gboolean gtk_tree_model_find_tcp_entry(GtkTreeModel * model,
                                              uint32_t addr, uint16_t port,
                                              GtkTreeIter * iter)
{
    if (gtk_tree_model_get_iter_first(model, iter)) {
        do {
            ProcNetTcpEntry *tcp = NULL;
            uint32_t localaddr;
            uint16_t localport;
            gtk_tree_model_get(model, iter,
                               YD_MAIN_WINDOW_TCP_COLUMNS_POINTER, &tcp,
                               -1);
            if (porc_net_tcp_entry_local(tcp, &localaddr, &localport) == 0
                //&& localaddr == addr
                && localport == port) {
                return TRUE;
            }
        } while (gtk_tree_model_iter_next(model, iter));
    }
    return FALSE;
}

/*
 * 双击TCP列表中的一项
 */
static void yd_main_window_tcpview_activated(GtkTreeView * tcpview,
                                             GtkTreePath * path,
                                             GtkTreeViewColumn * column,
                                             gpointer user_data)
{
    GtkTreeIter iter;
    GtkTreeModel *model = gtk_tree_view_get_model(tcpview);
    if (!gtk_tree_model_get_iter(model, &iter, path)) {
        return;
    }
    ProcNetTcpEntry *tcp = NULL;
    gtk_tree_model_get(model, &iter, YD_MAIN_WINDOW_TCP_COLUMNS_POINTER,
                       &tcp, -1);
    YdTcpDetail *dialog = yd_tcp_detail_new();
    yd_tcp_detail_update(dialog, tcp);
    yd_tcp_detail_show_dialog(dialog);
}

static gboolean yd_main_window_listen_timeout(gpointer data)
{
    YdMainWindow *self = (YdMainWindow *) data;
    GAsyncQueue *queue = self->priv->queue;

    g_async_queue_lock(queue);
    data = g_async_queue_try_pop_unlocked(queue);
    if (data) {                 /* 有数据接受到 */
        guint32 localaddr = (guint32) (long) data;
        guint16 port = (guint16) (long) g_async_queue_pop_unlocked(queue);
        GtkTreeModel *model = gtk_tree_view_get_model(self->priv->tcpview);
        GtkTreeIter iter;
        if (gtk_tree_model_find_tcp_entry(model, localaddr, port, &iter)) {
            GtkListStore *store = (GtkListStore *) model;
            gtk_list_store_set(store, &iter,
                               YD_MAIN_WINDOW_TCP_COLUMNS_FGRGBA, "red",
                               -1);
            gchar buf[100];
            g_snprintf(buf, 100, "端口%u正遭到攻击!!!", port);
            yd_notify("警告", buf);
        }

    }
    g_async_queue_unlock(queue);
    return TRUE;
}

static void _vala_array_destroy(gpointer array, gint array_length,
                                GDestroyNotify destroy_func)
{
    if ((array != NULL) && (destroy_func != NULL)) {
        int i;
        for (i = 0; i < array_length; i = i + 1) {
            if (((gpointer *) array)[i] != NULL) {
                destroy_func(((gpointer *) array)[i]);
            }
        }
    }
}


static void _vala_array_free(gpointer array, gint array_length,
                             GDestroyNotify destroy_func)
{
    _vala_array_destroy(array, array_length, destroy_func);
    g_free(array);
}
