/*
	bumo is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	bumo is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with bumo.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MONITOR_MANAGER_H_
#define MONITOR_MANAGER_H_

#include <proto/cpp/chain.pb.h>
#include <common/network.h>
#include <monitor/system_manager.h>

namespace bumo {
	/* monitor manager */
	class MonitorManager :public utils::Singleton<MonitorManager>,
		public StatusModule,
		public TimerNotify,
		public Network,
		public utils::Runnable {
		friend class utils::Singleton<bumo::MonitorManager>;
	public:
		MonitorManager();
		~MonitorManager();

		/*************************************************
		Function:       Initialize
		Description:    Initialize the variables of this class
		Calls:          StatusModule::RegisterModule; TimerNotify::RegisterModule
		Return:         boolean success or failure
		*************************************************/
		bool Initialize();

		/*************************************************
		Function:       Exit
		Description:    Exit this class
		Calls:          Stop; Join
		Return:         boolean success or failure
		*************************************************/
		bool Exit();

		/*************************************************
		Function:       OnTimer
		Description:    Check the connection of WebSocket
		Calls:          GetClientConnection; Connect
		Input:          current_time The current time
		*************************************************/
		virtual void OnTimer(int64_t current_time) override;

		/*************************************************
		Function:       OnSlowTimer
		Description:    Check alert and send alert
		Calls:          GetLastClosedLedger; GetPeerNodeAddress; GetSystemMonitor; 
		                GetClientConnection; SendRequest
		Input:          current_time The current time
		*************************************************/
		virtual void OnSlowTimer(int64_t current_time) override;

		/*************************************************
		Function:       GetModuleStatus
		Description:    Get the status of monitor
		Input:          data Json::Value The data of status
		*************************************************/
		virtual void GetModuleStatus(Json::Value &data);

		/*************************************************
		Function:       SendMonitor
		Description:    Send the monitor info
		Calls:          GetClientConnection; SendRequest
		Input:          type int64_t The type of monitor
		                data std::string The data of monitor
		Return:         bool success or failure
		*************************************************/
		bool SendMonitor(int64_t type, const std::string &data);
		
	protected:
		virtual void Run(utils::Thread *thread) override;

	private:
		virtual void OnDisconnect(Connection *conn);
		virtual bumo::Connection *CreateConnectObject(bumo::server *server_h, bumo::client *client_,
			bumo::tls_server *tls_server_h, bumo::tls_client *tls_client_h,
			bumo::connection_hdl con, const std::string &uri, int64_t id);

		// Handlers
		bool OnMonitorHello(protocol::WsMessage &message, int64_t conn_id);
		bool OnMonitorRegister(protocol::WsMessage &message, int64_t conn_id);
		bool OnBumoStatus(protocol::WsMessage &message, int64_t conn_id);
		bool OnLedgerStatus(protocol::WsMessage &message, int64_t conn_id);
		bool OnSystemStatus(protocol::WsMessage &message, int64_t conn_id);

		bool GetBumoStatus(monitor::BumoStatus &bumo_status);
		Connection * GetClientConnection();

	private:
		utils::Thread *thread_ptr_;    /* The pointer of the thread */

		std::string monitor_id_;  /* The id of the monitor */

		uint64_t last_connect_time_; /* The last time of connection */
		uint64_t connect_interval_; /* The interval between the time of the two connections */

		uint64_t check_alert_interval_; /* The interval between the time of checking alert */
		uint64_t last_alert_time_; /* The last time of checking alert */

		SystemManager system_manager_; /* The system manager */
	};
}

#endif