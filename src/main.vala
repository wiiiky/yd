using Gtk;

int main(string [] args)
{
	Gtk.init(ref args);

	Yd.TcpDetail dialog=new Yd.TcpDetail();

	dialog.run();

	Gtk.main();
	return 0;
}
