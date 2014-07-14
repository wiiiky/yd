using Gtk;
using GLib;

namespace Yd{

	public class TcpDetail:Gtk.Dialog{

		private Label local;
		private Label remote;
		private Label state;

		private const int DEFAULT_WIDTH=430;
		private const int DEFAULT_HEIGTH=350;

		public TcpDetail(){
			this.title="Connection Information";
			this.border_width=6;
			set_default_size(DEFAULT_WIDTH,DEFAULT_HEIGTH);
			set_modal(true);
			add_buttons("Close",ResponseType.CLOSE);
			GLib.Signal.connect(this,"response",
						(GLib.Callback)tcp_detail_response,null);

			init_widget();
		}

		private void init_widget(){
			var content_area=get_content_area();
			var frame=new Frame("<b>TCP</b>");
			(frame.label_widget as Label).use_markup=true;
			frame.border_width=5;
			var grid=new Grid();
			grid.border_width=10;
			grid.row_spacing=6;
			grid.column_spacing=15;
			((Container)content_area).add(frame);
			frame.add(grid);

			var label=new Label("Local Address :");
			label.halign=Align.START;
			grid.attach(label,0,0,1,1);
			local=new Label("127.0.0.1:1234");
			local.halign=Align.START;
			local.selectable=true;
			grid.attach(local,1,0,1,1);

			label=new Label("Foreign Address :");
			label.halign=Align.START;
			grid.attach(label,0,1,1,1);
			remote=new Label("127.0.0.2:4321");
			remote.halign=Align.START;
			remote.selectable=true;
			grid.attach(remote,1,1,1,1);

			label=new Label("State :");
			label.halign=Align.START;
			grid.attach(label,0,2,1,1);
			state=new Label("LISTEN");
			state.halign=Align.START;
			state.selectable=true;
			grid.attach(state,1,2,1,1);

			content_area.show_all();
		}

	}
	private void tcp_detail_response(Dialog dialog,int reponse_id,
				void *data){
		dialog.destroy();
	}
}
