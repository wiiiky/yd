using Gtk;
using Yd;

int main(string[] args){
	Gtk.init(ref args);

	var window=new MainWindow();
	window.show_all();
	Gtk.main();
	return 0;
}
