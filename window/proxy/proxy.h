#ifndef __PROXY_H_00A__
#define __PROXY_H_00A__


/*
	ip: ����ip
	port: ����port
	type :  0 http����
	        1 ϵͳ����
			2 http��ϵͳ����
*/
int setProxy(const char* ip, int port, int type);

/*
type :  0 http����
		1 ϵͳ����
		2 http��ϵͳ����
*/
int resetProxy(int type);




#endif //__PROXY_H_00A__