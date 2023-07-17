#pragma once

#include <iostream>
#include <cstdint>
#include <new>
#include <unordered_map>
#include <map>
#include <queue>
#include <list>

namespace gclub
{
	class time_hh_mm {
		private:
			uint8_t hh;
			uint8_t mm;

		public:
		time_hh_mm(uint8_t hh_val = 0, uint8_t mm_val = 0) : 
		hh(hh_val%24), mm(mm_val%60) {};

		void copy_time_to(time_hh_mm &copy_to)
		{ 
			copy_to.hh = hh; 
			copy_to.mm = mm;
		}

		void copy_time_from(const time_hh_mm &copy_from)
		{
			hh = copy_from.hh;
			mm = copy_from.mm;
		}

		void set_time(uint8_t hh_val, uint8_t mm_val)
		{
			hh = hh_val % 24;
			mm = mm_val % 60;
		}
		void append_time(uint8_t hh_val, uint8_t mm_val)
		{
			hh = (hh + hh_val) % 24;
			mm = (mm + mm_val) % 60;
		}

		uint16_t 	get_time_in_mm() { return hh * 60 + mm; }
		uint8_t  	get_time_in_hh() { return mm > 0 ? hh+1 : hh; }
		uint8_t  	get_hh()         { return hh; }
		uint8_t  	get_mm()         { return mm; }

		time_hh_mm  get_delta(const time_hh_mm &start);
		std::string get_string_time();
		void 		set_from_string(std::string time);
	};
	/// end class time_hh_mm

	class desktop 
	{
		private:
			uint32_t  		  id;
			uint32_t  		  daily_revenue;
			bool 	  		  busy;
			time_hh_mm  	  total_busy_tm;

		public:
		desktop() {
				id 			  	= 0;
				daily_revenue 	= 0;
				busy  			= 0;
		};
		desktop& operator=(desktop &);


			void 	 set_id(uint32_t val) 				{ id = val; }
			uint32_t get_id() 					    	{ return id; }

			void 	 append_daily_rev(uint32_t val) 	{ daily_revenue += val; } 
			uint32_t get_daily_rev() 			    	{ return daily_revenue; }

			void 	 set_busy() 						{ busy = 1; }
			void     set_not_busy()						{ busy = 0; }
			bool     is_busy() 					    	{ return busy; }

			time_hh_mm  
					 get_total_busy_tm()				{ return total_busy_tm; }
			std::string   
				 	 get_string_total_busy_tm() 	
			{ return total_busy_tm.get_string_time(); }
			
			void     append_total_busy_tm(uint8_t hh_val, uint8_t mm_val)
			{ total_busy_tm.append_time(hh_val, mm_val); }
	};
	/// end class desktop

	enum client_status {
		NOT_LANDED,
		LANDED,
		WAITING
	};

	class client
	{
		private:
			std::string 	  name;
			uint32_t 		  assigned_dskp_id;
			int 			  status;
			time_hh_mm   	  session_start;

		public:
		client(){
			assigned_dskp_id = 0;
			status 			 = client_status::NOT_LANDED;
		};

			std::string get_name() 				{ return name; }
			void        set_name(std::string &val) 
			{ name.assign(val); }

			int 		get_status()			{ return status; }
			void 		set_status(int new_status) 
			{ status = new_status; }

			uint32_t 	get_assigned_dskp_id() 	{ return assigned_dskp_id; }
			void 		assign_dskp(uint32_t id){ assigned_dskp_id = id; }

			time_hh_mm	get_session_start() 	{ return session_start; }
			std::string   
				 	 	get_string_session_start() 	
			{ return session_start.get_string_time(); }
			void 		set_session_start(time_hh_mm &time)
			{ session_start.copy_time_from(time); }
	};
	/// end class client

	class admin
	{
		private:
			uint32_t   max_dskps;
			uint32_t   nofvacant;
			uint32_t   cost_per_h;
			desktop   *dskps_table;
			time_hh_mm opens_at;
			time_hh_mm closes_at;
			
			std::unordered_map<std::string, client> client_list;
			std::queue<client> 			            client_queue;

		public:
		admin(uint32_t set_max_dskps, uint32_t cost, 
			  time_hh_mm opens, time_hh_mm closes): 
			  	max_dskps(set_max_dskps), cost_per_h(cost), 
			  	opens_at(opens), closes_at(closes)
		{
			dskps_table = new desktop[max_dskps+1];

			for (uint32_t i = 1; i <= max_dskps; i++){
				dskps_table[i].set_id(i);
			}
			nofvacant = max_dskps;
		};

