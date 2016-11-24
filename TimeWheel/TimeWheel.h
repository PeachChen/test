#ifndef TIMEWHEEL_H_
#define TIMEWHEEL_H_
#include "DefineHeader.h"
#include <string>

using namespace std;


class CTimeWheel
{
	public:
		struct timer_node
		{
			timer_node* next;
			int expire;
			string outstr;
		};

		struct link_list
		{
			timer_node head;
			timer_node* tail;
		};

		struct timer_mgr
		{
			link_list near[256];
			link_list t[4][63];
			int time;
			uint32 current;
			uint32 starttime;
		};

		SINGLETON_NODE(CTimeWheel);
		
		void Init();

		void create_add_node(int expire,string str);
	
		void update();
	private:
		timer_mgr tm;
		
		CTimeWheel();
		~CTimeWheel();
		
		timer_node* link_clear(link_list *list)
		{
			timer_node* ret=list->head.next;
		 	list->head.next=NULL;
			list->tail=&(list->head);
			return ret;
		}

		void link(link_list* list,timer_node* node)
		{
			list->tail->next=node;
			list->tail=node;
			node->next=NULL;
		}
		void add_node(timer_node *node);
		void timer_execute();
		//测试用
		void timeout(string str);
};
#endif //TIME_WHEEL_H
