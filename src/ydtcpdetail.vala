using Gtk;
using GLib;

namespace Yd{

	public class TcpDetail:Gtk.Dialog{

		private Label local;
		private Label remote;
		private Label state;
		private Label recv_q;
		private Label send_q;

		private Label uid;
		private Label uname;

		private const int DEFAULT_WIDTH=430;
		private const int DEFAULT_HEIGTH=350;

		private const string LOCALADDR_LABEL="Local Address :";
		private const string REMOTEADDR_LABEL="Foreign Address :";
		private const string STATE_LABEL="State :";
		private const string RECV_QUEUE_LABEL="Recv-Queue :";
		private const string SEND_QUEUE_LABEL="Send-Queue :";
		private const string UID_LABEL="Uid :";
		private const string UNAME_LABEL="User Name :";

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
			var tcp_frame=new Frame("<b>TCP</b>");
			(tcp_frame.label_widget as Label).use_markup=true;
			tcp_frame.border_width=5;
			var tcp_grid=new Grid();
			tcp_grid.border_width=10;
			tcp_grid.row_spacing=6;
			tcp_grid.column_spacing=15;
			((Box)content_area).pack_start(tcp_frame,false,false,0);
			tcp_frame.add(tcp_grid);
			var user_frame=new Frame("<b>User</b>");
			(user_frame.label_widget as Label).use_markup=true;
			user_frame.border_width=5;
			((Box)content_area).pack_start(user_frame,false,false,0);
			var user_grid=new Grid();
			user_grid.border_width=10;
			user_grid.row_spacing=6;
			user_grid.column_spacing=15;
			user_frame.add(user_grid);

			var label=new Label(LOCALADDR_LABEL);
			label.halign=Align.START;
			tcp_grid.attach(label,0,0,1,1);
			local=new Label("127.0.0.1:1234");
			local.halign=Align.START;
			local.selectable=true;
			tcp_grid.attach(local,1,0,1,1);

			label=new Label(REMOTEADDR_LABEL);
			label.halign=Align.START;
			tcp_grid.attach(label,0,1,1,1);
			remote=new Label("127.0.0.2:4321");
			remote.halign=Align.START;
			remote.selectable=true;
			tcp_grid.attach(remote,1,1,1,1);

			label=new Label(STATE_LABEL);
			label.halign=Align.START;
			tcp_grid.attach(label,0,2,1,1);
			state=new Label("LISTEN");
			state.halign=Align.START;
			state.selectable=true;
			tcp_grid.attach(state,1,2,1,1);

			label=new Label(RECV_QUEUE_LABEL);
			label.halign=Align.START;
			tcp_grid.attach(label,0,3,1,1);
			recv_q=new Label("0");
			recv_q.halign=Align.START;
			recv_q.selectable=true;
			tcp_grid.attach(recv_q,1,3,1,1);

			label=new Label(SEND_QUEUE_LABEL);
			label.halign=Align.START;
			tcp_grid.attach(label,0,4,1,1);
			send_q=new Label("1");
			send_q.halign=Align.START;
			send_q.selectable=true;
			tcp_grid.attach(send_q,1,4,1,1);

			/*************************/
			label=new Label(UID_LABEL);
			label.halign=Align.START;
			user_grid.attach(label,0,0,1,1);
			uid=new Label("1000");
			uid.halign=Align.START;
			uid.selectable=true;
			user_grid.attach(uid,1,0,1,1);

			label=new Label(UNAME_LABEL);
			label.halign=Align.START;
			user_grid.attach(label,0,1,1,1);
			uname=new Label("wiky");
			uname.halign=Align.START;
			uname.selectable=true;
			user_grid.attach(uname,1,1,1,1);

			content_area.show_all();
		}

	}
	private void tcp_detail_response(Dialog dialog,int reponse_id,
				void *data){
		dialog.destroy();
	}
}