		~admin() { delete[] dskps_table; }
			
			bool 	 dskp_status(uint32_t id) 
			{ return dskps_table[id].is_busy(); }

			void 	 set_dskp_busy(uint32_t id)
			{ dskps_table[id].set_busy(); --nofvacant; }

			void     set_dskp_vacant(uint32_t id)
			{ dskps_table[id].set_not_busy(); ++nofvacant; }

			uint32_t get_nofvacant() {return nofvacant;}
			
			desktop* get_dskp(uint32_t id) 
			{ return id <= max_dskps ? &dskps_table[id] : nullptr; }

			void 	 record_dskp(uint32_t id, time_hh_mm &curr_time, 
							     time_hh_mm &client_session_start)
			{ 
				time_hh_mm delta  =	curr_time.get_delta(client_session_start);

				desktop *dskp_ptr = get_dskp(id);
				dskp_ptr->append_total_busy_tm(delta.get_hh(),
											   delta.get_mm());
				dskp_ptr->append_daily_rev(delta.get_time_in_hh() * cost_per_h);
			}

			void 	report_desktops()
			{
				desktop *ptr;
				time_hh_mm total;
				for (uint32_t id = 1; id <= max_dskps; id++){
					ptr = get_dskp(id);
					total = ptr->get_total_busy_tm();
					std::cout << ptr->get_id() << ' ' << ptr->get_daily_rev() 
						      << ' ' << total.get_string_time() << std::endl;
				}
			}

			time_hh_mm get_open_hrs()  { return opens_at; }
			time_hh_mm get_close_hrs() { return closes_at; }

			bool client_list_isempty() 
			{ return client_list.empty(); }

			bool client_inside(const std::string &name)
			{ return client_list.find(name) != client_list.end(); }

			void append_client_list(const std::string &name, 
					                     const client &new_client)
			{ client_list[name] = new_client; }
			
			bool find_client(client &cl)
			{
				auto memb = client_list.find(cl.get_name());
				if (memb == client_list.end()){
					return 0;
				}
				cl = memb->second;
				return 1;
			}

			void delete_client(std::string &client_name)
			{ client_list.erase(client_name); }

			bool update_client_info(client &cl)
			{
				auto memb = client_list.find(cl.get_name());
				if (memb == client_list.end()){
					return 0;
				}
				memb->second = cl;
				return 1;
			}

			bool add_to_queue(client &cl)
			{
				if (client_queue.size() == max_dskps){
					return 0;
				} 
				client_queue.push(cl);
				return 1;
			}

			bool pull_from_queue(client &cl)
			{
				if (client_queue.empty()){
					return 0;
				}
				cl = client_queue.front();
				client_queue.pop();
				return 1;
			}

			void move_client_list_to(std::map<std::string, client> &ordered_list)
			{
				for(auto mv_client : client_list){
					ordered_list.insert(mv_client);
				}
				client_list.clear();
			}
	};
	/// end class admin

	class event {
		private:
			time_hh_mm  time;
			uint8_t     id;
			std::string body;

		public:
		event(){
			id 	 = 0;
			body = {0};
		}

		void set_id(uint8_t val) 			  { id = val; }
		void set_time(const time_hh_mm &from) { time.copy_time_from(from); }
		void set_body(std::string nbody) 	  { body.assign(nbody); }
		
		uint8_t     get_id()				  { return id; }
		time_hh_mm  get_time() 				  { return time; }
		std::string get_body() 				  { return body; }

	};
	/// end class event

	struct input_params{
		uint32_t 	     max_desktops;
		uint32_t 	 	 cost_per_h;
		time_hh_mm  	 opens_at;
		time_hh_mm 	     closes_at;
		std::list<event> event_list;
	};

	inline void print_event(const std::string &time, 
					 const unsigned &id, const std::string &body);
	inline void split_body(const std::string &body, 
			        std::string &name, uint32_t &deskp);
	
	void end_of_the_day(admin &adm);
	/// OUTPUT events
	void ev13_error(const std::string &time, const std::string &error);
	void ev12_client_landed(time_hh_mm &time, client &cl_land, 
			                uint32_t dskp_id, admin &adm);
	void ev11_client_leave(const std::string &time, 
			               const std::string &client_name);

	/// INPUT events
	void ev4_client_leave(event &ev_leave, admin &adm);
	void ev3_client_wait(event &ev_wait, admin &adm);
	void ev2_client_landed(event &ev_land, admin &adm);
	void ev1_client_came(event &ev_came, admin &adm);
	void handle_event(event &new_event, admin &adm);
};
///end namespace gclub
