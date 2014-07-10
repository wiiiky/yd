using Gtk;

namespace Yd{

	public class MainWindow:Window{
		public MainWindow(){
			this.title="攻击检测";
			this.window_position=WindowPosition.CENTER;
			this.destroy.connect(Gtk.main_quit);
			set_default_size(685,500);

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

			var aboutitem=new Gtk.MenuItem.with_label("About");
			menubar.add(aboutitem);
			var aboutmenu=new Gtk.Menu();
			aboutitem.set_submenu(aboutmenu);
			var helpitem=new Gtk.MenuItem.with_label("Help");
			aboutmenu.add(helpitem);

			var vbox=new Box(Orientation.VERTICAL,5);
			mainbox.pack_start(vbox,true,true,0);
			var notebook=new Notebook();
			vbox.pack_start(notebook,true,true,0);

			var title=new Label(" Processes ");
			var content=new Label("Content!!!!!!!");
			notebook.append_page(content,title);

			title=new Label(" Resources ");
			content=new Label("Content!!!!!!");
			notebook.append_page(content,title);
		}
	}
}
