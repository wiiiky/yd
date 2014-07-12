using Gtk;

namespace Yd{

	public class MainWindow:Window{

		private string tcp_tab_label="  TCP  ";
		private string udp_tab_label="  UDP  ";

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

			Stack stack=new Stack();
			switcher.set_stack(stack);
			stack.set_transition_duration(1000);
			stack.set_transition_type(StackTransitionType.SLIDE_LEFT_RIGHT);
			vbox.pack_start(stack,true,true,0);

			stack.add_titled(new Label("TCP..."),"label1",
						this.tcp_tab_label);
			stack.add_titled(new Label("UDP..."),"label2",
						this.udp_tab_label);
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
