using Gtk;
using GLib;

namespace Yd{

	public class MainWindow:Window{

		private const string TCP_TAB_TITLE="  TCP  ";
		private const string UDP_TAB_TITLE="  UDP  ";

		private const string TCP_TAB_NAME="tcp";
		private const string UDP_TAB_NAME="udp";

		private const string TCP_HDR_NO=" No. ";
		private const string TCP_HDR_LOCAL=" Local Address ";
		private const string TCP_HDR_REMOTE=" Foreign Address ";
		private const string TCP_HDR_STATE=" State ";
		private const string TCP_HDR_TRQUEUE=" Send-Q ";
		private const string TCP_HDR_REQUEUE=" Recv-Q ";
		private const string TCP_HDR_UID=" Uid ";

		private enum TcpColumns{
			TCP_COL_NO=0,
			TCP_COL_LOCALADDR,
			TCP_COL_REMOTEADDR,
			TCP_COL_TRQUEUE,
			TCP_COL_REQUEUE,
			TCP_COL_STATE,
			TCP_COL_UID,
		}

		private Stack stack;
		private TreeView tcpview;
		private TreeView udpview;

		public new void show(){
			this.show_all();
		}

		public MainWindow(){
			this.title="Network Detection";
			this.window_position=WindowPosition.CENTER;
			this.destroy.connect(Gtk.main_quit);
			set_default_size(685,500);

			var accel_group=new AccelGroup();
			add_accel_group(accel_group);

			var mainbox=new Box(Orientation.VERTICAL,0);
			add(mainbox);

			var menubar=new MenuBar();
			mainbox.pack_start(menubar,false,false,0);
			var fileitem=new Gtk.MenuItem.with_label("File");
			menubar.add(fileitem);
			var filemenu=new Gtk.Menu();
			fileitem.set_submenu(filemenu);
			var openitem=new Gtk.MenuItem.with_label("Open");
			filemenu.add(openitem);

			var aboutitem=new Gtk.MenuItem.with_mnemonic("_About");
			menubar.add(aboutitem);
			var aboutmenu=new Gtk.Menu();
			aboutitem.set_submenu(aboutmenu);
			var real_aboutitem=new Gtk.MenuItem.with_label("About");
			aboutmenu.add(real_aboutitem);
			real_aboutitem.activate.connect(about_item_activate);
			real_aboutitem.add_accelerator("activate",accel_group,
						'a',Gdk.ModifierType.CONTROL_MASK,
						AccelFlags.VISIBLE);

			var vbox=new Box(Orientation.VERTICAL,5);
			vbox.set_border_width(6);
			mainbox.pack_start(vbox,true,true,0);

			StackSwitcher switcher=new StackSwitcher();
			vbox.pack_start(switcher,false,false,0);

			this.stack=new Stack();
			GLib.Signal.connect_swapped(stack,"notify::visible-child-name",
						(GLib.Callback)stack_name_changed,this);
			switcher.set_stack(stack);
			stack.set_transition_duration(1000);
			stack.set_transition_type(StackTransitionType.SLIDE_LEFT_RIGHT);
			vbox.pack_start(stack,true,true,0);

			ListStore store=new ListStore(9,typeof(string),typeof(string),
						typeof(string),typeof(string),typeof(string),
						typeof(string),typeof(string),
						typeof(Pango.Alignment),typeof(bool));
			this.tcpview=new TreeView.with_model(store);
			CellRendererText cell=new CellRendererText();
			tcpview.insert_column_with_attributes(-1,
						TCP_HDR_NO,
						cell,"text",TcpColumns.TCP_COL_NO);
			tcpview.insert_column_with_attributes(-1,
						TCP_HDR_LOCAL,
						cell,"text",TcpColumns.TCP_COL_LOCALADDR);
			tcpview.insert_column_with_attributes(-1,
						TCP_HDR_REMOTE,
						cell,"text",TcpColumns.TCP_COL_REMOTEADDR);
			tcpview.insert_column_with_attributes(-1,
						TCP_HDR_STATE,cell,
						"text",TcpColumns.TCP_COL_STATE);
			tcpview.insert_column_with_attributes(-1,
						TCP_HDR_TRQUEUE,
						cell,"text",TcpColumns.TCP_COL_TRQUEUE);
			tcpview.insert_column_with_attributes(-1,
						TCP_HDR_REQUEUE,
						cell,"text",TcpColumns.TCP_COL_REQUEUE);
			tcpview.insert_column_with_attributes(-1,
						TCP_HDR_UID,
						cell,"text",TcpColumns.TCP_COL_UID);

			store=new ListStore(6,typeof(string),typeof(string),
						typeof(string),typeof(string),typeof(string),
						typeof(string));
			this.udpview=new TreeView.with_model(store);
			cell=new CellRendererText();
			udpview.insert_column_with_attributes(-1," No. ",cell,"text",0);
			udpview.insert_column_with_attributes(-1," local address ",
						cell,"text",0);
			udpview.insert_column_with_attributes(-1," remote address ",
						cell,"text",0);
			udpview.insert_column_with_attributes(-1," state ",
						cell,"text",0);

			stack.add_titled(tcpview,TCP_TAB_NAME,this.TCP_TAB_TITLE);
			stack.add_titled(udpview,UDP_TAB_NAME,this.UDP_TAB_TITLE);

			stack.add_titled(new Label("TODO"),"todo","TODO");
		}

		private void stack_name_changed(){
			string name=stack.get_visible_child_name();
			if(name==TCP_TAB_NAME){
				ListStore store=(ListStore)tcpview.get_model();
				TreeIter iter;
				store.append(out iter);
				store.set(iter,
							TcpColumns.TCP_COL_NO,"NO",
							TcpColumns.TCP_COL_LOCALADDR,"local address",
							TcpColumns.TCP_COL_REMOTEADDR,"remote address",
							TcpColumns.TCP_COL_STATE,"state",
							TcpColumns.TCP_COL_TRQUEUE,"transmit queue",
							TcpColumns.TCP_COL_REQUEUE,"receive queue",
							TcpColumns.TCP_COL_UID,"uid");
			}
		}

		private void about_item_activate(){
			AboutDialog dialog=new AboutDialog();
			dialog.set_destroy_with_parent(true);
			dialog.set_transient_for(this);
			dialog.set_modal(true);
			dialog.logo_icon_name="";	/* hide logo */

			dialog.authors={"Wiky L(wiiiky@yeah.net)","Xiaoduo Yuan"};
			dialog.documenters={"Wiky L(wiiiky@yeah.net)","Xiaoduo Yuan"};
			dialog.artists=null;
			dialog.translator_credits=null;

			dialog.program_name="YD";
			dialog.comments="袁小朵的攻击检测程序";
			dialog.copyright="Copyright (C) 2014-2014 Wiky L,Xiaoduo Yuan";
			dialog.version="1.0";

			dialog.license_type=License.GPL_3_0;
			dialog.wrap_license=true;
			dialog.website="https://github.com/wiiiky/yd";
			dialog.website_label="Github Repository";

			dialog.response.connect((response_id)=>{
						if(response_id==Gtk.ResponseType.CANCEL||
							response_id==Gtk.ResponseType.DELETE_EVENT){
						dialog.hide_on_delete();
						}
						});

			/* show dialog */
			dialog.present();
		}
	}
}
